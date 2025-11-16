#include <cmath>
#include <iostream>

#include "common.hpp"
#include "primitives/point.hpp"
#include "primitives/vector.hpp"

void Point::print(std::ostream &out) const noexcept { out << "p{" << x_ << ", " << y_ << ", " << z_ << "}"; }

Vector Point::toVector() const noexcept { return Vector(x_, y_, z_); }

// Bool
bool operator==(const Point &a, const Point &b) {
  return (approx_equal(a.getX(), b.getX())) && (approx_equal(a.getY(), b.getY())) && (approx_equal(a.getZ(), b.getZ()));
}

bool operator!=(const Point &a, const Point &b) {
  return (!approx_equal(a.getX(), b.getX())) || (!approx_equal(a.getY(), b.getY())) || (!approx_equal(a.getZ(), b.getZ()));
}
