#include "utils.hpp"

#include "bidding_game.hpp"


namespace bidding_game{

Environment::Environment() {
  reset();
}

std::vector<Action> Environment::get_available_actions() const {
  std::vector<Action> available_actions;
  available_actions.reserve(m_state.budget[m_state.current_player]);
  for (int i = 1; i <= m_state.budget[m_state.current_player]; ++i)
    available_actions.push_back(i);
  return available_actions;
}

bool Environment::is_terminal() const {
  return !m_state.budget[0] ||
         !m_state.budget[1] ||
         m_state.scotch == 10 ||
         m_state.scotch == 0;
}

const Reward& Environment::step(Action action, bool check) {
  if (check)
    mcts::check_action(*this, action);
  if (m_state.current_player == 0)
    m_action_0 = action; // hold the bet until next player's turn
  else {
    if (m_action_0 < action) {
      m_state.scotch++;
      m_state.budget[1] -= action;
    }
    else if (action < m_action_0) {
      m_state.scotch--;
      m_state.budget[0] -= m_action_0;
    }
    else {
      m_state.scotch += m_state.draw_advantage - !m_state.draw_advantage;
      m_state.budget[m_state.draw_advantage] -= action;
      m_state.draw_advantage = !m_state.draw_advantage;
    }
    m_turn += 1;
    update_score();
  }
  m_state.current_player = !m_state.current_player;
  return m_score;
}

void Environment::reset() {
  m_score.fill(0);
  m_state.budget.fill(100);
  m_state.scotch = 5;
  m_state.draw_advantage = 0;
  m_state.current_player = 0;
  m_turn = 0;
}

void Environment::update_score() {
  if (is_terminal()) {
    bool player_0_wins = m_state.scotch == 0 ||
      (!m_state.budget[1] && m_state.scotch != 10 && m_state.budget[0] >= m_state.scotch);
    bool player_1_wins = m_state.scotch == 10 ||
      (!m_state.budget[0] && m_state.scotch != 0 && m_state.budget[1] >= 10-m_state.scotch);
    bool tie = !player_0_wins && !player_1_wins;
    m_score = {0.5*tie + player_0_wins, 0.5*tie + player_1_wins};
  }
}

bool operator==(const State& lhs, const State& rhs) {
  return lhs.budget == rhs.budget &&
         lhs.scotch == rhs.scotch &&
         lhs.draw_advantage == rhs.draw_advantage &&
         lhs.current_player == rhs.current_player;
}

std::ostream& operator<<(std::ostream& out, const State& state) {
  out << "player 0";
  if (!state.draw_advantage)
    out << '*';
  else
    out << ' ';
  out << '(' << state.budget[0] << ") ";
  for (int i = 0; i <= 10; ++i) {
    if (i == state.scotch)
      out << 's';
    else
      out << '-';
  }
  out << " player 1";
  if (state.draw_advantage)
    out << '*';
  else
    out << ' ';
  return out << '(' << state.budget[1] << ')';
}

} // bidding_game

size_t std::hash<bidding_game::State>::operator()(const State& state) const {
  size_t seed = 0;
  mcts::hash_combine(seed, state.budget[0]);
  mcts::hash_combine(seed, state.budget[1]);
  mcts::hash_combine(seed, state.scotch);
  mcts::hash_combine(seed, state.draw_advantage);
  mcts::hash_combine(seed, state.current_player);
  return seed;
}

