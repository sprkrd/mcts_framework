#pragma once

#include <functional>
#include <list>
#include <memory>
#include <stack>
#include <type_traits>
#include <utility>

namespace mcts::memory {

template<class T>
class PoolAllocator {
  public:
    typedef T value_type;

    PoolAllocator() = default;

    template<class U>
    PoolAllocator(const PoolAllocator<U>&) {}

    T* allocate(std::size_t n) {
      T* ptr;
      if (n == 1 && !m_pool.empty()) {
        ptr = m_pool.top();
        m_pool.pop();
      }
      else
        ptr = static_cast<T*>(::operator new(sizeof(T)*n));
      return ptr;
    }

    void deallocate(T* ptr, std::size_t n) {
      if (n == 1)
        m_pool.push(ptr);
      else
        ::operator delete(ptr);
    }

    template<class U>
    bool operator==(const PoolAllocator<U>&) const {
      return sizeof(T) == sizeof(U);
    }

    template<class U>
    bool operator!=(const PoolAllocator<U>& other) const {
      return !(*this == other);
    }

    ~PoolAllocator() {
      while (!m_pool.empty()) {
        ::operator delete(m_pool.top());
        m_pool.pop();
      }
    }

  private:
    std::stack<T*> m_pool;
};

template< class Key,
          class T,
          class Hash = std::hash<Key>,
          class KeyEqual = std::equal_to<Key>,
          class Allocator = std::allocator<std::pair<const Key,T>> >
class LruMap {
  public:
    typedef Key key_type;
    typedef T mapped_type;
    typedef std::pair<const Key,T> value_type;
    typedef Hash hasher;
    typedef KeyEqual key_equal;
    typedef Allocator allocator_type;

  private:
    typedef std::list<value_type, Allocator> List;

  public:
    typedef typename List::iterator iterator;
    typedef typename List::const_iterator const_iterator;

  private:
    typedef std::reference_wrapper<const Key> KeyRef;
    typedef std::pair<const KeyRef,iterator> KeyRefTPair;
    typedef std::allocator_traits<Allocator> AllocatorTraits;
    typedef typename AllocatorTraits::template rebind_alloc<KeyRefTPair> MapAllocator;
    typedef std::unordered_map<KeyRef,iterator,Hash,KeyEqual,MapAllocator> HashMap;

  public:
    LruMap(
      std::size_t capacity = 1000000,
      Hash hash = Hash(),
      KeyEqual key_equal = KeyEqual(),
      Allocator alloc = Allocator()
    ) :
      m_capacity(capacity),
      m_list(alloc),
      m_map(1.4*capacity, hash, key_equal, MapAllocator(alloc)) {
    }

    iterator begin() {
      return m_list.begin();
    }

    iterator end() {
      return m_list.end();
    }

    const_iterator begin() const {
      return m_list.begin();
    }

    const_iterator end() const {
      return m_list.end();
    }

    iterator find(const Key& key) {
      auto it = m_map.find(key);
      if (it == m_map.end())
        return m_list.end();
      touch(it->second);
      return it->second;
    }

    T& operator[](const Key& key) {
      auto it = find(key);
      if (it != m_list.end())
        return it->second;
      if (m_list.size() == m_capacity)
        pop_least_recent();
      add_element(key);
      return m_list.front().second;
    }

    std::size_t size() const {
      return m_list.size();
    }

    std::size_t capacity() const {
      return m_capacity;
    }

    void clear() {
      m_list.clear();
      m_map.clear();
    }

  private:
    void touch(iterator it) {
      m_list.splice(m_list.begin(), m_list, it);
    }

    void pop_least_recent() {
      m_map.erase(m_list.back().first);
      m_list.pop_back();
    }

    void add_element(const Key& key) {
      m_list.emplace_front(key, T());
      m_map.emplace(m_list.front().first, m_list.begin());
    }

    std::size_t m_capacity;
    List m_list;
    HashMap m_map;
};

} // mcts::memory
