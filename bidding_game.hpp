#pragma once

#include <array>
#include <bitset>
#include <functional>
#include <ostream>
#include <vector>

namespace bidding_game {

class Environment {
  public:

    static constexpr int number_of_players = 2;

    typedef std::array<double,number_of_players> Reward;

    struct State {
      std::array<int,2> budget;
      int scotch, draw_advantage, current_player;
    };

    typedef int Action;

    Environment();

    int get_turn() const { return m_turn; }

    const State& get_state() const { return m_state; }

    std::vector<Action> get_available_actions() const;

    int get_number_of_players() const { return number_of_players; }

    int get_current_player() const { return m_state.current_player; }

    bool is_terminal() const;

    const Reward& get_score() const { return m_score; }

    const Reward& step(Action action, bool check = false);

    void reset();

  private:

    void update_score();

    Reward m_score;
    State m_state;
    int m_action_0;
    int m_turn;
};

typedef Environment::State State;
typedef Environment::Action Action;
typedef Environment::Reward Reward;

bool operator==(const State& lhs, const State& rhs);

std::ostream& operator<<(std::ostream& out, const State& state);

} // bidding_game

namespace std {

template<>
struct hash<bidding_game::State> {
  typedef bidding_game::State State;
  size_t operator()(const State& state) const;
};

} // end std ns
