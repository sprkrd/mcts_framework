#pragma once
#include <array>
#include <ostream>

template<typename T, std::size_t n>
std::array<T,n> operator+(const std::array<T,n>& l, const std::array<T,n>& r) {
  std::array<T,n> result;
  for (unsigned i = 0; i < n; ++i)
    result[i] = l[i] + r[i];
  return result;
}

template<typename T, std::size_t n>
std::array<T,n> operator-(const std::array<T,n>& l, const std::array<T,n>& r) {
  std::array<T,n> result;
  for (unsigned i = 0; i < n; ++i)
    result[i] = l[i] - r[i];
  return result;
}

template<typename T, std::size_t n>
std::array<T,n>& operator+=(std::array<T,n>& l, const std::array<T,n>& r) {
  for (unsigned i = 0; i < n; ++i)
        l[i] += r[i];
  return l;
}

template<typename T, std::size_t n>
std::array<T,n> operator*(T s, const std::array<T,n>& r) {
  std::array<T,n> result;
  for (unsigned i = 0; i < n; ++i)
    result[i] = s*r[i];
  return result;
}

template<typename T, std::size_t n>
std::ostream& operator<<(std::ostream& out, const std::array<T,n>& a) {
  out << '[';
  for (unsigned i = 0; i < a.size(); ++i) {
    if (i > 0) out << ',';
    out << a[i];
  }
  return out << ']';
}
