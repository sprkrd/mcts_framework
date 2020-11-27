#include "utils.hpp"

#include "ultimate_tictactoe.hpp"

using tictactoe::Result;

namespace ultimate_tictactoe {

namespace {

std::pair<int,int> to_subboard_cell(int i, int j) {
  int subboard = i - i%3 + j/3;
  int cell = (i%3)*3 + j%3;
  return {subboard, cell};
}

std::pair<int,int> to_row_col(int subboard, int cell) {
  int offset_i = subboard - subboard%3;
  int offset_j = (subboard%3)*3;
  return {offset_i + cell/3, offset_j + cell%3};
}

char get_char_representation(const State& state, int i, int j) {
  auto[subboard, cell] = to_subboard_cell(i, j);
  return tictactoe::get_char_representation(state.subboards[subboard], cell);
}

char get_subboard_status_char(const State& state, int subboard) {
  switch (tictactoe::calculate_result(state.subboards[subboard])) {
    case Result::tie:
      return 'T';
    case Result::o_wins:
      return 'O';
    case Result::ongoing:
      return state.active_subboard == -1 || state.active_subboard == subboard? '*' : ' ';
    default: //case Result::x_wins:
      return 'X';
  }
}

void get_available_actions_helper(
    const State& state,
    int subboard,
    std::vector<Action>& available_actions
) {
  for (int cell = 0; cell < 9; ++cell) {
    if (tictactoe::is_free_cell(state.subboards[subboard], cell))
      available_actions.push_back({subboard, cell});
  }
}

} // anonymous ns

Environment::Environment() {
  reset();
}

std::vector<Action> Environment::get_available_actions() const {
  std::vector<Action> available_actions;
  if (m_state.active_subboard == -1) {
    available_actions.reserve(81);
    for (int subboard = 0; subboard < 9; ++subboard)
      if (m_playable_subboards[subboard])
        get_available_actions_helper(m_state, subboard, available_actions);
  }
  else {
    available_actions.reserve(9);
    get_available_actions_helper(m_state, m_state.active_subboard, available_actions);
  }
  return available_actions;
}

bool Environment::is_terminal() const {
  return m_score[0] || m_score[1];
}

const Reward& Environment::step(const Action& action, bool check) {
  if (check)
    mcts::check_action(*this, action);
  tictactoe::make_move(m_state.subboards[action.subboard],
      action.cell, m_current_player);
  switch (tictactoe::calculate_result(m_state.subboards[action.subboard])) {
    case Result::tie:
      m_playable_subboards[action.subboard] = false;
      update_score();
      break;
    case Result::o_wins:
      m_playable_subboards[action.subboard] = false;
      m_o_winned_subboards[action.subboard] = true;
      update_score();
      break;
    case Result::x_wins:
      m_playable_subboards[action.subboard] = false;
      m_x_winned_subboards[action.subboard] = true;
      update_score();
    default:
      break;
  }
  ++m_turn;
  m_current_player = !m_current_player;
  m_state.active_subboard = m_playable_subboards[action.cell]? action.cell : -1;
  return m_score;
}

void Environment::reset() {
  for (auto& subboard : m_state.subboards)
    subboard.reset();
  m_state.active_subboard = -1;
  m_x_winned_subboards.reset();
  m_o_winned_subboards.reset();
  m_playable_subboards.set();
  m_score = Reward();
  m_turn = m_current_player = 0;
}

void Environment::update_score() {
  using tictactoe::LU_TABLE;
  bool x_ttt = LU_TABLE[m_x_winned_subboards.to_ulong()];
  bool o_ttt = LU_TABLE[m_o_winned_subboards.to_ulong()];
  bool more_plays = m_playable_subboards.any();
  int x_win_count = m_x_winned_subboards.count();
  int o_win_count = m_o_winned_subboards.count();
  bool x_wins = x_ttt || (!more_plays && !o_ttt && x_win_count>o_win_count);
  bool o_wins = o_ttt || (!more_plays && !x_ttt && o_win_count>x_win_count);
  bool tie = !x_wins && !o_wins && !more_plays;
  m_score[0] = x_wins + 0.5*tie;
  m_score[1] = o_wins + 0.5*tie;
}

bool operator==(const State& lhs, const State& rhs) {
  return lhs.active_subboard == rhs.active_subboard && lhs.subboards == rhs.subboards;
}

bool operator==(const Action& lhs, const Action& rhs) {
  return lhs.subboard == rhs.subboard && lhs.cell == rhs.cell;
}

std::ostream& operator<<(std::ostream& out, const State& state) {
  out << "  0 1 2   3 4 5   6 7 8\n";
  for (unsigned i = 0; i < 9; ++i) {
    if (i%3 == 0) {
      if (i > 0)
        out << " -----------------------\n";
      out << "        |       |       \n";
    }
    else
      out << "  ----- | ----- | ----- \n";
    for (unsigned j = 0; j < 9; ++j) {
      if (j == 0)
        out << i << ' ';
      else if (j%3 == 0)
        out << " | ";
      else
        out << '|';
      out << get_char_representation(state, i, j);
    }
    out << '\n';
    if (i%3 == 2)
      out << "    "    << get_subboard_status_char(state, i-i%3)
          << "   |   " << get_subboard_status_char(state, i-i%3+1)
          << "   |   " << get_subboard_status_char(state, i-i%3+2)
          << "   \n";
  }
  return out;
}

std::ostream& operator<<(std::ostream& out, const Action& action) {
  auto[i,j] = to_row_col(action.subboard, action.cell);
  return out << i << ' ' << j;
}

std::istream& operator>>(std::istream& in, Action& action) {
  int row, col;
  in >> row >> col;
  std::tie(action.subboard, action.cell) = to_subboard_cell(row, col);
  return in;
}

} // ultimate_tictactoe

std::size_t std::hash<ultimate_tictactoe::State>::operator()(const State& state) const {
  std::size_t seed = 0;
  mcts::hash_combine(seed, state.active_subboard);
  for (auto subboard : state.subboards)
    mcts::hash_combine(seed, subboard);
  return seed;
}

