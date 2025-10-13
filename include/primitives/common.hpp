#ifndef _INCLUDE_PRIMITIVES_COMMON_HPP_
#define _INCLUDE_PRIMITIVES_COMMON_HPP_

#include <cmath>

// Tunables for geometry (sane defaults; tweak if your coordinates are huge)
constexpr double kRelTol = 1e-6;   // relative tolerance
constexpr double kAbsTol = 1e-9;  // absolute floor

// |a - b| <= max(rel * max(|a|,|b|), abs)
bool approx_equal(double a, double b, double rel = kRelTol, double abs = kAbsTol);
bool approx_zero(double x, double scale = 1.0, double rel = kRelTol, double abs = kAbsTol);
bool le_approx(double a, double b, double rel = kRelTol, double abs = kAbsTol);
bool ge_approx(double a, double b, double rel = kRelTol, double abs = kAbsTol);

#endif