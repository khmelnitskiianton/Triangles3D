#ifndef _INCLUDE_PRIMITIVES_POINT_HPP_
#define _INCLUDE_PRIMITIVES_POINT_HPP_

#include "common.hpp"
#include <cmath>
#include <iostream>
#include <limits>

template <typename T> struct Vector;

template <typename T> struct Point {
    T x_;
    T y_;
    T z_;

    Point() noexcept : x_(T(0)), y_(T(0)), z_(T(0)) {}
    Point(const T &x, const T &y, const T &z) noexcept : x_(x), y_(y), z_(z) {}
    Point(const Vector<T> &v) : x_(v.x_), y_(v.y_), z_(v.z_) {}

    Vector<T> toVector() const noexcept { return Vector<T>(x_, y_, z_); }

    static Point badPoint() noexcept {
      return Point(std::numeric_limits<T>::quiet_NaN(), std::numeric_limits<T>::quiet_NaN(), std::numeric_limits<T>::quiet_NaN());
    }
    static Point zeroPoint() noexcept { return Point(T(0), T(0), T(0)); }

    bool isBad() const noexcept { return std::isnan(x_) || std::isnan(y_) || std::isnan(z_); }

    void print(std::ostream &out) const noexcept { out << "p{" << x_ << ", " << y_ << ", " << z_ << "}"; }
};

template <typename T> bool operator==(const Point<T> &a, const Point<T> &b) {
  return (approx_equal(a.x_, b.x_)) && (approx_equal(a.y_, b.y_)) && (approx_equal(a.z_, b.z_));
}
template <typename T> bool operator!=(const Point<T> &a, const Point<T> &b) {
  return (!approx_equal(a.x_, b.x_)) || (!approx_equal(a.y_, b.y_)) || (!approx_equal(a.z_, b.z_));
}

template <typename T> inline bool equal(const Point<T> &p1, const Point<T> &p2) noexcept {
  return approx_equal(p1.x_, p2.x_) && approx_equal(p1.y_, p2.y_) && approx_equal(p1.z_, p2.z_);
}

template <typename T> inline T distance(const Point<T> &a, const Point<T> &b) noexcept {
  T dx = a.x_ - b.x_;
  T dy = a.y_ - b.y_;
  T dz = a.z_ - b.z_;
  return std::sqrt(dx * dx + dy * dy + dz * dz);
}

#endif