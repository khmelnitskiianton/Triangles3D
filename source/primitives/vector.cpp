#include <cmath>

#include "common.hpp"
#include "primitives/vector.hpp"

/// vec(ab) == a->b
Vector::Vector(const Point &a, const Point &b) noexcept {
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

void Vector::print(std::ostream &out) const noexcept { out << "v{" << x_ << ", " << y_ << ", " << z_ << "}"; }

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
  const double dx = a.getX() - b.getX();
  const double dy = a.getY() - b.getY();
  const double dz = a.getZ() - b.getZ();
  const double diff = std::sqrt(dx * dx + dy * dy + dz * dz);

  const double la = std::sqrt(a.getX() * a.getX() + a.getY() * a.getY() + a.getZ() * a.getZ());
  const double lb = std::sqrt(b.getX() * b.getX() + b.getY() * b.getY() + b.getZ() * b.getZ());
  const double scale = std::max(1.0, std::max(la, lb));

  return approx_zero(diff, scale);
}

bool operator!=(const Vector &a, const Vector &b) { return !(a == b); }