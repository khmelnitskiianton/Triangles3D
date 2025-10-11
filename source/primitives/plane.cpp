#include <cmath>
#include <utility>

#include "primitives/common.hpp"
#include "primitives/plane.hpp"
#include "primitives/point.hpp"
#include "primitives/vector.hpp"

Plane::Plane(const Point &p1, const Point &p2, const Point &p3) {
  double x1 = p1.getX(), y1 = p1.getY(), z1 = p1.getZ();
  double x2 = p2.getX(), y2 = p2.getY(), z2 = p2.getZ();
  double x3 = p3.getX(), y3 = p3.getY(), z3 = p3.getZ();
  /// or [B-A x C-A]
  double a = (y2 - y1) * (z3 - z1) - (z2 - z1) * (y3 - y1);
  double b = (z2 - z1) * (x3 - x1) - (x2 - x1) * (z3 - z1);
  double c = (x2 - x1) * (y3 - y1) - (y2 - y1) * (x3 - x1);
  n_ = Vector(a, b, c);
  p_ = p1;
}

PlaneSide Plane::pointPosition(const Point &x) const {
  double d = n_ * (x - p_);
  if (compare_doubles(d, 0))
    return PlaneSide::BelongsToPlane;
  else if (d > 0)
    return PlaneSide::PositiveHalfSpace;
  else
    return PlaneSide::NegativeHalfSpace;
}

void Plane::print(std::ostream &out) const {
  out << "p: ";
  p_.print(out);
  out << ", n: ";
  n_.print(out);
  out << std::endl;
}

Plane Plane::badPlane() { return Plane(Point::badPoint(), Vector::badVector()); }

bool Plane::isBad() const { return p_.isBad() || n_.isBad(); }

bool Plane::isDegenerate() const {
  if (this->isBad())
    return false;
  return n_.isZero();
}

std::pair<PlaneToPlaneOrientation, Line> intersection_2planes(const Plane &p1, const Plane &p2) {
  if (p1.isBad() || p2.isBad())
    return std::make_pair(PlaneToPlaneOrientation::Invalid, Line::badLine());

  Vector n1 = p1.getN(), n2 = p2.getN();
  double s1 = p1.getS(), s2 = p2.getS();

  Vector line_n = cross_product(n1, n2);

  // Case Parallel
  if (compare_doubles(norm(line_n), 0)) {
    double l1 = norm(n1);
    double l2 = norm(n2);
    double c = (n1 * n2) / (l1 * l2);
    double sign = (c >= 0.0) ? 1.0 : -1.0;
    double d1 = s1 / l1;
    double d2 = (sign * s2) / l2;
    // Case Coincidence
    if (compare_doubles(d1, d2))
      return std::make_pair(PlaneToPlaneOrientation::Coincident, Line::badLine());
    return std::make_pair(PlaneToPlaneOrientation::Parallel, Line::badLine());
  }

  // Case Intersection
  double n1n2 = n1 * n2;
  double n1_norm_sqr = n1 * n1;
  double n2_norm_sqr = n2 * n2;

  double a = (s2 * n1n2 - s1 * n2_norm_sqr) / (n1n2 * n1n2 - n1_norm_sqr * n2_norm_sqr);
  double b = (s1 * n1n2 - s2 * n1_norm_sqr) / (n1n2 * n1n2 - n1_norm_sqr * n2_norm_sqr);
  Vector p_pos_vector = a * n1 + b * n2;
  Point line_p = p_pos_vector.toPoint();
  return std::make_pair(PlaneToPlaneOrientation::Intersect, Line(line_p, line_n));
}