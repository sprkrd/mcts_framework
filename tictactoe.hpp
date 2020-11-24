#pragma once

#include <array>
#include <bitset>
#include <functional>
#include <ostream>
#include <vector>

namespace tictactoe {

typedef std::bitset<18> Board;

class Environment {
  public:

    static constexpr int number_of_players = 2;

    typedef std::array<double,number_of_players> Reward;

    struct State {
      Board board;
    };

    struct Action {
      int cell;
    };

    Environment();

    int get_turn() const;

    const State& get_state() const { return m_state; }

    std::vector<Action> get_available_actions() const;

    int get_number_of_players() const { return number_of_players; }

    int get_current_player() const;

    bool is_terminal() const;

    Reward get_score() const;

    Reward step(const Action& action, bool check = false);

    void reset();

  private:

    State m_state;
};

typedef Environment::State State;
typedef Environment::Action Action;
typedef Environment::Reward Reward;

bool operator==(const State& lhs, const State& rhs);

bool operator==(const Action& lhs, const Action& rhs);

std::ostream& operator<<(std::ostream& out, const State& state);

std::ostream& operator<<(std::ostream& out, const Action& action);

std::istream& operator>>(std::istream& in, Action& action);

} // tictactoe

namespace std {

template<>
struct hash<tictactoe::State> {
  typedef tictactoe::State State;
  size_t operator()(const State& state) const;
};

} // end std ns
