#ifndef _INCLUDE_PRIMITIVES_LINE_HPP_
#define _INCLUDE_PRIMITIVES_LINE_HPP_

#include <iostream>

#include "primitives/point.hpp"
#include "primitives/vector.hpp"

enum class LineToLineOrientation { Invalid = -1, Coincident = 0, Parallel = 1, Skew = 2, Intersect = 3 };

// Parametric: X(t) = P + t*d, t - param
template <typename T> struct Line {
    Point<T> p_;
    Vector<T> d_;

    Line(const Line<T> &l) : p_(l.p_), d_(l.d_) {}
    Line(const Point<T> &p, const Vector<T> &d) : p_(p), d_(d) {}
    Line(const Point<T> &p1, const Point<T> &p2) : p_(p1), d_(Vector(p1, p2)) {}

    static Line badLine() noexcept { return Line(Point<T>::badPoint(), Vector<T>::badVector()); }
    bool isBad() const noexcept { return p_.isBad() || d_.isBad(); }

    bool isDegenerate() const noexcept { return d_.isZero(); }

    void print(std::ostream &out) const noexcept {
      out << "p: ";
      p_.print(out);
      out << ", d: ";
      d_.print(out);
      out << std::endl;
    }
};

template <typename T> inline bool equal(const Line<T> &l1, const Line<T> &l2) noexcept {
  if (!cross_product(l1.d_, l2.d_).isZero())
    return false;
  if (!cross_product(vec_from_points(l1.p_, l2.p_), l1.d_).isZero())
    return false;
  return true;
}

template <typename T> std::pair<LineToLineOrientation, Point<T>> intersection_2lines(const Line<T> &l1, const Line<T> &l2) {
  if (l1.isBad() || l2.isBad())
    return std::make_pair(LineToLineOrientation::Invalid, Point<T>::badPoint());

  Vector<T> d1 = l1.d_, d2 = l2.d_;
  Point<T> p1 = l1.p_, p2 = l2.p_;

  Vector<T> n = cross_product(d1, d2);

  Vector<T> diff_points = vec_from_points(p1, p2); // p2 - p1 vector

  // Case Parallel
  if (approx_zero<T>(norm(n), norm(d1) * norm(d2))) {
    // Case Coincidence
    if (approx_zero(norm(cross_product(diff_points, d1)), norm(diff_points) * norm(d1)))
      return std::make_pair(LineToLineOrientation::Coincident, Point<T>::badPoint());
    return std::make_pair(LineToLineOrientation::Parallel, Point<T>::badPoint());
  }
  T cop = diff_points * n;
  T cop_scale = std::max(1.0, norm(diff_points) * norm(n));
  // Case Skew
  if (!approx_zero(cop, cop_scale))
    return std::make_pair(LineToLineOrientation::Skew, Point<T>::badPoint());

  // Case Intersection
  T t = (cross_product(diff_points, d2) * n) / (n * n);
  Point inter = (p1.toVector() + t * d1).toPoint();
  return std::make_pair(LineToLineOrientation::Intersect, inter);
}

#endif