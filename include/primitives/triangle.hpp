#ifndef _INCLUDE_PRIMITIVES_TRIANGLES_HPP_
#define _INCLUDE_PRIMITIVES_TRIANGLES_HPP_

#include <iostream>
#include <algorithm>
#include <optional>
#include <variant>

#include "octtree/bounding_box.hpp"
#include "octtree/bounding_sphere.hpp"
#include "primitives/plane.hpp"
#include "primitives/point.hpp"
#include "primitives/segment.hpp"

template <typename T> using Collapsed = typename std::variant<std::monostate, Point<T>, Segment<T>>;

template <typename T> Collapsed<T> collapsedTriangle(Point<T> a, Point<T> b, Point<T> c) {
  // 1) Check degeneracy by area ~ ||(B-A) x (C-A)||^2
  Vector<T> AB = vec_from_points(a, b);
  Vector<T> AC = vec_from_points(a, c);
  Vector<T> n = cross_product(AB, AC);
  T n2 = n * n;

  // Proper (non-degenerate) triangle -> not collapsed
  if (!approx_zero<T>(n2))
    return std::monostate{};

  // 2) Degenerate: choose farthest pair among the three vertices
  auto d2 = [&](const Point<T> &p, const Point<T> &q) {
    return norm2(vec_from_points(p, q)); // squared distance
  };

  T dab = d2(a, b);
  T dac = d2(a, c);
  T dbc = d2(b, c);

  const Point<T> *p = &a;
  const Point<T> *q = &b;
  T dmax = dab;

  if (dac >= dmax) {
    p = &a;
    q = &c;
    dmax = dac;
  }
  if (dbc >= dmax) {
    p = &b;
    q = &c;
    dmax = dbc;
  }

  // 3) If even the longest edge is ~0, all points coincide -> point
  if (approx_zero(dmax))
    return Point<T>(*p);

  // 4) Otherwise collapsed triangle is that longest edge
  return Segment<T>(*p, *q);
}

template <typename T> struct Triangle {
    Point<T> a_;
    Point<T> b_;
    Point<T> c_;

    Plane<T> plane_;
    Collapsed<T> collapsed_;
    BoundingBox<T> box_;

    Triangle(const Point<T> &a, const Point<T> &b, const Point<T> &c)
        : a_(a), b_(b), c_(c), plane_(Plane(a_, b_, c_)), collapsed_(collapsedTriangle(a_, b_, c_)),
          box_(BoundingBox(a_, b_, c_)) {}
    Triangle(const Triangle &t) : a_(t.a_), b_(t.b_), c_(t.c_), plane_(t.plane_), collapsed_(t.collapsed_), box_(t.box_) {}
    Triangle &operator=(const Triangle &) noexcept = default;

    Triangle badTriangle() noexcept { return Triangle(Point<T>::badPoint(), Point<T>::badPoint(), Point<T>::badPoint()); }
    bool isBad() const noexcept { return a_.isBad() || b_.isBad() || c_.isBad(); }


    bool isDegenerate() const noexcept { return !std::holds_alternative<std::monostate>(collapsed_); }

    bool isInclude(const Point<T> &p) const noexcept {
      Vector n_p = plane_.n_;

      // not on a plane
      Vector ap = vec_from_points(a_, p);
      T offset = n_p * ap;
      T plane_scale = std::max(1.0, norm(n_p) * norm(ap));
      if (!approx_zero(offset, plane_scale))
        return false;

      T c1 = n_p * cross_product(vec_from_points(b_, a_), vec_from_points(p, a_));
      T c2 = n_p * cross_product(vec_from_points(c_, b_), vec_from_points(p, b_));
      T c3 = n_p * cross_product(vec_from_points(a_, c_), vec_from_points(p, c_));

      // Treat "nonnegative"
      // Treat "nonpositive"
      auto geq0 = [&](T x) { return ge_approx(x, 0.0); };
      auto leq0 = [&](T x) { return le_approx(x, 0.0); };

      bool nonNeg = geq0(c1) && geq0(c2) && geq0(c3);
      bool nonPos = leq0(c1) && leq0(c2) && leq0(c3);
      return nonNeg || nonPos;
    }

    BoundingSphere<T> getBoundingSphere() const noexcept {
      const Vector<T> a = a_.toVector();
      const Vector<T> b = b_.toVector();
      const Vector<T> c = c_.toVector();

      const Vector center = (a + b + c) * (1.0 / 3.0);
      const T r = std::max({distance(a.toPoint(), center.toPoint()), distance(b.toPoint(), center.toPoint()),
                            distance(c.toPoint(), center.toPoint())});
      return BoundingSphere(center, r);
    }

    /// Methods
    void print(std::ostream &out) const noexcept {
      out << "a: ";
      a_.print(out);
      out << ", b: ";
      b_.print(out);
      out << ", c: ";
      c_.print(out);
      out << std::endl;
    }
};

template <typename T> std::optional<Segment<T>> intersection_triangle_line(const Triangle<T> &t, const Line<T> &l) {
  Plane<T> pt = t.plane_;
  Vector<T> n = pt.n_;
  T s = pt.getS();

  Point<T> p0 = l.p_; // any point on the line
  Vector<T> v = l.d_; // line direction (non-zero)

  // n·(p0 + t v) = s  ->  t = (s - n·p0) / (n·v)
  T nv = n * v; // dot
  T np0 = n * Vector<T>(p0.x_, p0.y_, p0.z_);

  // Scale for "n·v" should reflect |n|·|v|
  T nv_scale = n * n + v * v;
  if (approx_zero(nv, nv_scale)) {
    T s_scale = std::max(1.0, std::fabs(s)); // scale approx |s| or 1
    // Line is parallel to plane
    if (approx_zero(np0 - s, s_scale)) {
      // Complanar case, on one plane
      std::optional<Segment<T>> result = intersection_triangle_line_on_plane(t, l);
      if (result)
        return result.value();
      return std::nullopt;
    }
    // in parallel planes, no intersection
    return std::nullopt;
  }
  T tparam = (s - np0) / nv;
  Point<T> x = (p0.toVector() + v * tparam).toPoint(); // point of line-plane intersection
  if (t.isInclude(x))
    return Segment<T>(x, x);
  return std::nullopt;
}

template <typename T> bool intersection_triangle_segment(const Triangle<T> &t, const Segment<T> &s) {
  // Degenerate segment: reduce to point-in-triangle
  if (s.isDegenerate()) {
    return t.isInclude(s.a_);
  }

  Line l = s.findLine();
  std::optional<Segment<T>> opt_res = intersection_triangle_line(t, l);
  if (!opt_res)
    return false;

  Segment<T> inter_line = *opt_res;
  return intersection_2segments_on_line<T>(inter_line, s);
}


template <typename T> std::optional<Segment<T>> intersection_triangle_line_on_plane(const Triangle<T> &t, const Line<T> &l) {
  Line<T> ab = Line<T>(t.a_, t.b_);
  Line<T> ac = Line<T>(t.a_, t.c_);
  Line<T> bc = Line<T>(t.b_, t.c_);

  size_t idx = 0;
  std::array<Point<T>, 2> intersect_points{Point<T>::badPoint(), Point<T>::badPoint()};

  auto push_unique = [&](const Point<T> &q) {
    // dedup against already collected points
    for (size_t i = 0; i < idx; ++i) {
      if (equal<T>(intersect_points[i], q))
        return; // already have it
    }
    if (idx < 2) {
      intersect_points[idx++] = q;
    }
  };

  // Intersection line ab with l
  std::pair<LineToLineOrientation, Point<T>> pair_intersect_ab = intersection_2lines(ab, l);
  if (pair_intersect_ab.first == LineToLineOrientation::Coincident) {
    return Segment<T>(t.a_, t.b_);
  }
  if (pair_intersect_ab.first == LineToLineOrientation::Intersect) {
    Point<T> inter_ab = pair_intersect_ab.second;
    if (Segment<T>(t.a_, t.b_).isInclude(inter_ab)) {
      push_unique(inter_ab);
      if (idx == 2)
        return Segment<T>(intersect_points[0], intersect_points[1]);
    }
  }

  // Intersection line ac with l
  std::pair<LineToLineOrientation, Point<T>> pair_intersect_ac = intersection_2lines(ac, l);
  if (pair_intersect_ac.first == LineToLineOrientation::Coincident) {
    return Segment<T>(t.a_, t.c_);
  }
  if (pair_intersect_ac.first == LineToLineOrientation::Intersect) {
    Point<T> inter_ac = pair_intersect_ac.second;
    if (Segment<T>(t.a_, t.c_).isInclude(inter_ac)) {
      push_unique(inter_ac);
      if (idx == 2)
        return Segment<T>(intersect_points[0], intersect_points[1]);
    }
  }

  // Intersection line bc with l
  std::pair<LineToLineOrientation, Point<T>> pair_intersect_bc = intersection_2lines(bc, l);
  if (pair_intersect_bc.first == LineToLineOrientation::Coincident) {
    return Segment<T>(t.b_, t.c_);
  }
  if (pair_intersect_bc.first == LineToLineOrientation::Intersect) {
    Point<T> inter_bc = pair_intersect_bc.second;
    if (Segment<T>(t.b_, t.c_).isInclude(inter_bc)) {
      push_unique(inter_bc);
      if (idx == 2)
        return Segment<T>(intersect_points[0], intersect_points[1]);
    }
  }

  // Check if there one or more intersect points
  if (idx == 0)
    return std::nullopt;
  if (idx == 1)
    return Segment<T>(intersect_points[0], intersect_points[0]); // degenerated segment
  if (idx == 2)
    return Segment<T>(intersect_points[0], intersect_points[1]); // degenerated segment

  // Bad
  return std::nullopt;
}

/// This method for case intersection 2 planes
template <typename T>
bool intersection_2triangles_intersect_planes(const Triangle<T> &t1, const Triangle<T> &t2, const Line<T> &l) {
  std::optional<Segment<T>> opt_seg1 = intersection_triangle_line_on_plane(t1, l);
  std::optional<Segment<T>> opt_seg2 = intersection_triangle_line_on_plane(t2, l);
  if (!opt_seg1 || !opt_seg2)
    return false;
  Segment<T> seg1 = opt_seg1.value();
  Segment<T> seg2 = opt_seg2.value();
  return intersection_2segments_on_line(seg1, seg2);
}

/// This method for case 2 triangles on one plane
template <typename T> bool intersection_2triangles_coincident_planes(const Triangle<T> &t1, const Triangle<T> &t2) {
  std::array<Point<T>, 3> t1_arr = {t1.a_, t1.b_, t1.c_};
  std::array<Point<T>, 3> t2_arr = {t2.a_, t2.b_, t2.c_};

  // Check that some edge of one triangle lies in triangle
  for (size_t i = 0; i < 3; i++)
    if (t1.isInclude(t2_arr[i]))
      return true;
  for (size_t i = 0; i < 3; i++)
    if (t2.isInclude(t1_arr[i]))
      return true;

  // Check intersection of segments
  for (size_t i = 0; i < 3; i++) {
    Point a = t1_arr[i], b = t1_arr[(i + 1) % 3];
    Segment<T> t1_side = Segment<T>(a, b);
    for (size_t j = 0; j < 3; j++) {
      Point c = t2_arr[j], d = t2_arr[(j + 1) % 3];
      Segment<T> t2_side = Segment<T>(c, d);
      if (intersection_2segments(t1_side, t2_side))
        return true;
    }
  }
  return false;
}

template <typename T> bool intersection_2triangles(const Triangle<T> &t1, const Triangle<T> &t2) {
  // Raw check with AABB
  if (!overlap(t1.box_, t2.box_))
    return false;

  // Main checking
  Collapsed<T> obj1 = t1.collapsed_;
  Collapsed<T> obj2 = t2.collapsed_;
  bool monostate_1 = std::holds_alternative<std::monostate>(obj1);
  bool monostate_2 = std::holds_alternative<std::monostate>(obj2);
  // Really triangles
  if (monostate_1 && monostate_2) {
    Plane<T> p1 = t1.plane_;
    Plane<T> p2 = t2.plane_;
    std::pair<PlaneToPlaneOrientation, Line<T>> inter = intersection_2planes(p1, p2);
    switch (inter.first) {
    case PlaneToPlaneOrientation::Parallel:
    case PlaneToPlaneOrientation::Invalid:
      return false;
    case PlaneToPlaneOrientation::Intersect:
      return intersection_2triangles_intersect_planes(t1, t2, inter.second);
    case PlaneToPlaneOrientation::Coincident:
      return intersection_2triangles_coincident_planes(t1, t2);
    }
  }

  // Triangle & Segment
  if (monostate_1 && std::holds_alternative<Segment<T>>(obj2)) {
    return intersection_triangle_segment(t1, std::get<Segment<T>>(obj2));
  }
  if (monostate_2 && std::holds_alternative<Segment<T>>(obj1)) {
    return intersection_triangle_segment(t2, std::get<Segment<T>>(obj1));
  }

  // Triangle & Point
  if (monostate_1 && std::holds_alternative<Point<T>>(obj2))
    return t1.isInclude(std::get<Point<T>>(obj2));
  if (monostate_2 && std::holds_alternative<Point<T>>(obj1))
    return t2.isInclude(std::get<Point<T>>(obj1));

  // Segment & Segment
  if (std::holds_alternative<Segment<T>>(obj1) && std::holds_alternative<Segment<T>>(obj2)) {
    return intersection_2segments(std::get<Segment<T>>(obj1), std::get<Segment<T>>(obj2));
  }

  // Point & Point
  if (std::holds_alternative<Point<T>>(obj1) && std::holds_alternative<Point<T>>(obj2)) {
    return equal(std::get<Point<T>>(obj1), std::get<Point<T>>(obj2));
  }

  // Segment & Point
  if (std::holds_alternative<Segment<T>>(obj1) && std::holds_alternative<Point<T>>(obj2))
    return std::get<Segment<T>>(obj1).isInclude(std::get<Point<T>>(obj2));
  if (std::holds_alternative<Segment<T>>(obj2) && std::holds_alternative<Point<T>>(obj1))
    return std::get<Segment<T>>(obj2).isInclude(std::get<Point<T>>(obj1));

  // Bad case
  return false;
}

#endif