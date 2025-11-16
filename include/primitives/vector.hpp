#ifndef _INCLUDE_PRIMITIVES_VECTOR_HPP_
#define _INCLUDE_PRIMITIVES_VECTOR_HPP_

#include "common.hpp"
#include "primitives/point.hpp"
#include <cmath>
#include <iostream>

class Vector {
    double x_ = NAN;
    double y_ = NAN;
    double z_ = NAN;

  public:
    Vector(const Vector &v) : x_(v.x_), y_(v.y_), z_(v.z_) {}
    Vector(double x, double y, double z) : x_(x), y_(y), z_(z) {}

    /// Vector(a, b), a - begin point, b - end point
    Vector(const Point &a, const Point &b) noexcept;

    Vector() noexcept = default;
    Vector &operator=(const Vector &v) noexcept = default;

    /// Getters
    inline double getX() const noexcept { return x_; }
    inline double getY() const noexcept { return y_; }
    inline double getZ() const noexcept { return z_; }

    static inline Vector badVector() noexcept { return Vector(NAN, NAN, NAN); }

    inline bool isBad() const noexcept { return std::isnan(x_) || std::isnan(y_) || std::isnan(z_); }

    /// Methods
    void print(std::ostream &out) const noexcept;

    inline bool isZero() const noexcept { return approx_zero(x_) && approx_zero(y_) && approx_zero(z_); }

    inline bool equal(const Vector &vec) const noexcept {
      const double dx = x_ - vec.x_;
      const double dy = y_ - vec.y_;
      const double dz = z_ - vec.z_;
      const double diff = std::sqrt(dx * dx + dy * dy + dz * dz);

      const double a_len = std::sqrt(x_ * x_ + y_ * y_ + z_ * z_);
      const double b_len = std::sqrt(vec.x_ * vec.x_ + vec.y_ * vec.y_ + vec.z_ * vec.z_);
      const double scale = std::max(1.0, std::max(a_len, b_len));
      return approx_zero(diff, scale);
    }

    inline Point toPoint() const noexcept { return Point(x_, y_, z_); }
    /// Operators
    Vector &operator+=(const Vector &vec);
    Vector &operator-=(const Vector &vec);
    Vector &operator*=(const Vector &vec);
    Vector &operator/=(const Vector &vec);
};

/// Different operators
Vector operator+(const Vector &a, const Vector &b);

Vector operator-(const Vector &a, const Vector &b);
Vector operator-(const Point &a, const Point &b);

double operator*(const Vector &a, const Vector &b);
Vector operator*(const Vector &a, double scalar);
Vector operator*(double scalar, const Vector &a);

double operator*(const Vector &v, const Point &p);
double operator*(const Point &p, const Vector &v);

Vector operator/(const Vector &a, double scalar);

bool operator==(const Vector &a, const Vector &b);
bool operator!=(const Vector &a, const Vector &b);

/// Support methods

inline double norm(const Vector &v) noexcept { return std::sqrt(v * v); }
inline double norm2(const Vector &v) noexcept { return v * v; }

inline Vector normalize(const Vector &v) noexcept {
  double n = norm(v);
  if (approx_zero(n))
    return Vector::badVector();
  return v / norm(v);
}

inline Vector cross_product(const Vector &a, const Vector &b) noexcept {
  return Vector(a.getY() * b.getZ() - a.getZ() * b.getY(), a.getZ() * b.getX() - a.getX() * b.getZ(),
                a.getX() * b.getY() - a.getY() * b.getX());
}

/// \vec{ab}, a - beginning, b - ending
inline Vector vec_from_points(const Point &a, const Point &b) noexcept {
  double x = b.getX() - a.getX();
  double y = b.getY() - a.getY();
  double z = b.getZ() - a.getZ();
  return Vector(x, y, z);
}

#endif