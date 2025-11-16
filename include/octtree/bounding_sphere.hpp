#ifndef _INCLUDE_OCTTREE_BOUNDING_SPHERE_HPP_
#define _INCLUDE_OCTTREE_BOUNDING_SPHERE_HPP_

#include "primitives/point.hpp"
#include "primitives/vector.hpp"

class Triangle;

class BoundingSphere {
    Point center_;
    double radius_;

  public:
    BoundingSphere() : BoundingSphere(Point::zeroPoint(), 0.0) {}
    BoundingSphere(const Point &center, const double radius) : center_(center), radius_(radius) {}
    BoundingSphere(const Vector &center, const double &radius) : center_(center.toPoint()), radius_(radius) {}
    BoundingSphere(const BoundingSphere &rhs) : center_(rhs.center_), radius_(rhs.radius_) {}

    ~BoundingSphere() = default;
    BoundingSphere &operator=(const BoundingSphere &bb) = default;

    inline Point getCenter() const { return center_; };
    inline double getRadius() const { return radius_; };
};

inline bool overlap(const BoundingSphere &a, const BoundingSphere &b) noexcept {
  const Point ca = a.getCenter();
  const Point cb = b.getCenter();

  const Vector d = vec_from_points(ca, cb); // или просто cb - ca, как у тебя реализовано
  const double dist2 = d * d;               // квадрат расстояния
  const double r = a.getRadius() + b.getRadius();
  return dist2 <= r * r;
}

#endif