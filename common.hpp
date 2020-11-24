#pragma once

#include <ostream>
#include <utility>
#include <vector>

namespace mcts {

template<class Environment>
using State = typename Environment::State;

template<class Environment>
using Action = typename Environment::Action;

template<class Environment>
using Reward = typename Environment::Reward;

template<class Environment>
using StateIntPair = std::pair<State<Environment>,int>;

template<class Environment>
using TreePath = std::vector<StateIntPair<Environment>>;

template<class Environment>
using ActionVector = std::vector<Action<Environment>>;

template<class Environment>
using RewardVector = std::vector<Reward<Environment>>;

template<class Environment>
struct ActionInfoBase {
  Reward<Environment> expected_return;
  Action<Environment> action;
  int visits;
};

template<class ActionInfo>
struct NodeBase {
  std::vector<ActionInfo> action_vector;
  int maximizing_player, visits;

  double get_action_value(int action_index) const {
    return action_vector[action_index].expected_return[maximizing_player];
  }

  template<class Environment>
  void init(const Environment& environment) {
    visits = 0;
    maximizing_player = environment.get_current_player();
    auto available_actions = environment.get_available_actions();
    action_vector.reserve(available_actions.size());
    for (auto& action : available_actions) {
      action_vector.emplace_back();
      action_vector.back().action = action;
    }
  }
};

template<class ActionInfo>
std::ostream& operator<<(std::ostream& out, const NodeBase<ActionInfo>& node) {
  out << "visits: " << node.visits << '\n'
      << "maximizing player: " << node.maximizing_player << '\n'
      << "actions:";
  if (node.action_vector.empty())
    out << "\n    none";
  else {
    for (unsigned i = 0; i < node.action_vector.size(); ++i) {
      out << "\n    " << node.action_vector[i].action
          << ", value: " << node.get_action_value(i)
          << ", visits: " << node.action_vector[i].visits;
    }
  }
  return out;
}

} // mcts
