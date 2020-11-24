#pragma once

#include <array>
#include <bitset>

namespace tictactoe {

extern std::array<bool,512> LU_TABLE;

template<size_t n>
int calculate_result(const std::bitset<n>& board) {
  std::bitset<n> mask = 0x1FF;
  int x_wins = LU_TABLE[(board&mask).to_ulong()];
  int o_wins = LU_TABLE[((board>>9)&mask).to_ulong()];
  return x_wins - o_wins;
}

}
