#include <iostream>
#include <utility>

#include "common.hpp"
#include "primitives/line.hpp"
#include "primitives/point.hpp"
#include "primitives/vector.hpp"

void Line::print(std::ostream &out) const noexcept {
  out << "p: ";
  p_.print(out);
  out << ", d: ";
  d_.print(out);
  out << std::endl;
}

std::pair<LineToLineOrientation, Point> intersection_2lines(const Line &l1, const Line &l2) {
  if (l1.isBad() || l2.isBad())
    return std::make_pair(LineToLineOrientation::Invalid, Point::badPoint());

  Vector d1 = l1.getD(), d2 = l2.getD();
  Point p1 = l1.getP(), p2 = l2.getP();

  Vector n = cross_product(d1, d2);

  Vector diff_points = vec_from_points(p1, p2); // p2 - p1 vector

  // Case Parallel
  if (approx_zero(norm(n), norm(d1) * norm(d2))) {
    // Case Coincidence
    if (approx_zero(norm(cross_product(diff_points, d1)), norm(diff_points) * norm(d1)))
      return std::make_pair(LineToLineOrientation::Coincident, Point::badPoint());
    return std::make_pair(LineToLineOrientation::Parallel, Point::badPoint());
  }
  double cop = diff_points * n;
  double cop_scale = std::max(1.0, norm(diff_points) * norm(n));
  // Case Skew
  if (!approx_zero(cop, cop_scale))
    return std::make_pair(LineToLineOrientation::Skew, Point::badPoint());

  // Case Intersection
  double t = (cross_product(diff_points, d2) * n) / (n * n);
  Point inter = (p1.toVector() + t * d1).toPoint();
  return std::make_pair(LineToLineOrientation::Intersect, inter);
}