#ifndef _INCLUDE_PRIMITIVES_VECTOR_HPP_
#define _INCLUDE_PRIMITIVES_VECTOR_HPP_

#include <cmath>
#include <iostream>

class Point;

class Vector {
    double x_ = NAN;
    double y_ = NAN;
    double z_ = NAN;

  public:
    Vector(const Vector &v) : x_(v.x_), y_(v.y_), z_(v.z_) {}
    Vector(double x, double y, double z) : x_(x), y_(y), z_(z) {}

    /// Vector(a, b), a - begin point, b - end point
    Vector(const Point &a, const Point &b);

    Vector() = default;
    Vector &operator=(const Vector &v) = default;

    /// Getters
    double getX() const { return x_; }
    double getY() const { return y_; }
    double getZ() const { return z_; }

    static Vector badVector();
    bool isBad() const;

    /// Methods
    void print(std::ostream &out) const;
    bool isZero() const;
    bool equal(const Vector &vec) const;

    Point toPoint() const;
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

double norm(const Vector &v);
Vector normalize(const Vector &v);
Vector cross_product(const Vector &a, const Vector &b);

#endif