#ifndef _INCLUDE_TRIANGLES_HPP_
#define _INCLUDE_TRIANGLES_HPP_

#include <iostream>
#include <optional>
#include <variant>

#include "primitives/plane.hpp"
#include "primitives/point.hpp"
#include "primitives/segment.hpp"

using Collapsed = typename std::variant<std::monostate, Point, Segment>;

class Triangle {
    Point a_;
    Point b_;
    Point c_;

  public:
    Triangle(const Point &a, const Point &b, const Point &c) : a_(a), b_(b), c_(c) {}
    Triangle(const Triangle &t) : a_(t.a_), b_(t.b_), c_(t.c_) {}

    Point getA() const { return a_; }
    Point getB() const { return b_; }
    Point getC() const { return c_; }

    Plane findPlane() const;

    static Triangle badTriangle();
    bool isBad() const;

    bool isDegenerate() const;
    Collapsed collapsedTriangle() const;

    bool isInclude(const Point &p) const;

    /// Methods
    void print(std::ostream &out) const;
};

std::optional<Segment> intersection_triangle_line_on_plane(const Triangle &t, const Line &l);
std::optional<Segment> intersection_triangle_line(const Triangle &t, const Line &s);

bool intersection_triangle_segment(const Triangle &t, const Segment &s);

bool intersection_2triangles_intersect_planes(const Triangle &t1, const Triangle &t2);
bool intersection_2triangles_coincident_planes(const Triangle &t1, const Triangle &t2);
bool intersection_2triangles(const Triangle &t1, const Triangle &t2);

#endif