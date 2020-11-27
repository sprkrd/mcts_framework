#include <condition_variable>
#include <mutex>
#include <queue>
#include <thread>
#include <vector>

#include "thread_pool.hpp"

using namespace std;
using namespace multithreading;

typedef Pool::Job Job;

class Pool::PoolImpl {
  public:
    PoolImpl(unsigned number_of_workers);

    void add_job(Job job);

    void shutdown();

    void wait();

    bool is_active() const;

    unsigned pending_jobs() const;

    unsigned number_of_workers() const;

    ~PoolImpl();

  private:

    void work();

    mutable mutex m_mtx;
    condition_variable m_worker_proceed, m_wait_empty;
    queue<Job> m_job_queue;
    vector<thread> m_workers;
    unsigned m_pending_jobs;
    bool m_active;
};

Pool::PoolImpl::PoolImpl(unsigned number_of_workers) : m_workers(number_of_workers),
                                                       m_pending_jobs(0),
                                                       m_active(true)
{
  for (auto& worker : m_workers)
    worker = thread(&PoolImpl::work, this);
}

void Pool::PoolImpl::add_job(Job job) {
  {
    unique_lock lock(m_mtx);
    m_job_queue.push(move(job));
    ++m_pending_jobs;
  }
  m_worker_proceed.notify_one();
}

void Pool::PoolImpl::shutdown() {
  bool active;
  {
    unique_lock lock(m_mtx);
    active = m_active;
    m_active = false;
  }
  if (active) {
    m_worker_proceed.notify_all();
    for (auto& worker : m_workers)
      worker.join();
  }
}

void Pool::PoolImpl::wait() {
  unique_lock lock(m_mtx);
  m_wait_empty.wait(lock, [this]{ return m_pending_jobs == 0; });
}

bool Pool::PoolImpl::is_active() const {
  return m_active;
}

unsigned Pool::PoolImpl::pending_jobs() const {
  unique_lock lock(m_mtx);
  return m_pending_jobs;
}

unsigned Pool::PoolImpl::number_of_workers() const {
  return m_workers.size();
}

Pool::PoolImpl::~PoolImpl() {
  shutdown();
}


void Pool::PoolImpl::work() {
  while (true) {
    Job job;
    {
      unique_lock lock(m_mtx);
      m_worker_proceed.wait(lock, [this]{return !m_job_queue.empty() || !m_active; });
      if (!is_active())
        break;
      job = move(m_job_queue.front());
      m_job_queue.pop();
    }
    job();
    unsigned pending_jobs;
    {
      unique_lock lock(m_mtx);
      pending_jobs = --m_pending_jobs;
    }
    if (!pending_jobs)
      m_wait_empty.notify_all();
  }
}

Pool::Pool() : Pool(thread::hardware_concurrency()) {}

Pool::Pool(unsigned number_of_workers) : m_impl(new PoolImpl(number_of_workers)) {}

void Pool::add_job(Job job) {
  m_impl->add_job(move(job));
}

void Pool::shutdown() {
  m_impl->shutdown();
}

void Pool::wait() {
  m_impl->wait();
}

bool Pool::is_active() const {
  return m_impl->is_active();
}

unsigned Pool::pending_jobs() const {
  return m_impl->pending_jobs();
}

unsigned Pool::number_of_workers() const {
  return m_impl->number_of_workers();
}

Pool::~Pool() = default;

