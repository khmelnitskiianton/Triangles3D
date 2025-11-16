#ifndef _INCLUDE_COMMON_HPP_
#define _INCLUDE_COMMON_HPP_

#include <cmath>

// Tunables for geometry
constexpr double kRelTol = 1e-6; // relative tolerance
constexpr double kAbsTol = 1e-9; // absolute floor

// |a - b| <= max(rel * max(|a|,|b|), abs)

inline bool approx_equal(double a, double b, double rel = kRelTol, double abs = kAbsTol) {
  if (std::isnan(a) || std::isnan(b))
    return false;
  return std::fabs(a - b) <= std::max(rel * std::max(std::fabs(a), std::fabs(b)), abs);
}
inline bool approx_zero(double x, double scale = 1.0, double rel = kRelTol, double abs = kAbsTol) {
  return std::fabs(x) <= std::max(rel * std::fabs(scale), abs);
}

// Inequalities that tolerate tiny noise
inline bool le_approx(double a, double b, double rel = kRelTol, double abs = kAbsTol) {
  return (a < b) || approx_equal(a, b, rel, abs);
}
inline bool ge_approx(double a, double b, double rel = kRelTol, double abs = kAbsTol) {
  return (a > b) || approx_equal(a, b, rel, abs);
}

inline bool l_approx(double a, double b, double rel = kRelTol, double abs = kAbsTol) {
  return (a < b) && !approx_equal(a, b, rel, abs);
}
inline bool g_approx(double a, double b, double rel = kRelTol, double abs = kAbsTol) {
  return (a > b) && !approx_equal(a, b, rel, abs);
}

#endif