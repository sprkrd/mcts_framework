#pragma once

#include <cmath>
#include <limits>
#include <random>
#include <utility>

#include "common.hpp"

namespace mcts {

struct GreedySelect {
  template<class Node>
  int operator()(const Node& node) const {
    int argmax = -1;
    double max = -std::numeric_limits<double>::infinity();
    for (unsigned i = 0; i < node.action_vector.size(); ++i) {
      double value = node.get_action_value(i);
      if (value > max) {
        argmax = i;
        max = value;
      }
    }
    return argmax;
  }
};

struct MostVisitedSelect {
  template<class Node>
  int operator()(const Node& node) const {
    int argmax = -1;
    int max = -1;
    for (unsigned i = 0; i < node.action_vector.size(); ++i) {
      int visits = node.action_vector[i].visits;
      if (visits > max) {
        argmax = i;
        max = visits;
      }
    }
    return argmax;
  }
};

template<class RandomPtr>
struct EpsilonGreedySelect {
  double epsilon0, decay;
  RandomPtr rng;

  EpsilonGreedySelect(RandomPtr rng, double epsilon0 = 0.1, double decay = 0) :
    epsilon0(epsilon0), decay(decay), rng(std::move(rng)) {
  }

  template<class Node>
  int operator()(const Node& node) const {
    std::uniform_real_distribution<> unif_real(0,1);
    double epsilon = epsilon0 / (1 + decay*node.visits);
    if (unif_real(*rng) < epsilon) {
      std::uniform_int_distribution<> unif_int(0,node.action_vector.size()-1);
      return unif_int(*rng);
    }
    GreedySelect greedy_select;
    return greedy_select(node);
  }
};

struct UctSelect {
  double c;

  UctSelect(double c) : c(c) {}

  template<class Node>
  int operator()(const Node& node) const {
    double max = -std::numeric_limits<double>::infinity();
    int argmax = -1;
    for (unsigned i = 0; i < node.action_vector.size(); ++i) {
      const auto& action_info = node.action_vector[i];
      if (not action_info.visits)
        return i;
      double score = node.get_action_value(i) +
        c*std::sqrt(std::log(node.visits)/action_info.visits);
      if (score > max) {
        max = score;
        argmax = i;
      }
    }
    return argmax;
  }
};

} // mcts
