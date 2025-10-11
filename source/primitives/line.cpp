#include <iostream>
#include <utility>

#include "primitives/common.hpp"
#include "primitives/line.hpp"
#include "primitives/point.hpp"
#include "primitives/vector.hpp"

void Line::print(std::ostream &out) const {
  out << "p: ";
  p_.print(out);
  out << ", d: ";
  d_.print(out);
  out << std::endl;
}

Line Line::badLine() { return Line(Point::badPoint(), Vector::badVector()); }

bool Line::isBad() const { return p_.isBad() || d_.isBad(); }

bool Line::isDegenerate() const {
  if (this->isBad())
    return false;
  return d_.isZero();
}

bool equal(const Line &l1, const Line &l2) {
  if (l1.isBad() || l2.isBad())
    return false;
  if (!cross_product(l1.getD(), l2.getD()).isZero())
    return false;
  if (!cross_product(vec_from_points(l1.getP(), l2.getP()), l1.getD()).isZero())
    return false;
  return true;
}

std::pair<LineToLineOrientation, Point> intersection_2lines(const Line &l1, const Line &l2) {
  if (l1.isBad() || l2.isBad())
    return std::make_pair(LineToLineOrientation::Invalid, Point::badPoint());

  Vector d1 = l1.getD(), d2 = l2.getD();
  Point p1 = l1.getP(), p2 = l2.getP();

  Vector n = cross_product(d1, d2);

  Vector diff_points = vec_from_points(p1, p2); // p2 - p1 vector

  // Case Parallel
  if (compare_doubles(norm(n), 0)) {
    // Case Coincidence
    if (compare_doubles(norm(cross_product(diff_points, d1)), 0))
      return std::make_pair(LineToLineOrientation::Coincident, Point::badPoint());
    return std::make_pair(LineToLineOrientation::Parallel, Point::badPoint());
  }

  // Case Skew
  if (!compare_doubles(diff_points * n, 0))
    return std::make_pair(LineToLineOrientation::Skew, Point::badPoint());

  // Case Intersection
  double t = (cross_product(diff_points, d2) * n) / (n * n);
  Point inter = (p1.toVector() + t * d1).toPoint();
  return std::make_pair(LineToLineOrientation::Intersect, inter);
}