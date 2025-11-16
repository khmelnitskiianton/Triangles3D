#ifndef _INCLUDE_PRIMITIVES_LINE_HPP_
#define _INCLUDE_PRIMITIVES_LINE_HPP_

#include <iostream>

#include "primitives/point.hpp"
#include "primitives/vector.hpp"

enum class LineToLineOrientation { Invalid = -1, Coincident = 0, Parallel = 1, Skew = 2, Intersect = 3 };

// Parametric: X(t) = P + t*d, t - param
class Line {
    Point p_;
    Vector d_;

  public:
    Line(const Line &l) : p_(l.p_), d_(l.d_) {}
    Line(const Point &p, const Vector &d) : p_(p), d_(d) {}
    Line(const Point &p1, const Point &p2) : p_(p1), d_(Vector(p1, p2)) {}

    static inline Line badLine() noexcept { return Line(Point::badPoint(), Vector::badVector()); }
    inline bool isBad() const noexcept { return p_.isBad() || d_.isBad(); }

    inline bool isDegenerate() const noexcept { return d_.isZero(); }

    /// Getters
    inline Point getP() const noexcept { return p_; }
    inline Vector getD() const noexcept { return d_; }

    void print(std::ostream &out) const noexcept;
};

inline bool equal(const Line &l1, const Line &l2) noexcept {
  if (!cross_product(l1.getD(), l2.getD()).isZero())
    return false;
  if (!cross_product(vec_from_points(l1.getP(), l2.getP()), l1.getD()).isZero())
    return false;
  return true;
}

std::pair<LineToLineOrientation, Point> intersection_2lines(const Line &l1, const Line &l2);

#endif