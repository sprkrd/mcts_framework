#include "tictactoe_utils.hpp"

namespace tictactoe {

namespace {

template<typename T, int nrows, int ncols>
using Table2d = std::array<std::array<T, ncols>, nrows>;

bool is_winning_configuration(Board board) {
  static const Table2d<int, 8, 3> LINES{{
    {0,1,2}, {3,4,5}, {6,7,8}, {0,3,6}, {1,4,7}, {2,5,8}, {0,4,8}, {2,4,6}}};
  for (const auto& line : LINES) {
    if (board[line[0]] && board[line[1]] && board[line[2]])
      return true;
  }
  return false;
}

std::array<bool, 512> compute_winning_lookup_table() {
  std::array<bool, 512> lu_table;
  for (unsigned board = 0; board < 512; ++board)
    lu_table[board] = is_winning_configuration(board);
  return lu_table;
}

} // anonymous ns

std::array<bool,512> LU_TABLE =
  compute_winning_lookup_table();

Result calculate_result(Board board) {
  Board mask = 0x1FF;
  bool x_wins = LU_TABLE[(board&mask).to_ulong()];
  bool o_wins = LU_TABLE[((board>>9)&mask).to_ulong()];
  int score = x_wins - o_wins;
  bool tie = !score && board.count() == 9;
  return static_cast<Result>(score - 2*tie);
}

char get_char_representation(Board board, int cell) {
  char content = ' ';
  if (board[cell])
    content = 'x';
  else if (board[9+cell])
    content = 'o';
  return content;
}

} // tictactoe
