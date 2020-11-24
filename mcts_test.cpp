#include <iostream>

#include "mcts.hpp"
#include "minimal_pcg32.hpp"
#include "tictactoe.hpp"
#include "ultimate_tictactoe.hpp"
#include "utils.hpp"

using namespace std;
using namespace mcts;

int main() {
  typedef tictactoe::Environment TicTacToe;
  typedef ultimate_tictactoe::Environment UltimateTicTacToe;

  auto state = UltimateTicTacToe::State();
  cout << state << endl;

  // pcg32 rng(42);
  //
  // auto algorithm = create_algorithm<TicTacToe>(
  //   EpsilonGreedySelect(&rng,0.1,0),
  //   RandomPolicy(&rng),
  //   QlearnBackup<TicTacToe,ExponentialAverage>(1.0,0.1));
  //
  // TicTacToe env;
  // while (not env.is_terminal()) {
  //   cout << "Turn " << env.get_turn() << endl;
  //   cout << "-------------------" << endl;
  //   cout << env.get_state() << endl << endl;
  //   auto action = algorithm->search(env, &cout, 0.1);
  //   cout << endl;
  //   cout << "Next action\n"
  //        << "-----------\n"
  //        << action << endl << endl;
  //   env.step(action);
  // }
  // cout << "Turn " << env.get_turn() << endl;
  // cout << "-------------------" << endl;
  // cout << env.get_state() << endl;
}
