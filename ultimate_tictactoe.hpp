#pragma once

#include <array>
#include <bitset>
#include <functional>
#include <ostream>
#include <vector>

#include "tictactoe_utils.hpp"

namespace ultimate_tictactoe {

typedef std::bitset<9> ActiveMask;

class Environment {
  public:
    static constexpr int number_of_players = 2;

    struct State {
      std::array<tictactoe::Board,9> subboards;
      int active_subboard;
    };

    struct Action {
      int subboard, cell;
    };

    typedef std::array<double,number_of_players> Reward;

    Environment();

    int get_turn() const { return m_turn; }

    const State& get_state() const { return m_state; }

    std::vector<Action> get_available_actions() const;

    int get_number_of_players() const { return number_of_players; }

    int get_current_player() const { return m_current_player; }

    bool is_terminal() const;

    const Reward& get_score() const { return m_score; }

    const Reward& step(const Action& action, bool check = false);

    void reset();

  private:

    void update_score();

    Reward m_score;
    State m_state;
    std::bitset<9> m_playable_subboards;
    std::bitset<9> m_x_winned_subboards;
    std::bitset<9> m_o_winned_subboards;
    int m_turn;
    int m_current_player;
};

typedef Environment::State State;
typedef Environment::Action Action;
typedef Environment::Reward Reward;

bool operator==(const State& lhs, const State& rhs);

bool operator==(const Action& lhs, const Action& rhs);

std::ostream& operator<<(std::ostream& out, const State& state);

std::ostream& operator<<(std::ostream& out, const Action& action);

std::istream& operator>>(std::istream& in, Action& action);

} // ultimate_tictactoe

namespace std {

template<>
struct hash<ultimate_tictactoe::State> {
  typedef ultimate_tictactoe::State State;
  std::size_t operator()(const State& state) const;
};

} // end std ns
