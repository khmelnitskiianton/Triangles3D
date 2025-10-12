#include <cmath>

#include "primitives/common.hpp"
#include "primitives/point.hpp"
#include "primitives/vector.hpp"

/// vec(ab) == a->b
Vector::Vector(const Point &a, const Point &b) {
  x_ = b.getX() - a.getX();
  y_ = b.getY() - a.getY();
  z_ = b.getZ() - a.getZ();
}

/// Member operators

Vector &Vector::operator+=(const Vector &vec) {
  x_ += vec.x_;
  y_ += vec.y_;
  z_ += vec.z_;
  return *this;
}
Vector &Vector::operator-=(const Vector &vec) {
  x_ -= vec.x_;
  y_ -= vec.y_;
  z_ -= vec.z_;
  return *this;
}
Vector &Vector::operator*=(const Vector &vec) {
  x_ *= vec.x_;
  y_ *= vec.y_;
  z_ *= vec.z_;
  return *this;
}
Vector &Vector::operator/=(const Vector &vec) {
  x_ /= vec.x_;
  y_ /= vec.y_;
  z_ /= vec.z_;
  return *this;
}

/// Class methods

void Vector::print(std::ostream &out) const { out << "v{" << x_ << ", " << y_ << ", " << z_ << "}"; }

Vector Vector::badVector() { return Vector(NAN, NAN, NAN); }

bool Vector::isBad() const { return std::isnan(x_) || std::isnan(y_) || std::isnan(z_); }

bool Vector::isZero() const {
  if (compare_doubles(x_, 0) && compare_doubles(y_, 0) && compare_doubles(z_, 0))
    return true;
  return false;
}

bool Vector::equal(const Vector &vec) const {
  if (vec.isBad() || this->isBad())
    return false;

  if (compare_doubles(x_, vec.x_) && compare_doubles(y_, vec.y_) && compare_doubles(z_, vec.z_))
    return true;
  return false;
}

Point Vector::toPoint() const { return Point(x_, y_, z_); }

/// Operators

Vector operator+(const Vector &a, const Vector &b) {
  return Vector(a.getX() + b.getX(), a.getY() + b.getY(), a.getZ() + b.getZ());
}

Vector operator-(const Vector &a, const Vector &b) {
  return Vector(a.getX() - b.getX(), a.getY() - b.getY(), a.getZ() - b.getZ());
}
Vector operator-(const Point &a, const Point &b) { return Vector(a.getX() - b.getX(), a.getY() - b.getY(), a.getZ() - b.getZ()); }

double operator*(const Vector &a, const Vector &b) { return a.getX() * b.getX() + a.getY() * b.getY() + a.getZ() * b.getZ(); }

Vector operator*(const Vector &a, double scalar) { return Vector(a.getX() * scalar, a.getY() * scalar, a.getZ() * scalar); }
Vector operator*(double scalar, const Vector &a) { return a * scalar; }
double operator*(const Vector &v, const Point &p) { return v.getX() * p.getX() + v.getY() * p.getY() + v.getZ() * p.getZ(); }
double operator*(const Point &p, const Vector &v) { return v * p; }


Vector operator/(const Vector &a, double scalar) { return a * (1 / scalar); }

// Bool
bool operator==(const Vector &a, const Vector &b) {
  return (!compare_doubles(a.getX(), b.getX())) && (!compare_doubles(a.getY(), b.getY())) &&
         (!compare_doubles(a.getZ(), b.getZ()));
}

bool operator!=(const Vector &a, const Vector &b) { return !(a == b); }

/// Other methods

double norm(const Vector &v) { return std::sqrt(v * v); }
double norm2(const Vector &v) { return v * v; }

Vector normalize(const Vector &v) {
  double n = norm(v);
  if (compare_doubles(n, 0))
    return Vector::badVector();
  return v / norm(v);
}

Vector cross_product(const Vector &a, const Vector &b) {
  if (a.isBad() || b.isBad())
    return Vector::badVector();

  return Vector(a.getY() * b.getZ() - a.getZ() * b.getY(), a.getZ() * b.getX() - a.getX() * b.getZ(),
                a.getX() * b.getY() - a.getY() * b.getX());
}