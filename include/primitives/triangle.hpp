#ifndef _INCLUDE_PRIMITIVES_TRIANGLES_HPP_
#define _INCLUDE_PRIMITIVES_TRIANGLES_HPP_

#include <iostream>
#include <optional>
#include <variant>

#include "octtree/bounding_box.hpp"
#include "octtree/bounding_sphere.hpp"
#include "primitives/plane.hpp"
#include "primitives/point.hpp"
#include "primitives/segment.hpp"

using Collapsed = typename std::variant<std::monostate, Point, Segment>;

Collapsed collapsedTriangle(Point a, Point b, Point c);

class Triangle {
    Point a_;
    Point b_;
    Point c_;

    Plane plane_;
    Collapsed collapsed_;
    BoundingBox box_;

  public:
    Triangle(const Point &a, const Point &b, const Point &c)
        : a_(a), b_(b), c_(c), plane_(Plane(a_, b_, c_)), collapsed_(collapsedTriangle(a_, b_, c_)),
          box_(BoundingBox(a_, b_, c_)) {}
    Triangle(const Triangle &t) : a_(t.a_), b_(t.b_), c_(t.c_), plane_(t.plane_), collapsed_(t.collapsed_), box_(t.box_) {}
    Triangle &operator=(const Triangle &) noexcept = default;

    inline Point getA() const noexcept { return a_; }
    inline Point getB() const noexcept { return b_; }
    inline Point getC() const noexcept { return c_; }

    inline Plane getPlane() const noexcept { return plane_; }
    inline Collapsed getCollapsed() const noexcept { return collapsed_; }
    inline BoundingBox getBox() const noexcept { return box_; }

    Triangle badTriangle() noexcept { return Triangle(Point::badPoint(), Point::badPoint(), Point::badPoint()); }
    bool isBad() const noexcept { return a_.isBad() || b_.isBad() || c_.isBad(); }


    bool isDegenerate() const noexcept {
      const Collapsed &c = getCollapsed();
      return !std::holds_alternative<std::monostate>(c);
    }

    bool isInclude(const Point &p) const noexcept;

    BoundingSphere getBoundingSphere() const noexcept;

    /// Methods
    void print(std::ostream &out) const noexcept;
};

std::optional<Segment> intersection_triangle_line_on_plane(const Triangle &t, const Line &l);
std::optional<Segment> intersection_triangle_line(const Triangle &t, const Line &s);

bool intersection_triangle_segment(const Triangle &t, const Segment &s);

bool intersection_2triangles_intersect_planes(const Triangle &t1, const Triangle &t2, const Line &l);
bool intersection_2triangles_coincident_planes(const Triangle &t1, const Triangle &t2);
bool intersection_2triangles(const Triangle &t1, const Triangle &t2);

#endif