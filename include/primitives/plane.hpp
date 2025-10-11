#ifndef _INCLUDE_PRIMITIVES_PLANE_HPP_
#define _INCLUDE_PRIMITIVES_PLANE_HPP_

#include <iostream>

#include "primitives/line.hpp"
#include "primitives/point.hpp"
#include "primitives/vector.hpp"

enum class PlaneSide { BelongsToPlane = 0, PositiveHalfSpace = 1, NegativeHalfSpace = -1 };

enum class PlaneToPlaneOrientation { Invalid = -1, Coincident = 0, Parallel = 1, Intersect = 2 };

/// Normal-point form:
/// 0: Use: Point p & Vector n, n * (X - P) = 0, X - every dot on plane, P - known dot
/// 1: Use: Scalar s & Vector n, n * X = s, s = n * P - scalar
class Plane {
    Point p_;
    Vector n_;

  public:
    Plane(const Plane &plane) : p_(plane.p_), n_(plane.n_) {}
    Plane(const Point &p, const Vector &n) : p_(p), n_(n) {}
    Plane(const Point &p1, const Point &p2, const Point &p3);

    static Plane badPlane();
    bool isBad() const;

    bool isDegenerate() const;

    Point getP() const { return p_; }
    Vector getN() const { return n_; }

    /// get scalar value for n*P
    double getS() const { return n_ * p_; }

    void print(std::ostream &out) const;

    // Methods
    PlaneSide pointPosition(const Point &x) const;
};

std::pair<PlaneToPlaneOrientation, Line> intersection_2planes(const Plane &p1, const Plane &p2);

#endif