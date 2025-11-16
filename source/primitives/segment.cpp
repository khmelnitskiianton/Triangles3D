#include <algorithm>
#include <iostream>

#include "common.hpp"
#include "primitives/line.hpp"
#include "primitives/point.hpp"
#include "primitives/segment.hpp"
#include "primitives/vector.hpp"

void Segment::print(std::ostream &out) const noexcept {
  out << "a: ";
  a_.print(out);
  out << " b: ";
  b_.print(out);
  out << std::endl;
}

Segment Segment::badSegment() { return Segment(Point::badPoint(), Point::badPoint()); }

bool Segment::isInclude(const Point &p) const noexcept {
  if (this->isDegenerate())
    return a_ == p;

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


bool equal(const Segment &s1, const Segment &s2) noexcept {
  Point a = s1.getA(), b = s1.getB(), c = s2.getA(), d = s2.getB();
  Vector v = vec_from_points(a, b);
  Vector ac = vec_from_points(a, c);
  Vector ad = vec_from_points(a, d);
  if (!cross_product(ac, v).isZero() || !cross_product(ad, v).isZero())
    return false;
  double t_c = (ac * v) / (v * v);
  double t_d = (ad * v) / (v * v);
  if (approx_equal(std::min(t_c, t_d), 0) && approx_equal(std::max(t_c, t_d), 1))
    return true;
  return false;
}

/// On one line also with degeneration!
bool intersection_2segments_on_line(const Segment &s1, const Segment &s2) {
  // Case s1, s2 - points
  if (s1.isDegenerate() && s2.isDegenerate()) {
    return equal(s1.getA(), s2.getA());
  }
  if (s1.isDegenerate())
    return s2.isInclude(s1.getA());

  if (s2.isDegenerate())
    return s1.isInclude(s2.getA());

  // Case s1, s2 not points
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

bool intersection_2segments(const Segment &s1, const Segment &s2) {
  // Case s1 or s2 - points
  if (s1.isDegenerate() && s2.isDegenerate()) {
    return equal(s1.getA(), s2.getA());
  }
  if (s1.isDegenerate())
    return s2.isInclude(s1.getA());

  if (s2.isDegenerate())
    return s1.isInclude(s2.getA());

  // Not degenerate case
  Line l1 = s1.findLine();
  Line l2 = s2.findLine();
  std::pair<LineToLineOrientation, Point> inter = intersection_2lines(l1, l2);
  if ((inter.first == LineToLineOrientation::Parallel) || (inter.first == LineToLineOrientation::Skew) ||
      (inter.first == LineToLineOrientation::Invalid))
    return false;
  if (inter.first == LineToLineOrientation::Coincident)
    return intersection_2segments_on_line(s1, s2);

  // Case intersection for lines
  Point p = inter.second;
  if (s1.isInclude(p) && s2.isInclude(p))
    return true;

  return false;
}