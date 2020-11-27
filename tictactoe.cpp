#include "utils.hpp"

#include "tictactoe.hpp"


namespace tictactoe {

Environment::Environment() : m_state{0} {

}

int Environment::get_turn() const {
  return m_state.board.count();
}

std::vector<Action> Environment::get_available_actions() const {
  std::vector<Action> available_actions;
  available_actions.reserve(9);
  for (int i = 0; i < 9; ++i) {
    if (is_free_cell(m_state.board, i))
      available_actions.push_back({i});
  }
  return available_actions;
}

int Environment::get_current_player() const {
  return get_turn()%2;
}

bool Environment::is_terminal() const {
  return calculate_result(m_state.board) != Result::ongoing;
}

Reward Environment::get_score() const {
  switch (calculate_result(m_state.board)) {
    case Result::tie:
      return {0.5,0.5};
    case Result::o_wins:
      return {0,1};
    case Result::ongoing:
      return {0,0};
    default: //case Result::x_wins:
      return {1,0};
  }
}

Reward Environment::step(const Action& action, bool check) {
  if (check)
    mcts::check_action(*this, action);
  make_move(m_state.board, action.cell, get_current_player());
  return get_score();
}

void Environment::reset() {
  m_state.board.reset();
}

bool operator==(const State& lhs, const State& rhs) {
  return lhs.board == rhs.board;
}

bool operator==(const Action& lhs, const Action& rhs) {
  return lhs.cell == rhs.cell;
}

std::ostream& operator<<(std::ostream& out, const State& state) {
  out << "  0 1 2\n";
  for (int i = 0; i < 9; ++i) {
    int row = i/3;
    int col = i%3;
    if (col == 0) {
      if (row > 0)
        out << "\n  -----\n";
      out << row << ' ';
    }
    else if (col > 0)
      out << '|';
    out << get_char_representation(state.board, i);
  }
  switch (calculate_result(state.board)) {
    case Result::tie:
      return out << "\ntie";
    case Result::o_wins:
      return out << "\no wins";
    case Result::ongoing:
      return out;
    default: //case Result::x_wins:
      return out << "\nx wins";
  }
}

std::ostream& operator<<(std::ostream& out, const Action& action) {
  int row = action.cell/3;
  int col = action.cell%3;
  return out << row << ' ' << col;
}

std::istream& operator>>(std::istream& in, Action& action) {
  int row, col;
  in >> row >> col;
  action.cell = row*3 + col;
  return in;
}

} // tictactoe

size_t std::hash<tictactoe::State>::operator()(const State& state) const {
  std::hash<tictactoe::Board> hasher;
  return hasher(state.board);
}
