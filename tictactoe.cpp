#include "tictactoe_utils.hpp"
#include "utils.hpp"

#include "tictactoe.hpp"

using tictactoe::Board;
using tictactoe::calculate_result;
using mcts::check_action;

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
    if (!m_state.board[i] && !m_state.board[9+i])
      available_actions.push_back({i});
  }
  return available_actions;
}

int Environment::get_current_player() const {
  return get_turn()%2;
}

bool Environment::is_terminal() const {
  return get_turn() == 9 || calculate_result(m_state.board);
}

Reward Environment::get_score() const {
  int result = calculate_result(m_state.board);
  if (result)
    return {double(result==1), double(result==-1)};
  return {0.5*is_terminal(), 0.5*is_terminal()};
}

Reward Environment::step(const Action& action, bool check) {
  if (check)
    check_action(*this, action);
  int current_player = get_current_player();
  m_state.board[9*current_player + action.cell] = true;
  return get_score();
}

void Environment::reset() {
  m_state.board = 0;
}

bool operator==(const State& lhs, const State& rhs) {
  return lhs.board == rhs.board;
}

bool operator==(const Action& lhs, const Action& rhs) {
  return lhs.cell == rhs.cell;
}

std::ostream& operator<<(std::ostream& out, const State& state) {
  int number_of_filled_cells = state.board.count();
  int result = calculate_result(state.board);
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
    if (state.board[i])
      out << 'x';
    else if (state.board[9+i])
      out << 'o';
    else
      out << ' ';
  }
  if (result == 1)
    out << "\nx wins";
  else if (result == -1)
    out << "\no wins";
  else if (number_of_filled_cells == 9)
    out << "\nis a draw";
  return out;
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
  std::hash<Board> hasher;
  return hasher(state.board);
}
