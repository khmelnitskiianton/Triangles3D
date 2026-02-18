#ifndef _INCLUDE_COMMON_HPP_
#define _INCLUDE_COMMON_HPP_

#include <cmath>

constexpr double kRelTol = 1e-6; // relative
constexpr double kAbsTol = 1e-9; // absolute

// |a - b| <= max(rel * max(|a|,|b|), abs)
template <typename T> inline bool approx_equal(T a, T b, T rel = kRelTol, T abs = kAbsTol) {
  if (std::isnan(a) || std::isnan(b))
    return false;
  return std::fabs(a - b) <= std::max(rel * std::max(std::fabs(a), std::fabs(b)), abs);
}

template <typename T> inline bool approx_zero(T x, T scale = 1.0, T rel = kRelTol, T abs = kAbsTol) {
  return std::fabs(x) <= std::max(rel * std::fabs(scale), abs);
}

// Inequalities that tolerate tiny noise
template <typename T> inline bool le_approx(T a, T b, T rel = kRelTol, T abs = kAbsTol) {
  return (a < b) || approx_equal(a, b, rel, abs);
}
template <typename T> inline bool ge_approx(T a, T b, T rel = kRelTol, T abs = kAbsTol) {
  return (a > b) || approx_equal(a, b, rel, abs);
}
template <typename T> inline bool l_approx(T a, T b, T rel = kRelTol, T abs = kAbsTol) {
  return (a < b) && !approx_equal(a, b, rel, abs);
}
template <typename T> inline bool g_approx(T a, T b, T rel = kRelTol, T abs = kAbsTol) {
  return (a > b) && !approx_equal(a, b, rel, abs);
}

#endif