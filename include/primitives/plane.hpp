#ifndef _INCLUDE_PRIMITIVES_PLANE_HPP_
#define _INCLUDE_PRIMITIVES_PLANE_HPP_

#include <iostream>

#include "primitives/line.hpp"
#include "primitives/point.hpp"
#include "primitives/vector.hpp"

enum class PlaneSide { BelongsToPlane = 0, PositiveHalfSpace = 1, NegativeHalfSpace = -1 };

enum class PlaneToPlaneOrientation { Invalid = -1, Coincident = 0, Parallel = 1, Intersect = 2 };

/// Normal-point form:
/// 0: Use: Point p & Vector n, n * (X - P) = 0, X - every dot on plane, P - known dot
/// 1: Use: Scalar s & Vector n, n * X = s, s = n * P - scalar
template <typename T> struct Plane {
    Point<T> p_;
    Vector<T> n_;

    Plane(const Plane &plane) : p_(plane.p_), n_(plane.n_) {}
    Plane(const Point<T> &p, const Vector<T> &n) : p_(p), n_(n) {}
    Plane(const Point<T> &p1, const Point<T> &p2, const Point<T> &p3) {
      T x1 = p1.x_, y1 = p1.y_, z1 = p1.z_;
      T x2 = p2.x_, y2 = p2.y_, z2 = p2.z_;
      T x3 = p3.x_, y3 = p3.y_, z3 = p3.z_;
      /// or [B-A x C-A]
      T a = (y2 - y1) * (z3 - z1) - (z2 - z1) * (y3 - y1);
      T b = (z2 - z1) * (x3 - x1) - (x2 - x1) * (z3 - z1);
      T c = (x2 - x1) * (y3 - y1) - (y2 - y1) * (x3 - x1);
      n_ = Vector<T>(a, b, c);
      p_ = p1;
    }

    static Plane badPlane() noexcept { return Plane(Point<T>::badPoint(), Vector<T>::badVector()); }

    bool isBad() const noexcept { return p_.isBad() || n_.isBad(); }

    bool isDegenerate() const noexcept { return n_.isZero(); }

    /// get scalar value for n*P
    T getS() const noexcept { return n_ * p_; }

    void print(std::ostream &out) const noexcept {
      out << "p: ";
      p_.print(out);
      out << ", n: ";
      n_.print(out);
      out << std::endl;
    }

    // Methods
    PlaneSide pointPosition(const Point<T> &x) const noexcept {
      T d = n_ * (x - p_);
      if (approx_zero(d))
        return PlaneSide::BelongsToPlane;
      else if (ge_approx(d, 0.0))
        return PlaneSide::PositiveHalfSpace;
      else
        return PlaneSide::NegativeHalfSpace;
    }
};

template <typename T> std::pair<PlaneToPlaneOrientation, Line<T>> intersection_2planes(const Plane<T> &p1, const Plane<T> &p2) {
  Vector n1 = p1.n_, n2 = p2.n_;
  T s1 = p1.getS(), s2 = p2.getS();

  Vector line_n = cross_product(n1, n2);

  T l1 = norm(n1);
  T l2 = norm(n2);

  if (approx_zero(l1) || approx_zero(l2)) {
    return std::make_pair(PlaneToPlaneOrientation::Invalid, Line<T>::badLine());
  }

  // Case Parallel
  if (approx_zero(norm(line_n), l1 * l1)) {
    T c = (n1 * n2) / (l1 * l2);
    T sign = ge_approx(c, 0.0) ? 1.0 : -1.0;
    T d1 = s1 / l1;
    T d2 = (sign * s2) / l2;
    // Case Coincidence
    if (approx_equal(d1, d2))
      return std::make_pair(PlaneToPlaneOrientation::Coincident, Line<T>::badLine());
    return std::make_pair(PlaneToPlaneOrientation::Parallel, Line<T>::badLine());
  }

  // Case Intersection
  T n1n2 = n1 * n2;
  T n1_norm_sqr = n1 * n1;
  T n2_norm_sqr = n2 * n2;

  T a = (s2 * n1n2 - s1 * n2_norm_sqr) / (n1n2 * n1n2 - n1_norm_sqr * n2_norm_sqr);
  T b = (s1 * n1n2 - s2 * n1_norm_sqr) / (n1n2 * n1n2 - n1_norm_sqr * n2_norm_sqr);
  Vector p_pos_vector = a * n1 + b * n2;
  Point line_p = p_pos_vector.toPoint();
  return std::make_pair(PlaneToPlaneOrientation::Intersect, Line(line_p, line_n));
}

#endif