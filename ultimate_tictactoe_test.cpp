#include <iostream>

#include "array_operations.hpp"
#include "ultimate_tictactoe.hpp"
using namespace std;

template<class T, class Allocator>
ostream& operator<<(ostream& out, const vector<T, Allocator>& vector) {
  out << '[';
  for (size_t i = 0; i < vector.size(); ++i) {
    if (i > 0) out << ", ";
    out << vector[i];
  }
  return out << ']';
}

int main() {
  typedef ultimate_tictactoe::Environment UltimateTicTacToe;
  UltimateTicTacToe env;
  while (not env.is_terminal()) {
   auto available_actions = env.get_available_actions();
   UltimateTicTacToe::Action chosen_action;
   cout << "Turn " << env.get_turn() << endl;
   cout << "------" << endl;
   cout << env.get_state() << endl;
   cout << "Available actions: " << available_actions << endl;
   cout << "Your action (player " << env.get_current_player() << "): ";
   cin >> chosen_action;
   try {
     env.step(chosen_action, true);
   }
   catch (std::invalid_argument& e) {
     cout << e.what() << endl;
   }
   cout << endl;
  }
  cout << "Final state" << endl;
  cout << "-----------" << endl;
  cout << env.get_state() << endl;
  cout << endl;
  cout << "Final score" << endl;
  cout << "-----------" << endl;
  cout << env.get_score() << endl;
}
