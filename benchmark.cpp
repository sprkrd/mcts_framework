#include <iostream>
#include <memory>
#include <string>

#include "better_rand.hpp"
#include "mcts.hpp"
#include "thread_pool.hpp"
#include "tictactoe.hpp"
#include "ultimate_tictactoe.hpp"
using namespace std;
using namespace mcts;

using ultimate_tictactoe::Environment;

typedef decltype(create_algorithm<Environment>()) AlgorithmPtr;
typedef function<AlgorithmPtr()> AlgorithmCreator;
typedef vector<pair<string,AlgorithmCreator>> AlgorithmVector;

struct ResultSummary {
  int wins, ties, loses;
  ResultSummary& operator+=(const ResultSummary& other) {
    wins += other.wins;
    ties += other.ties;
    loses += other.loses;
    return *this;
  }
  ResultSummary operator+(const ResultSummary& other) const {
    ResultSummary result = *this;
    result += other;
    return result;
  }
  ResultSummary reverse() const {
    ResultSummary reversed;
    reversed.wins = loses;
    reversed.ties = ties;
    reversed.loses = wins;
    return reversed;
  }
};

typedef vector<vector<ResultSummary>> ResultMatrix;

ostream& operator<<(ostream& out, const ResultSummary& result) {
  return out << result.wins << '/' << result.ties << '/' << result.loses;
}

//AlgorithmPtr standard_mcts(double c) {
  //auto rng = make_shared<pcg32>();
  //rng->random_seed();
  //return create_algorithm<Environment>(
      //UctSelect(c), RandomPolicy(rng), StandardBackup<Environment,SampleAverage>());
//}

//AlgorithmPtr sarsa_mcts(double c) {
  //auto rng = make_shared<pcg32>();
  //rng->random_seed();
  //return create_algorithm<Environment>(
      //UctSelect(c), RandomPolicy(rng), SarsaBackup<Environment,ExponentialAverage>(1.0,0.2));
//}

//AlgorithmPtr qlearn_mcts(double c) {
  //auto rng = make_shared<pcg32>();
  //rng->random_seed();
  //return create_algorithm<Environment>(
      //UctSelect(c), RandomPolicy(rng), QlearnBackup<Environment,SampleAverage>());
//}

ResultSummary benchmark_job(AlgorithmCreator creator1, AlgorithmCreator creator2, unsigned number_of_plays) {
  auto result = ResultSummary();
  while (number_of_plays) {
    Environment env;
    auto alg1 = creator1();
    auto alg2 = creator2();
    while (!env.is_terminal()) {
      env.step(alg1->search(env, nullptr, 0.1));
      if (!env.is_terminal())
        env.step(alg2->search(env, nullptr, 0.1));
    }
    auto score = env.get_score();
    ResultSummary game_result = { score[0] > score[1], score[0] == score[1], score[0] < score[1] };
    result += game_result;
    --number_of_plays;
  }
  return result;
}

//ResultMatrix benchmark(const AlgorithmVector& algorithms) {
  //multithreading::Pool thread_pool;
  //ResultMatrix results(algorithms.size(), vector<ResultSummary>(algorithms.size()));
  //unsigned progress = 0;
  //unsigned total = algorithms.size()*algorithms.size();
  //cerr << "Progress: 0/" << total << '\n';
  //for (unsigned i = 0; i < algorithms.size(); ++i) {
    //for (unsigned j = 0; j < algorithms.size(); ++j) {
      //vector<future<ResultSummary>> partial_results_i_j(thread_pool.number_of_workers());
      //for (auto& fut : partial_results_i_j)
        //fut = thread_pool.async(
            //benchmark_job, algorithms[i].second, algorithms[j].second, 4);
      //for (auto& fut : partial_results_i_j)
        //results[i][j] += fut.get();
      //cerr << "Progress: " << (++progress) << '/' << total << '\n';
    //}
  //}
  //return results;
//}

AlgorithmPtr create_alg_1() {
  auto rng = make_shared<pcg32>();
  rng->random_seed();
  return create_algorithm<Environment>(UctSelect(0.5), RandomPolicy(rng), StandardBackup<Environment,SampleAverage>());
}

AlgorithmPtr create_alg_2() {
  auto rng = make_shared<pcg32>();
  rng->random_seed();
  return create_algorithm<Environment>(UctSelect(0.5), RandomPolicy(rng), SarsaBackup<Environment,SampleAverage>());
}

int main() {

  multithreading::Pool pool;

  auto result_summary_1_2 = ResultSummary();
  auto result_summary_2_1 = ResultSummary();

  vector<future<ResultSummary>> partial_results_1_2(pool.number_of_workers());
  vector<future<ResultSummary>> partial_results_2_1(pool.number_of_workers());

  for (auto& fut : partial_results_1_2)
    fut = pool.async(benchmark_job, create_alg_1, create_alg_2, 8);
  
  cerr << "Progress 1vs2: 0/" << partial_results_1_2.size() << '\n';
  for (unsigned i = 0; i < partial_results_1_2.size(); ++i) {
    result_summary_1_2 += partial_results_1_2[i].get();
    cerr << "Progress 1vs2: " << (i+1) << '/' << partial_results_1_2.size() << '\n';
  }

  for (auto& fut : partial_results_2_1)
    fut = pool.async(benchmark_job, create_alg_2, create_alg_1, 8);

  cerr << "Progress 2vs1: 0/" << partial_results_2_1.size() << '\n';
  for (unsigned i = 0; i < partial_results_1_2.size(); ++i) {
    result_summary_2_1 += partial_results_2_1[i].get();
    cerr << "Progress 2vs1: " << i << '/' << partial_results_2_1.size() << '\n';
  }

  cout << "Result Alg. 1 vs Alg. 2: " << result_summary_1_2 << endl;
  cout << "Result Alg. 2 vs Alg. 1: " << result_summary_2_1 << endl;

  auto result_overall = result_summary_1_2 + result_summary_2_1.reverse();
  cout << "Result overall: " << result_overall << endl;


  //AlgorithmVector algorithms;
  //algorithms.emplace_back("mcts-standard(1.0)", bind(standard_mcts, 1.0));
  //algorithms.emplace_back("mcts-sarsa(1.0)",  bind(sarsa_mcts, 1.0));
  //algorithms.emplace_back("mcts-qlearn(0.5)", bind(qlearn_mcts, 0.5));
  //algorithms.emplace_back("mcts-standard(2.0)", bind(standard_mcts, 2.0));
  //algorithms.emplace_back("mcts-standard(2.5)", bind(standard_mcts, 2.5));

  //auto result_matrix = benchmark(algorithms);

  //for (unsigned i = 0; i < result_matrix.size(); ++i) {
    //for (unsigned j = 0; j < result_matrix[i].size(); ++j) {
      //cout << result_matrix[i][j] << " ";
    //}
    //cout << endl;
  //}

  //multithreading::Pool thread_pool;
  //vector<int> v(100);
  //vector<thread::id> responsible(v.size());
  //for (unsigned i = 0; i < v.size(); ++i)
    //v[i] = i;
  //for (unsigned i = 0; i < v.size(); ++i)
    //thread_pool.add_job([&,i]{ v[i] *= 2; responsible[i] = this_thread::get_id(); });
  //thread_pool.wait();
  //for (unsigned i = 0; i < v.size(); ++i) {
    //cout << v[i] << ' ' << responsible[i] << endl;
  //}
}



