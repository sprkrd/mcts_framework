#pragma once

#include <ostream>
#include <chrono>
#include <memory>

#include "backup.hpp"
#include "common.hpp"
#include "default_policy.hpp"
#include "memory_utils.hpp"
#include "select.hpp"

namespace mcts {

struct Statistics {
  double elapsed_last_call, elapsed_per_call,
         time_per_simulation, time_per_simulation_last;
  int number_of_calls, max_episode_length,
      number_of_simulations, number_of_simulations_last;

  void update_episode_length(int length) {
    if (length > max_episode_length)
      max_episode_length = length;
  }

  void update(int nsim, double elapsed) {
    number_of_calls++;
    number_of_simulations += nsim;
    number_of_simulations_last = nsim;

    elapsed_last_call = elapsed;
    elapsed_per_call += (elapsed - elapsed_per_call)/number_of_calls;
    time_per_simulation_last = elapsed/nsim;
    time_per_simulation += (double(nsim)/number_of_simulations)*
      (time_per_simulation_last - time_per_simulation);
  }
};

std::ostream& operator<<(std::ostream& out, const Statistics& stats) {
  out << "Last call\n"
      << "---------\n"
      << "Elapsed: " << (stats.elapsed_last_call*1000) << "ms\n"
      << "Number of simulations: " << stats.number_of_simulations_last << '\n'
      << "Time per simulation: " << (stats.time_per_simulation_last*1000) << "ms\n\n"
      << "Overall\n"
      << "-------\n"
      << "Number of calls: " << stats.number_of_calls << '\n'
      << "Time per call: " << (stats.elapsed_per_call*1000) << "ms\n"
      << "Number of simulations: " << stats.number_of_simulations << '\n'
      << "Time per simulation: " << (stats.time_per_simulation*1000) << "ms\n"
      << "Maximum episode length: " << stats.max_episode_length;
    return out;
}

template<class Environment>
class MctsBase {
  public:
    MctsBase() : m_statistics() {}

    virtual Action<Environment> search(
      const Environment& env,
      std::ostream* log = nullptr,
      double timeout_s = -1,
      int simulation_limit = -1
    ) = 0;

    const Statistics& get_statistics() const {
      return m_statistics;
    }

    virtual void reset() { }

    virtual ~MctsBase() = default;

  protected:
    Statistics m_statistics;
};

template< class Environment,
          class Select,
          class DefaultPolicy,
          class Backup >
class Mcts : public MctsBase<Environment> {
  public:
    Mcts(
      Select select,
      DefaultPolicy default_policy,
      Backup backup,
      int memory_capacity = 3000000
    ) :
      m_select(std::move(select)),
      m_default_policy(std::move(default_policy)),
      m_backup(std::move(backup)),
      m_memory(memory_capacity) {}

    virtual Action<Environment> search(
      const Environment& env,
      std::ostream* log = nullptr,
      double timeout_s = -1,
      int simulation_limit = -1
    ) override {
      using namespace std::chrono;
      if (timeout_s < 0)
        timeout_s = std::numeric_limits<double>::infinity();
      if (simulation_limit < 0)
        simulation_limit = std::numeric_limits<int>::max();
      duration<double> timeout(timeout_s), elapsed(0.0);
      auto start = steady_clock::now();
      int number_of_simulations = 0;
      do {
        single_pass(env);
        ++number_of_simulations;
        elapsed = steady_clock::now() - start;
      } while (number_of_simulations < simulation_limit &&
               elapsed < timeout);
      this->m_statistics.update(number_of_simulations, elapsed.count());
      MostVisitedSelect select;
      const Node& root = m_memory.find(env.get_state())->second;
      int argmax = select(root);
      if (log) {
        *log << this->m_statistics << "\n\n"
             << "Current node\n"
             << "------------\n"
             << root << '\n';
      }
      return root.action_vector[argmax].action;
    }

    virtual void reset() override {
      m_memory.clear();
    }

  private:
    typedef typename Backup::Node Node;

    void single_pass(Environment sandbox) {
      TreePath<Environment> tree_path;
      RewardVector<Environment> rewards;
      tree_path.reserve(this->m_statistics.max_episode_length);
      rewards.reserve(this->m_statistics.max_episode_length);
      tree_sim(sandbox, tree_path, rewards);
      default_sim(sandbox, rewards);
      m_backup(m_memory, tree_path, rewards);
      this->m_statistics.update_episode_length(sandbox.get_turn());
    }

    Node& expand(const Environment& env) {
      Node& node = m_memory[env.get_state()];
      node.init(env);
      return node;
    }

    void tree_sim(
      Environment& sandbox,
      TreePath<Environment>& tree_path,
      RewardVector<Environment>& rewards
    ) {
      bool leaf_or_terminal = sandbox.is_terminal();
      while (!leaf_or_terminal) {
        Node* node;
        auto it = m_memory.find(sandbox.get_state());
        if (it == m_memory.end()) {
          node = &expand(sandbox);
          leaf_or_terminal = true;
        }
        else
          node = &(it->second);
        int selected = m_select(*node);
        tree_path.emplace_back(sandbox.get_state(), selected);
        rewards.push_back(sandbox.step(node->action_vector[selected].action));
        leaf_or_terminal = leaf_or_terminal || sandbox.is_terminal();
      }
    }

    void default_sim(
      Environment& sandbox,
      RewardVector<Environment>& rewards
    ) {
      while (!sandbox.is_terminal()) {
        auto available_actions = sandbox.get_available_actions();
        int selected = m_default_policy(sandbox, available_actions);
        rewards.push_back(sandbox.step(available_actions[selected]));
      }
    }

    Select m_select;
    DefaultPolicy m_default_policy;
    Backup m_backup;
    memory::LruMap<State<Environment>,Node> m_memory;
    //std::unordered_map<State<Environment>,Node> m_memory;
};

template<class Environment, class... Args>
std::unique_ptr<MctsBase<Environment>> create_algorithm(Args&&... args) {
  return std::make_unique<Mcts<Environment,Args...>>(std::forward<Args>(args)...);
}

}
