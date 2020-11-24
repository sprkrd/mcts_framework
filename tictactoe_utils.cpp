#include "tictactoe_utils.hpp"

namespace {

template<typename T, int nrows, int ncols>
using Table2d = std::array<std::array<T, ncols>, nrows>;

bool is_winning_configuration(const std::bitset<18>& board) {
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

std::array<bool,512> tictactoe::LU_TABLE =
  compute_winning_lookup_table();
