#include "tictactoe_utils.hpp"
#include "utils.hpp"

#include "ultimate_tictactoe.hpp"

using ultimate_tictactoe::Board;
using tictactoe::calculate_result;
using mcts::check_action;

namespace ultimate_tictactoe {

Environment::Environment() {
  reset();
}

std::vector<Action> Environment::get_available_actions() const {
  std::vector<Action> available_actions;
  available_actions.reserve(81);
  return available_actions;
}

bool Environment::is_terminal() const {
  return m_score[0] + m_score[1] != 0;
}

const Reward& Environment::step(const Action& action, bool check) {
  if (check)
    check_action(*this, action);
  return m_score;
}

void Environment::reset() {
  m_state = State();
  m_macro_view = Subboard();
  m_score = Reward();
  m_turn = m_current_player = 0;
}

bool operator==(const State& lhs, const State& rhs) {
  return lhs.board == rhs.board && lhs.active_subboard == rhs.active_subboard;
}

bool operator==(const Action& lhs, const Action& rhs) {
  return lhs.cell == rhs.cell;
}

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

char cell_to_char(const Board& board, int i, int j) {
  auto[subboard, cell] = to_subboard_cell(i, j);
  char cell_content = ' ';
  if (board[18*subboard+cell])
    cell_content = 'x';
  else if (board[18*subboard+9+cell])
    cell_content = 'o';
  return cell_content;
}

}

std::ostream& operator<<(std::ostream& out, const State& state) {
  out << "  0 1 2   3 4 5   6 7 8\n";
  for (unsigned i = 0; i < 9; ++i) {
    
    if (i > 0 && i%3 == 0) {
      out << "        |       |       \n"
          << " -----------------------\n";
    }
    for (unsigned j = 0; j < 9; ++j) {
      if (j == 0)
        out << i << ' ';
      else if (j%3 == 0)
        out << " | ";
      else
        out << '|';
      char content = cell_to_char(state.board, i, j);
      out << content;
    }
    out << '\n';
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
  mcts::hash_combine(seed, state.board);
  mcts::hash_combine(seed, state.active_subboard);
  return seed;
}
