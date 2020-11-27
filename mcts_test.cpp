#include <iostream>
#include <thread>

#include "mcts.hpp"
#include "minimal_pcg32.hpp"
#include "tictactoe.hpp"
#include "ultimate_tictactoe.hpp"
#include "bidding_game.hpp"
#include "utils.hpp"

using namespace std;
using namespace mcts;

int main() {
  //typedef tictactoe::Environment Environment;
  //typedef ultimate_tictactoe::Environment Environment;
  typedef bidding_game::Environment Environment;

  pcg32 rng(42);
 
  auto algorithm = create_algorithm<Environment>(
    //EpsilonGreedySelect(&rng,0.1,0),
    UctSelect(1.0),
    RandomPolicy(&rng),
    QlearnBackup<Environment,SampleAverage>());
 
  Environment env;
  while (not env.is_terminal()) {
    cout << "Turn " << env.get_turn() << endl;
    cout << "-------------------" << endl;
    cout << env.get_state() << endl << endl;
    auto action = algorithm->search(env, &cout, 0.2);
    cout << endl;
    cout << "Next action\n"
         << "-----------\n"
         << action << endl << endl;
    env.step(action);
  }
  cout << "Turn " << env.get_turn() << endl;
  cout << "-------------------" << endl;
  cout << env.get_state() << endl;
  cout << "Score: " << env.get_score() << endl;
}
