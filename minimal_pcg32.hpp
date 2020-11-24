#ifndef _MINIMAL_PCG32_HPP_
#define _MINIMAL_PCG32_HPP_

#include <cstdint>
#include <random>

class pcg32 {
  /* 64bit state, 32bit output, period of 2^64 (~1.8e19)*/
  public:
    typedef uint32_t result_type;
    typedef uint64_t state_type;
    static constexpr result_type min() { return 0; }
    static constexpr result_type max() { return ~result_type(0ULL); }
    static constexpr double resolution() { return 0x1p-32; }
    pcg32(state_type s = 0) { seed(s); }
    void random_seed() {
      std::random_device rdev;
      state_type s = ((state_type)rdev())<<32 ^ rdev();
      seed(s);
    }
    void seed(state_type s) {
      _state = s;
      (*this)();
    }
    result_type operator()() {
      state_type old_state = _state;
      _state = _state*6364136223846793005ULL + 1442695040888963407ULL;
      result_type xorshifted = ((old_state>>18)^old_state) >> 27;
      result_type rot = old_state >> 59;
      return (xorshifted>>rot) | (xorshifted<<((-rot)&31));
    }
    int randint(int hi, int lo = 0) {
      uint64_t range = hi - lo;
      uint64_t x = (uint64_t)((*this)());
      return lo + (int)((x*range)>>32);
    }
    double random() {
      result_type x = (*this)();
      return x*resolution();
    }
  private:
    state_type _state;
};

#endif
