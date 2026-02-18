#ifndef _INCLUDE_OCTTREE_BOUNDING_SPHERE_HPP_
#define _INCLUDE_OCTTREE_BOUNDING_SPHERE_HPP_

#include "primitives/point.hpp"
#include "primitives/vector.hpp"

template <typename T> struct Triangle;

template <typename T> struct BoundingSphere {
    Point<T> center_;
    T radius_;

    BoundingSphere() : BoundingSphere(Point<T>::zeroPoint(), 0.0) {}
    BoundingSphere(const Point<T> &center, const T radius) : center_(center), radius_(radius) {}
    BoundingSphere(const Vector<T> &center, const T &radius) : center_(center.toPoint()), radius_(radius) {}
    BoundingSphere(const BoundingSphere &rhs) : center_(rhs.center_), radius_(rhs.radius_) {}

    ~BoundingSphere() = default;
    BoundingSphere &operator=(const BoundingSphere &bb) = default;
};

template <typename T> inline bool overlap(const BoundingSphere<T> &a, const BoundingSphere<T> &b) noexcept {
  const Point ca = a.center_;
  const Point cb = b.center_;

  const Vector d = vec_from_points(ca, cb);
  const T dist2 = d * d;
  const T r = a.radius_ + b.radius_;
  return dist2 <= r * r;
} 

#endif