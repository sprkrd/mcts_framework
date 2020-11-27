#pragma once

#include <algorithm>
#include <cstdint>
#include <functional>
#include <sstream>
#include <stdexcept>

namespace mcts {

template<class Environment>
void check_action(const Environment& env, const typename Environment::Action& action) {
  auto available_actions = env.get_available_actions();
  bool valid_action = std::find(available_actions.begin(),
      available_actions.end(), action) != available_actions.end();
  if (!valid_action) {
    std::ostringstream oss;
    oss << "Action " << action << " is not valid";
    throw std::invalid_argument(oss.str());
  }
}

template <typename T>
inline void hash_combine(std::size_t& seed, const T& val) {
  static_assert(sizeof(std::size_t) == 4 || sizeof(std::size_t) == 8);
  if constexpr(sizeof(std::size_t) == 4)
    seed ^= std::hash<T>{}(val) + 0x9e3779b9U + (seed<<6) + (seed>>2);
  else
    seed ^= std::hash<T>{}(val) + 0x9e3779b97f4a7c15ULL + (seed<<12) + (seed>>4);
}

}
