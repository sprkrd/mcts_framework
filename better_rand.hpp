#include <cstdint>
#include <ostream>
#include <sstream>
#include <iomanip>

class Ran {
  public:
    typedef uint64_t result_type;
    static constexpr result_type min() { return 0; }
    static constexpr result_type max() { return ~result_type(0ULL); }
    Ran(result_type s = 0) : _v(4101842887655102017ULL), _w(1) {
      _u = s^_v; (*this)();
      _v = _u;   (*this)();
      _w = _v;   (*this)();
    }
    result_type operator()() {
      _u = _u * 2862933555777941757LL + 7046029254386353087LL;
      _v ^= _v >> 17; _v ^= _v << 31; _v ^= _v >> 8;
      _w = 4294957665U*(_w & 0xffffffff) + (_w >> 32);
      uint64_t x = _u ^ (_u << 21); x ^= x >> 35; x ^= x << 4;
      return (x + _v) ^ _w;
    }
  private:
    result_type _u, _v, _w;
};

class RanQ1 {
  public:
    typedef uint64_t result_type;
    static constexpr result_type min() { return 0; }
    static constexpr result_type max() { return ~result_type(0ULL); }
    RanQ1(result_type s = 0) : _v(4101842887655102017ULL) {
      _v ^= s;
      _v = (*this)();
    }
    result_type operator()() {
      _v ^= _v >> 21; _v ^= _v << 35; _v ^= _v >> 4;
      return _v*2685821657736338717ULL;
    }
  private:
    result_type _v;
};

class pcg32 {
  /* 64bit state, 32bit output, period of 2^64 (~1.8e19)*/
  public:
    typedef uint32_t result_type;
    typedef uint64_t state_type;
    static constexpr result_type min() { return 0; }
    static constexpr result_type max() { return ~result_type(0ULL); }
    static constexpr double resolution() { return 0x1p-32; }
    pcg32(state_type s = 0) { seed(s); }
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

class xoshiro256ss {
  /* 256bit state, 64bit output, period of 2^256-1 (~1.2e77)*/
  public:
    typedef uint64_t result_type;
    static result_type splitmix64(result_type& s) {
      result_type z = (s += 0x9e3779b97f4a7c15);
    	z = (z ^ (z >> 30)) * 0xbf58476d1ce4e5b9;
    	z = (z ^ (z >> 27)) * 0x94d049bb133111eb;
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
      for (result_type& s_i : _s)
        s_i = splitmix64(s);
    }
    result_type operator()() {
      result_type result = rotl(_s[1] * 5, 7) * 9;
    	result_type t = _s[1] << 17;
    	_s[2] ^= _s[0];
      _s[3] ^= _s[1];
      _s[1] ^= _s[2];
      _s[0] ^= _s[3];
    	_s[2] ^= t;
    	_s[3] = rotl(_s[3], 45);
    	return result;
    }
  private:
    result_type _s[4];
};
