#pragma once

#include <array>
#include <bitset>

namespace tictactoe {

extern std::array<bool,512> LU_TABLE;

typedef std::bitset<3*3*2> Board;

enum class Result {tie=-2, o_wins, ongoing, x_wins};

inline bool is_free_cell(Board board, int cell) {
  return !board[cell] && !board[9+cell];
}

inline void make_move(Board& board, int cell, int player) {
  board.set(cell+9*player);
}

Result calculate_result(Board board);

char get_char_representation(Board board, int cell);

}
