#include <cmath>

#include "primitives/common.hpp"

bool approx_equal(double a, double b, double rel, double abs) {
  if (std::isnan(a) || std::isnan(b))
    return false;
  return std::fabs(a - b) <= std::max(rel * std::max(std::fabs(a), std::fabs(b)), abs);
}

bool approx_zero(double x, double scale, double rel, double abs) { return std::fabs(x) <= std::max(rel * std::fabs(scale), abs); }

// Inequalities that tolerate tiny noise
bool le_approx(double a, double b, double rel, double abs) { return (a < b) || approx_equal(a, b, rel, abs); }
bool ge_approx(double a, double b, double rel, double abs) { return (a > b) || approx_equal(a, b, rel, abs); }