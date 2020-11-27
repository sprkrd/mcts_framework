#pragma once

#include <functional>
#include <future>
#include <memory>
#include <type_traits>
#include <utility>

namespace multithreading {

class Pool {
  public:
    typedef std::function<void()> Job;

    Pool();

    Pool(unsigned number_of_workers);

    void add_job(Job job);

    template<class F, class... Args>
    auto async(F&& fun, Args&&... args) {
      typedef std::invoke_result_t<F,Args...> ResultType;
      typedef std::packaged_task<ResultType()> PackagedTask;
      auto job = std::make_shared<PackagedTask>(
          std::bind(std::forward<F>(fun), std::forward<Args>(args)...));
      add_job([job] { (*job)(); } );
      return job->get_future();
    }

    void shutdown();

    void wait();

    bool is_active() const;

    unsigned pending_jobs() const;

    unsigned number_of_workers() const;

    ~Pool();

  private:

    class PoolImpl;
    std::unique_ptr<PoolImpl> m_impl;
};

} // multithreading

