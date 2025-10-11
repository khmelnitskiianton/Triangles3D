#ifndef _INCLUDE_PRIMITIVES_COMMON_HPP_
#define _INCLUDE_PRIMITIVES_COMMON_HPP_

#include <cmath>

constexpr double EPSILON = std::numeric_limits<double>::epsilon();

bool compare_doubles(const double a, const double b);

#endif