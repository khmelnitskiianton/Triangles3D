#include <cmath>
#include <iostream>

#include "primitives/common.hpp"
#include "primitives/point.hpp"
#include "primitives/vector.hpp"

Vector Point::toVector() const { return Vector(x_, y_, z_); }

void Point::print(std::ostream &out) const { out << "p{" << x_ << ", " << y_ << ", " << z_ << "}"; }

Point Point::badPoint() { return Point(NAN, NAN, NAN); }

bool Point::isBad() const { return std::isnan(x_) || std::isnan(y_) || std::isnan(z_); }

bool equal(const Point &p1, const Point &p2) {
  return approx_equal(p1.getX(), p2.getX()) && approx_equal(p1.getY(), p2.getY()) && approx_equal(p1.getZ(), p2.getZ());
}

Vector vec_from_points(const Point &a, const Point b) {
  double x = b.getX() - a.getX();
  double y = b.getY() - a.getY();
  double z = b.getZ() - a.getZ();
  return Vector(x, y, z);
}

// Bool
bool operator==(const Point &a, const Point &b) {
  return (approx_equal(a.getX(), b.getX())) && (approx_equal(a.getY(), b.getY())) && (approx_equal(a.getZ(), b.getZ()));
}

bool operator!=(const Point &a, const Point &b) {
  return (!approx_equal(a.getX(), b.getX())) || (!approx_equal(a.getY(), b.getY())) || (!approx_equal(a.getZ(), b.getZ()));
}
