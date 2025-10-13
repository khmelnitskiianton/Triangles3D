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
  if (approx_zero(x_) && approx_zero(y_) && approx_zero(z_))
    return true;
  return false;
}

bool Vector::equal(const Vector &vec) const {
  if (vec.isBad() || this->isBad())
    return false;

  const double dx = x_ - vec.x_;
  const double dy = y_ - vec.y_;
  const double dz = z_ - vec.z_;
  const double diff = std::sqrt(dx * dx + dy * dy + dz * dz);

  const double a_len = std::sqrt(x_ * x_ + y_ * y_ + z_ * z_);
  const double b_len = std::sqrt(vec.x_ * vec.x_ + vec.y_ * vec.y_ + vec.z_ * vec.z_);
  const double scale = std::max(1.0, std::max(a_len, b_len));
  return approx_zero(diff, scale);
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

/// Other methods

double norm(const Vector &v) { return std::sqrt(v * v); }
double norm2(const Vector &v) { return v * v; }

Vector normalize(const Vector &v) {
  double n = norm(v);
  if (approx_zero(n))
    return Vector::badVector();
  return v / norm(v);
}

Vector cross_product(const Vector &a, const Vector &b) {
  if (a.isBad() || b.isBad())
    return Vector::badVector();

  return Vector(a.getY() * b.getZ() - a.getZ() * b.getY(), a.getZ() * b.getX() - a.getX() * b.getZ(),
                a.getX() * b.getY() - a.getY() * b.getX());
}