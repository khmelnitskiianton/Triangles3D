#ifndef _INCLUDE_PRIMITIVES_POINT_HPP_
#define _INCLUDE_PRIMITIVES_POINT_HPP_

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

    Point() = default;
    Point &operator=(const Point &p) = default;

    double getX() const { return x_; }
    double getY() const { return y_; }
    double getZ() const { return z_; }

    Vector toVector() const;

    static Point badPoint();
    bool isBad() const;

    void print(std::ostream &out) const;
};

bool operator==(const Point &a, const Point &b);
bool operator!=(const Point &a, const Point &b);

bool equal(const Point &p1, const Point &p2);

/// \vec{ab}, a - beginning, b - ending
Vector vec_from_points(const Point &a, const Point b);

#endif