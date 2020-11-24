#pragma once

#include <queue>
#include <utility>
#include <vector>

#include "array_operations.hpp"
#include "common.hpp"
#include "select.hpp"

namespace mcts {

struct SampleAverage {};
struct RunningAverage {};
struct ExponentialAverage {};

template<class Environment, class Tag>
struct UpdateMethod;

template<class Environment>
struct UpdateMethod<Environment, SampleAverage> {
  typedef ActionInfoBase<Environment> ActionInfo;

  void operator()(ActionInfo& action_info, const Reward<Environment>& target) const {
    double step = 1.0 / action_info.visits;
    action_info.expected_return += step*(target - action_info.expected_return);
  }
};

template<class Environment>
struct UpdateMethod<Environment, RunningAverage> {
  struct ActionInfo : ActionInfoBase<Environment> {
    std::queue<Reward<Environment>> window;
  };

  unsigned max_window_size;

  UpdateMethod(unsigned max_window_size = 10) : max_window_size(max_window_size) {}

  void operator()(ActionInfo& action_info, const Reward<Environment>& target) const {
    action_info.window.push(target);
    if (action_info.window.size() <= max_window_size) {
      double step = 1.0 / action_info.window.size();
      action_info.expected_return += step*(target - action_info.expected_return);
    }
    else {
      double step = 1.0 / max_window_size;
      action_info.expected_return += step*(target - action_info.window.front());
      action_info.window.pop();
    }
  }
};

template<class Environment>
struct UpdateMethod<Environment, ExponentialAverage> {
  typedef ActionInfoBase<Environment> ActionInfo;

  double step;

  UpdateMethod(double step = 0.1) : step(step) {}

  void operator()(ActionInfo& action_info, const Reward<Environment>& target) const {
    action_info.expected_return += step*(target - action_info.expected_return);
  }
};

template<class Environment, class Tag>
struct StandardBackup {
  typedef UpdateMethod<Environment,Tag> UpdateMethodType;
  typedef NodeBase<typename UpdateMethodType::ActionInfo> Node;

  double discount;
  UpdateMethodType update;

  StandardBackup(double discount = 1) : discount(discount) {}

  template<class... Args>
  StandardBackup(double discount, Args&&... args) :
    discount(discount), update(std::forward<Args>(args)...) {
  }

  template<class Memory>
  void operator()(
      Memory& memory,
      const TreePath<Environment>& tree_path,
      const RewardVector<Environment>& rewards) const {
    Reward<Environment> acc_reward{0};
    for (int i = rewards.size()-1; i >= tree_path.size(); --i)
      acc_reward = rewards[i] + discount*acc_reward;
    for (int i = tree_path.size()-1; i >= 0; --i) {
      const auto&[state, action_index] = tree_path[i];
      auto& node = memory.find(state)->second;
      auto& action_info = node.action_vector[action_index];
      ++node.visits;
      ++action_info.visits;
      acc_reward = rewards[i] + discount*acc_reward;
      update(action_info, acc_reward);
    }
  }
};


template<class Environment, class Tag>
struct SarsaBackup {
  typedef UpdateMethod<Environment,Tag> UpdateMethodType;
  typedef NodeBase<typename UpdateMethodType::ActionInfo> Node;

  double discount;
  UpdateMethodType update;

  SarsaBackup(double discount = 1) : discount(discount) {}

  template<class... Args>
  SarsaBackup(double discount, Args&&... args) :
    discount(discount), update(std::forward<Args>(args)...) {
  }

  template<class Memory>
  void operator()(
      Memory& memory,
      const TreePath<Environment>& tree_path,
      const RewardVector<Environment>& rewards) const {
    Reward<Environment> td_target{0};
    for (int i = rewards.size()-1; i >= tree_path.size(); --i)
      td_target = rewards[i] + discount*td_target;
    for (int i = tree_path.size()-1; i >= 0; --i) {
      const auto&[state, action_index] = tree_path[i];
      auto& node = memory.find(state)->second;
      auto& action_info = node.action_vector[action_index];
      ++node.visits;
      ++action_info.visits;
      update(action_info, rewards[i] + discount*td_target);
      td_target = action_info.expected_return;
    }
  }
};

template<class Environment, class Tag>
struct QlearnBackup {
  typedef UpdateMethod<Environment,Tag> UpdateMethodType;
  typedef NodeBase<typename UpdateMethodType::ActionInfo> Node;

  double discount;
  UpdateMethodType update;

  QlearnBackup(double discount = 1) : discount(discount) {}

  template<class... Args>
  QlearnBackup(double discount, Args&&... args) :
    discount(discount), update(std::forward<Args>(args)...) {
  }

  template<class Memory>
  void operator()(
      Memory& memory,
      const TreePath<Environment>& tree_path,
      const RewardVector<Environment>& rewards) const {
    GreedySelect greedy_select;
    Reward<Environment> td_target{0};
    for (int i = rewards.size()-1; i >= tree_path.size(); --i)
      td_target = rewards[i] + discount*td_target;
    for (int i = tree_path.size()-1; i >= 0; --i) {
      const auto&[state, action_index] = tree_path[i];
      auto& node = memory.find(state)->second;
      auto& action_info = node.action_vector[action_index];
      ++node.visits;
      ++action_info.visits;
      update(action_info, rewards[i] + discount*td_target);
      int argmax = greedy_select(node);
      td_target = node.action_vector[argmax].expected_return;
    }
  }
};


} // mcts
