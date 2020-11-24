#pragma once

#include <random>
#include <utility>

#include "common.hpp"

namespace mcts {

template<class RandomPtr>
struct RandomPolicy {
  RandomPtr rng;

  RandomPolicy(RandomPtr rng) : rng(std::move(rng)) {}

  template<class Environment>
  int operator()(const Environment&, const ActionVector<Environment>& actions) {
    std::uniform_int_distribution<> dist(0, actions.size()-1);
    return dist(*rng);
  }
};

} // mcts
