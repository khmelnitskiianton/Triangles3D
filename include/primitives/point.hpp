#ifndef _INCLUDE_PRIMITIVES_POINT_HPP_
#define _INCLUDE_PRIMITIVES_POINT_HPP_

#include "common.hpp"
#include <cmath>
#include <iostream>

class Vector;

class Point {
    double x_ = NAN;
    double y_ = NAN;
    double z_ = NAN;

  public:
    Point(double x, double y, double z) : x_(x), y_(y), z_(z) {}
    Point(const Point &p) : x_(p.x_), y_(p.y_), z_(p.z_) {}
    Point() noexcept = default;
    Point &operator=(const Point &p) noexcept = default;

    inline double getX() const noexcept { return x_; }
    inline double getY() const noexcept { return y_; }
    inline double getZ() const noexcept { return z_; }

    inline void setX(double x) noexcept { x_ = x; }
    inline void setY(double y) noexcept { y_ = y; }
    inline void setZ(double z) noexcept { z_ = z; }

    Vector toVector() const noexcept;

    static inline Point badPoint() noexcept { return Point(NAN, NAN, NAN); }
    static inline Point zeroPoint() noexcept { return Point(0, 0, 0); }

    inline bool isBad() const noexcept { return std::isnan(x_) || std::isnan(y_) || std::isnan(z_); }

    void print(std::ostream &out) const noexcept;
};

bool operator==(const Point &a, const Point &b);
bool operator!=(const Point &a, const Point &b);

inline bool equal(const Point &p1, const Point &p2) noexcept {
  return approx_equal(p1.getX(), p2.getX()) && approx_equal(p1.getY(), p2.getY()) && approx_equal(p1.getZ(), p2.getZ());
}

inline double distance(const Point &a, const Point &b) noexcept {
  const double dx = a.getX() - b.getX();
  const double dy = a.getY() - b.getY();
  const double dz = a.getZ() - b.getZ();
  return std::sqrt(dx * dx + dy * dy + dz * dz);
}

#endif