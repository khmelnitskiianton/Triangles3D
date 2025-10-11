#include <algorithm>
#include <iostream>

#include "primitives/common.hpp"
#include "primitives/point.hpp"
#include "primitives/segment.hpp"
#include "primitives/vector.hpp"

void Segment::print(std::ostream &out) const {
  out << "a: ";
  a_.print(out);
  out << " b: ";
  b_.print(out);
  out << std::endl;
}

Segment Segment::badSegment() { return Segment(Point::badPoint(), Point::badPoint()); }

bool Segment::isBad() const { return a_.isBad() || b_.isBad(); }

bool Segment::isInclude(const Point &p) const {
  if (p.isBad())
    return false;

  Vector u = vec_from_points(a_, b_);
  Vector v = vec_from_points(a_, p);
  if (!cross_product(u, v).isZero())
    return false;
  double scalar = u * v;
  double segment_scalar = u * u;
  if ((scalar < 0) || (scalar > segment_scalar))
    return false;
  return true;
}

bool Segment::isDegenerate() const {
  if (this->isBad())
    return false;
  return a_ == b_;
}

bool equal(const Segment &s1, const Segment &s2) {
  Point a = s1.getA(), b = s1.getB(), c = s2.getA(), d = s2.getB();
  Vector v = vec_from_points(a, b);
  Vector ac = vec_from_points(a, c);
  Vector ad = vec_from_points(a, d);
  if (!cross_product(ac, v).isZero() || !cross_product(ad, v).isZero())
    return false;
  double t_c = (ac * v) / (v * v);
  double t_d = (ad * v) / (v * v);
  if (compare_doubles(std::min(t_c, t_d), 0) && compare_doubles(std::max(t_c, t_d), 1))
    return true;
  return false;
}

bool intersection_2segments(const Segment &s1, const Segment &s2) {
  Point a = s1.getA(), b = s1.getB(), c = s2.getA(), d = s2.getB();
  Vector v = vec_from_points(a, b);
  Vector proj_c = vec_from_points(a, c);
  Vector proj_d = vec_from_points(a, d);
  double t_c = (proj_c * v) / (v * v);
  double t_d = (proj_d * v) / (v * v);
  // Intersection
  if (std::max(0.0, std::min(t_c, t_d)) <= std::min(1.0, std::max(t_c, t_d)))
    return true;
  return false;
}