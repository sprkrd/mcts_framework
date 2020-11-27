#include <cstdint>
#include <random>

inline uint64_t produce_random_seed() {
  std::random_device rdev;
  return ((uint64_t)rdev())<<32 | rdev();
}

class Ran {
  public:
    typedef uint64_t result_type;

    static constexpr result_type min() { return 0; }
    static constexpr result_type max() { return ~result_type(0ULL); }

    Ran(result_type s = 0) {
      seed(s);
    }

    void seed(result_type s) {
      m_v = 4101842887655102017ULL;
      m_w = 1;
      m_u = s^m_v; (*this)();
      m_v = m_u; (*this)();
      m_w = m_v; (*this)();
    }

    void random_seed() {
      seed(produce_random_seed());
    }

    result_type operator()() {
      m_u = m_u * 2862933555777941757LL + 7046029254386353087LL;
      m_v ^= m_v >> 17; m_v ^= m_v << 31; m_v ^= m_v >> 8;
      m_w = 4294957665U*(m_w & 0xffffffff) + (m_w >> 32);
      uint64_t x = m_u ^ (m_u << 21); x ^= x >> 35; x ^= x << 4;
      return (x + m_v) ^ m_w;
    }

  private:
    result_type m_u, m_v, m_w;
};

class RanQ1 {
  public:
    typedef uint64_t result_type;

    static constexpr result_type min() { return 0; }
    static constexpr result_type max() { return ~result_type(0ULL); }

    RanQ1(result_type s = 0) {
      seed(s);
    }

    void seed(result_type s) {
      m_v = 4101842887655102017ULL;
      m_v ^= s;
      m_v = (*this)();
    }

    void random_seed() {
      seed(produce_random_seed());
    }

    result_type operator()() {
      m_v ^= m_v >> 21; m_v ^= m_v << 35; m_v ^= m_v >> 4;
      return m_v*2685821657736338717ULL;
    }

  private:
    result_type m_v;
};

class pcg32 {
  /* 64bit state, 32bit output, period of 2^64 (~1.8e19)*/
  public:
    typedef uint32_t result_type;
    typedef uint64_t state_type;

    static constexpr state_type mul = 6364136223846793005ULL;
    static constexpr state_type inc = 1442695040888963407ULL;

    static constexpr result_type min() { return 0; }
    static constexpr result_type max() { return ~result_type(0ULL); }

    pcg32(state_type s = 0) { seed(s); }

    void seed(state_type s) {
      m_state = s;
      (*this)();
    }

    void random_seed() {
      seed(produce_random_seed());
    }

    result_type operator()() {
      state_type old_state = m_state;
      m_state = m_state*mul + inc;
      result_type xorshifted = ((old_state>>18)^old_state) >> 27;
      result_type rot = old_state >> 59;
      return (xorshifted>>rot) | (xorshifted<<((-rot)&31));
    }
  private:
    state_type m_state;
};

class xoshiro256ss {
  /* 256bit state, 64bit output, period of 2^256-1 (~1.2e77)*/
  public:
    typedef uint64_t result_type;

    static result_type splitmix64(result_type& s) {
      result_type z = (s += 0x9e3779b97f4a7c15ULL);
    	z = (z ^ (z >> 30)) * 0xbf58476d1ce4e5b9ULL;
    	z = (z ^ (z >> 27)) * 0x94d049bb133111ebULL;
    	return z ^ (z >> 31);
    }

    static result_type rotl(result_type x, unsigned k) {
      return (x << k) | (x >> (64 - k));
    }

    static constexpr result_type min() { return 0; }
    static constexpr result_type max() { return ~result_type(0ULL); }

    xoshiro256ss(result_type s = 0) {
      seed(s);
    }

    void seed(result_type s = 0) {
      for (result_type& s_i : m_s)
        s_i = splitmix64(s);
    }

    void random_seed() {
      seed(produce_random_seed());
    }

    result_type operator()() {
      result_type result = rotl(m_s[1] * 5, 7) * 9;
    	result_type t = m_s[1] << 17;
    	m_s[2] ^= m_s[0];
      m_s[3] ^= m_s[1];
      m_s[1] ^= m_s[2];
      m_s[0] ^= m_s[3];
    	m_s[2] ^= t;
    	m_s[3] = rotl(m_s[3], 45);
    	return result;
    }

  private:
    result_type m_s[4];
};
