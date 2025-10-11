#include <cmath>

#include "primitives/common.hpp"

bool compare_doubles(const double a, const double b) {
  if (std::isnan(a) || std::isnan(b))
    return false;
  return std::fabs(a - b) < EPSILON;
}