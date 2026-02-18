#ifndef _INCLUDE_PRIMITIVES_VECTOR_HPP_
#define _INCLUDE_PRIMITIVES_VECTOR_HPP_

#include "common.hpp"
#include "primitives/point.hpp"
#include <cmath>
#include <iostream>

template <typename T> struct Vector {
    T x_;
    T y_;
    T z_;

    Vector() noexcept : x_(T(0)), y_(T(0)), z_(T(0)) {}
    Vector(const T &x, const T &y, const T &z) noexcept : x_(x), y_(y), z_(z) {}

    /// Vector(a, b), a - begin point, b - end point
    Vector(const Point<T> &a, const Point<T> &b) noexcept {
      x_ = b.x_ - a.x_;
      y_ = b.y_ - a.y_;
      z_ = b.z_ - a.z_;
    }

    static Vector badVector() noexcept {
      return Vector(std::numeric_limits<T>::quiet_NaN(), std::numeric_limits<T>::quiet_NaN(),
                    std::numeric_limits<T>::quiet_NaN());
    }
    bool isBad() const noexcept { return std::isnan(x_) || std::isnan(y_) || std::isnan(z_); }

    /// Methods
    void print(std::ostream &out) const noexcept { out << "v{" << x_ << ", " << y_ << ", " << z_ << "}"; }

    bool isZero() const noexcept { return approx_zero(x_) && approx_zero(y_) && approx_zero(z_); }

    bool equal(const Vector &vec) const noexcept {
      const T dx = x_ - vec.x_;
      const T dy = y_ - vec.y_;
      const T dz = z_ - vec.z_;
      const T diff = std::sqrt(dx * dx + dy * dy + dz * dz);

      const T a_len = std::sqrt(x_ * x_ + y_ * y_ + z_ * z_);
      const T b_len = std::sqrt(vec.x_ * vec.x_ + vec.y_ * vec.y_ + vec.z_ * vec.z_);
      const T scale = std::max(1.0, std::max(a_len, b_len));
      return approx_zero(diff, scale);
    }

    Point<T> toPoint() const noexcept { return Point(x_, y_, z_); }

    /// Operators

    Vector<T> &operator+=(const Vector<T> &vec) {
      x_ += vec.x_;
      y_ += vec.y_;
      z_ += vec.z_;
      return *this;
    }
    Vector<T> &operator-=(const Vector<T> &vec) {
      x_ -= vec.x_;
      y_ -= vec.y_;
      z_ -= vec.z_;
      return *this;
    }
    Vector<T> &operator*=(const Vector<T> &vec) {
      x_ *= vec.x_;
      y_ *= vec.y_;
      z_ *= vec.z_;
      return *this;
    }
    Vector<T> &operator/=(const Vector<T> &vec) {
      x_ /= vec.x_;
      y_ /= vec.y_;
      z_ /= vec.z_;
      return *this;
    }
};

/// Different operators
template <typename T> Vector<T> operator+(const Vector<T> &a, const Vector<T> &b) {
  return Vector(a.x_ + b.x_, a.y_ + b.y_, a.z_ + b.z_);
}

template <typename T> Vector<T> operator-(const Vector<T> &a, const Vector<T> &b) {
  return Vector(a.x_ - b.x_, a.y_ - b.y_, a.z_ - b.z_);
}
template <typename T> Vector<T> operator-(const Point<T> &a, const Point<T> &b) {
  return Vector(a.x_ - b.x_, a.y_ - b.y_, a.z_ - b.z_);
}
template <typename T> T operator*(const Vector<T> &a, const Vector<T> &b) { return a.x_ * b.x_ + a.y_ * b.y_ + a.z_ * b.z_; }
template <typename T> Vector<T> operator*(const Vector<T> &a, T scalar) {
  return Vector(a.x_ * scalar, a.y_ * scalar, a.z_ * scalar);
}
template <typename T> Vector<T> operator*(T scalar, const Vector<T> &a) { return a * scalar; }
template <typename T> T operator*(const Vector<T> &v, const Point<T> &p) { return v.x_ * p.x_ + v.y_ * p.y_ + v.z_ * p.z_; }
template <typename T> T operator*(const Point<T> &p, const Vector<T> &v) { return v * p; }

template <typename T> Vector<T> operator/(const Vector<T> &a, T scalar) { return a * (1 / scalar); }

/// Bool

template <typename T> bool operator==(const Vector<T> &a, const Vector<T> &b) {
  const T dx = a.x_ - b.x_;
  const T dy = a.y_ - b.y_;
  const T dz = a.z_ - b.z_;
  const T diff = std::sqrt(dx * dx + dy * dy + dz * dz);

  const T la = std::sqrt(a.x_ * a.x_ + a.y_ * a.y_ + a.z_ * a.z_);
  const T lb = std::sqrt(b.x_ * b.x_ + b.y_ * b.y_ + b.z_ * b.z_);
  const T scale = std::max(1.0, std::max(la, lb));

  return approx_zero(diff, scale);
}
template <typename T> bool operator!=(const Vector<T> &a, const Vector<T> &b) { return !(a == b); }

/// Support methods

template <typename T> inline T norm(const Vector<T> &v) noexcept { return std::sqrt(v * v); }
template <typename T> inline T norm2(const Vector<T> &v) noexcept { return v * v; }

template <typename T> inline Vector<T> normalize(const Vector<T> &v) noexcept {
  T n = norm(v);
  if (approx_zero(n))
    return Vector<T>::badVector();
  return v / norm(v);
}

template <typename T> inline Vector<T> cross_product(const Vector<T> &a, const Vector<T> &b) noexcept {
  return Vector(a.y_ * b.z_ - a.z_ * b.y_, a.z_ * b.x_ - a.x_ * b.z_, a.x_ * b.y_ - a.y_ * b.x_);
}

/// \vec{ab}, a - beginning, b - ending
template <typename T> inline Vector<T> vec_from_points(const Point<T> &a, const Point<T> &b) noexcept {
  T x = b.x_ - a.x_;
  T y = b.y_ - a.y_;
  T z = b.z_ - a.z_;
  return Vector<T>(x, y, z);
}

#endif