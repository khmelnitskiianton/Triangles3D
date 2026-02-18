#ifndef _INCLUDE_PRIMITIVES_SEGMENT_HPP_
#define _INCLUDE_PRIMITIVES_SEGMENT_HPP_

#include "primitives/line.hpp"
#include "primitives/point.hpp"

// ab segment
template <typename T> struct Segment {
    Point<T> a_;
    Point<T> b_;

    Segment(const Point<T> &a, const Point<T> &b) : a_(a), b_(b) {}
    Segment(const Segment &s) : a_(s.a_), b_(s.b_) {}

    static Segment<T> badSegment() { return Segment(Point<T>::badPoint(), Point<T>::badPoint()); }
    bool isBad() const noexcept { return a_.isBad() || b_.isBad(); }

    Line<T> findLine() const noexcept { return Line(a_, b_); }

    bool isInclude(const Point<T> &p) const noexcept {
      if (this->isDegenerate())
        return a_ == p;

      Vector u = vec_from_points(a_, b_);
      Vector v = vec_from_points(a_, p);
      if (!cross_product(u, v).isZero())
        return false;
      T scalar = u * v;
      T segment_scalar = u * u;
      if ((scalar < 0) || (scalar > segment_scalar))
        return false;
      return true;
    }

    bool isDegenerate() const noexcept { return a_ == b_; }
    void print(std::ostream &out) const noexcept {
      out << "a: ";
      a_.print(out);
      out << " b: ";
      b_.print(out);
      out << std::endl;
    }
};

template <typename T> bool equal(const Segment<T> &s1, const Segment<T> &s2) noexcept {
  Point a = s1.a_, b = s1.b_, c = s2.a_, d = s2.b_;
  Vector v = vec_from_points(a, b);
  Vector ac = vec_from_points(a, c);
  Vector ad = vec_from_points(a, d);
  if (!cross_product(ac, v).isZero() || !cross_product(ad, v).isZero())
    return false;
  T t_c = (ac * v) / (v * v);
  T t_d = (ad * v) / (v * v);
  if (approx_equal<T>(std::min(t_c, t_d), 0) && approx_equal<T>(std::max(t_c, t_d), 1))
    return true;
  return false;
}

/// Intersection of 2 segments lying on one line(also degenerated cases)
template <typename T> bool intersection_2segments_on_line(const Segment<T> &s1, const Segment<T> &s2) {
  // Case s1, s2 - points
  if (s1.isDegenerate() && s2.isDegenerate()) {
    return equal(s1.a_, s2.a_);
  }
  if (s1.isDegenerate())
    return s2.isInclude(s1.a_);

  if (s2.isDegenerate())
    return s1.isInclude(s2.a_);

  // Case s1, s2 not points
  Point a = s1.a_, b = s1.b_, c = s2.a_, d = s2.b_;
  Vector v = vec_from_points(a, b);
  Vector proj_c = vec_from_points(a, c);
  Vector proj_d = vec_from_points(a, d);
  T t_c = (proj_c * v) / (v * v);
  T t_d = (proj_d * v) / (v * v);
  // Intersection
  if (std::max(0.0, std::min(t_c, t_d)) <= std::min(1.0, std::max(t_c, t_d)))
    return true;
  return false;
}
/// Intersection of 2 segments in space(also degenerated cases)
template <typename T> bool intersection_2segments(const Segment<T> &s1, const Segment<T> &s2) {
  // Case s1 or s2 - points
  if (s1.isDegenerate() && s2.isDegenerate()) {
    return equal(s1.a_, s2.a_);
  }
  if (s1.isDegenerate())
    return s2.isInclude(s1.a_);

  if (s2.isDegenerate())
    return s1.isInclude(s2.a_);

  // Not degenerate case
  Line l1 = s1.findLine();
  Line l2 = s2.findLine();
  std::pair<LineToLineOrientation, Point<T>> inter = intersection_2lines(l1, l2);
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

#endif