#include <array>
#include <iostream>
#include <optional>
#include <variant>

#include "primitives/common.hpp"
#include "primitives/line.hpp"
#include "primitives/plane.hpp"
#include "primitives/point.hpp"
#include "primitives/segment.hpp"
#include "primitives/vector.hpp"
#include "triangle.hpp"

void Triangle::print(std::ostream &out) const {
  out << "a: ";
  a_.print(out);
  out << ", b: ";
  b_.print(out);
  out << ", c: ";
  c_.print(out);
  out << std::endl;
}

Triangle Triangle::badTriangle() { return Triangle(Point::badPoint(), Point::badPoint(), Point::badPoint()); }

bool Triangle::isBad() const { return a_.isBad() || b_.isBad() || c_.isBad(); }

bool Triangle::isInclude(const Point &p) const {
  Plane plane = findPlane();
  Vector n_p = plane.getN();

  // not on a plane
  const double offset = n_p * vec_from_points(a_, p);
  if (!compare_doubles(offset, 0.0))
    return false;

  double c1 = n_p * cross_product(vec_from_points(b_, a_), vec_from_points(p, a_));
  double c2 = n_p * cross_product(vec_from_points(c_, b_), vec_from_points(p, b_));
  double c3 = n_p * cross_product(vec_from_points(a_, c_), vec_from_points(p, c_));

  // Treat "nonnegative"
  // Treat "nonpositive"
  auto geq0 = [&](double x) { return (x > 0.0) || compare_doubles(x, 0.0); };
  auto leq0 = [&](double x) { return (x < 0.0) || compare_doubles(x, 0.0); };

  bool nonNeg = geq0(c1) && geq0(c2) && geq0(c3);
  bool nonPos = leq0(c1) && leq0(c2) && leq0(c3);
  return nonNeg || nonPos;
}

bool Triangle::isDegenerate() const {
  // Equal points
  if (equal(a_, b_) || equal(a_, c_) || equal(c_, b_))
    return true;
  // Area = 0
  Vector ab = vec_from_points(a_, b_);
  Vector ac = vec_from_points(a_, c_);
  Vector n = cross_product(ab, ac);
  return compare_doubles(n * n, 0);
}

Collapsed Triangle::collapsedTriangle() const {
  // 1) Check degeneracy by area ~ ||(B-A) x (C-A)||^2
  Vector AB = vec_from_points(a_, b_);
  Vector AC = vec_from_points(a_, c_);
  Vector n = cross_product(AB, AC);
  double n2 = n * n;

  // Proper (non-degenerate) triangle -> not collapsed
  if (!compare_doubles(n2, 0.0))
    return std::monostate{};

  // 2) Degenerate: choose farthest pair among the three vertices
  auto d2 = [&](const Point &p, const Point &q) {
    return norm2(vec_from_points(p, q)); // squared distance
  };

  double dab = d2(a_, b_);
  double dac = d2(a_, c_);
  double dbc = d2(b_, c_);

  const Point *p = &a_;
  const Point *q = &b_;
  double dmax = dab;

  if (dac >= dmax) {
    p = &a_;
    q = &c_;
    dmax = dac;
  }
  if (dbc >= dmax) {
    p = &b_;
    q = &c_;
    dmax = dbc;
  }

  // 3) If even the longest edge is ~0, all points coincide -> point
  if (compare_doubles(dmax, 0.0))
    return Point(*p);

  // 4) Otherwise collapsed triangle is that longest edge
  return Segment(*p, *q);
}

Plane Triangle::findPlane() const {
  if (this->isBad())
    return Plane::badPlane();

  double x1 = a_.getX(), y1 = a_.getY(), z1 = a_.getZ();
  double x2 = b_.getX(), y2 = b_.getY(), z2 = b_.getZ();
  double x3 = c_.getX(), y3 = c_.getY(), z3 = c_.getZ();

  /// or [B-A x C-A]
  double a = (y2 - y1) * (z3 - z1) - (z2 - z1) * (y3 - y1);
  double b = (z2 - z1) * (x3 - x1) - (x2 - x1) * (z3 - z1);
  double c = (x2 - x1) * (y3 - y1) - (y2 - y1) * (x3 - x1);

  Vector n = Vector(a, b, c);
  return Plane(a_, n);
}

std::optional<Segment> intersection_triangle_line_on_plane(const Triangle &t, const Line &l) {
  if (t.isBad() || l.isBad())
    return std::nullopt;

  Line ab = Line(t.getA(), t.getB());
  Line ac = Line(t.getA(), t.getC());
  Line bc = Line(t.getB(), t.getC());

  size_t idx = 0;
  std::array<Point, 2> intersect_points{Point::badPoint(), Point::badPoint()};

  auto push_unique = [&](const Point &q) {
    // dedup against already collected points
    for (size_t i = 0; i < idx; ++i) {
      if (equal(intersect_points[i], q))
        return; // already have it
    }
    if (idx < 2) {
      intersect_points[idx++] = q;
    }
  };

  // Intersection line ab with l
  std::pair<LineToLineOrientation, Point> pair_intersect_ab = intersection_2lines(ab, l);
  if (pair_intersect_ab.first == LineToLineOrientation::Coincident) {
    return Segment(t.getA(), t.getB());
  }
  if (pair_intersect_ab.first == LineToLineOrientation::Intersect) {
    Point inter_ab = pair_intersect_ab.second;
    if (Segment(t.getA(), t.getB()).isInclude(inter_ab)) {
      push_unique(inter_ab);
      if (idx == 2)
        return Segment(intersect_points[0], intersect_points[1]);
    }
  }

  // Intersection line ac with l
  std::pair<LineToLineOrientation, Point> pair_intersect_ac = intersection_2lines(ac, l);
  if (pair_intersect_ac.first == LineToLineOrientation::Coincident) {
    return Segment(t.getA(), t.getC());
  }
  if (pair_intersect_ac.first == LineToLineOrientation::Intersect) {
    Point inter_ac = pair_intersect_ac.second;
    if (Segment(t.getA(), t.getC()).isInclude(inter_ac)) {
      push_unique(inter_ac);
      if (idx == 2)
        return Segment(intersect_points[0], intersect_points[1]);
    }
  }

  // Intersection line bc with l
  std::pair<LineToLineOrientation, Point> pair_intersect_bc = intersection_2lines(bc, l);
  if (pair_intersect_bc.first == LineToLineOrientation::Coincident) {
    return Segment(t.getB(), t.getC());
  }
  if (pair_intersect_bc.first == LineToLineOrientation::Intersect) {
    Point inter_bc = pair_intersect_bc.second;
    if (Segment(t.getB(), t.getC()).isInclude(inter_bc)) {
      push_unique(inter_bc);
      if (idx == 2)
        return Segment(intersect_points[0], intersect_points[1]);
    }
  }

  // Check if there one or more intersect points
  if (idx == 0)
    return std::nullopt;
  if (idx == 1)
    return Segment(intersect_points[0], intersect_points[0]); // degenerated segment
  if (idx == 2)
    return Segment(intersect_points[0], intersect_points[1]); // degenerated segment

  // Bad
  return std::nullopt;
}

/// This method for case intersection 2 planes
bool intersection_2triangles_intersect_planes(const Triangle &t1, const Triangle &t2) {
  Plane p1 = t1.findPlane();
  Plane p2 = t2.findPlane();
  std::pair<PlaneToPlaneOrientation, Line> inter = intersection_2planes(p1, p2);
  // This method for case intersection 2 planes
  if (inter.first != PlaneToPlaneOrientation::Intersect) {
    return false;
  }

  Line l = inter.second;
  std::optional<Segment> opt_seg1 = intersection_triangle_line_on_plane(t1, l);
  std::optional<Segment> opt_seg2 = intersection_triangle_line_on_plane(t2, l);
  if (!opt_seg1 || !opt_seg2)
    return false;
  Segment seg1 = opt_seg1.value();
  Segment seg2 = opt_seg2.value();
  return intersection_2segments_on_line(seg1, seg2);
}

/// This method for case 2 triangles on one plane
bool intersection_2triangles_coincident_planes(const Triangle &t1, const Triangle &t2) {
  Plane p1 = t1.findPlane();
  Plane p2 = t2.findPlane();
  std::pair<PlaneToPlaneOrientation, Line> inter = intersection_2planes(p1, p2);
  // This method for case 2 triangle
  if (inter.first != PlaneToPlaneOrientation::Coincident) {
    return false;
  }

  std::array<Point, 3> t1_arr = {t1.getA(), t1.getB(), t1.getC()};
  std::array<Point, 3> t2_arr = {t2.getA(), t2.getB(), t2.getC()};

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
    Segment t1_side = Segment(a, b);
    for (size_t j = 0; j < 3; j++) {
      Point c = t2_arr[j], d = t2_arr[(j + 1) % 3];
      Segment t2_side = Segment(c, d);
      if (intersection_2segments(t1_side, t2_side))
        return true;
    }
  }
  return false;
}

bool intersection_2triangles(const Triangle &t1, const Triangle &t2) {
  Collapsed obj1 = t1.collapsedTriangle();
  Collapsed obj2 = t2.collapsedTriangle();
  // Really triangles
  if (std::holds_alternative<std::monostate>(obj1) && std::holds_alternative<std::monostate>(obj2)) {
    Plane p1 = t1.findPlane();
    Plane p2 = t2.findPlane();
    std::pair<PlaneToPlaneOrientation, Line> inter = intersection_2planes(p1, p2);
    if ((inter.first == PlaneToPlaneOrientation::Parallel) || (inter.first == PlaneToPlaneOrientation::Invalid))
      return false;

    if (inter.first == PlaneToPlaneOrientation::Intersect) {
      return intersection_2triangles_intersect_planes(t1, t2);
    }

    if (inter.first == PlaneToPlaneOrientation::Coincident) {
      return intersection_2triangles_coincident_planes(t1, t2);
    }
  }

  // Triangle & Segment
  if (std::holds_alternative<std::monostate>(obj1) && std::holds_alternative<Segment>(obj2)) {
    std::get<Segment>(obj2).print(std::cout);
    return intersection_triangle_segment(t1, std::get<Segment>(obj2));
  }
  if (std::holds_alternative<std::monostate>(obj2) && std::holds_alternative<Segment>(obj1)) {
    return intersection_triangle_segment(t2, std::get<Segment>(obj1));
  }

  // Triangle & Point
  if (std::holds_alternative<std::monostate>(obj1) && std::holds_alternative<Point>(obj2))
    return t1.isInclude(std::get<Point>(obj2));
  if (std::holds_alternative<std::monostate>(obj2) && std::holds_alternative<Point>(obj1))
    return t2.isInclude(std::get<Point>(obj1));

  // Segment & Segment
  if (std::holds_alternative<Segment>(obj1) && std::holds_alternative<Segment>(obj2)) {
    return intersection_2segments(std::get<Segment>(obj1), std::get<Segment>(obj2));
  }

  // Point & Point
  if (std::holds_alternative<Point>(obj1) && std::holds_alternative<Point>(obj2)) {
    return equal(std::get<Point>(obj1), std::get<Point>(obj2));
  }

  // Segment & Point
  if (std::holds_alternative<Segment>(obj1) && std::holds_alternative<Point>(obj2))
    return std::get<Segment>(obj1).isInclude(std::get<Point>(obj2));
  if (std::holds_alternative<Segment>(obj2) && std::holds_alternative<Point>(obj1))
    return std::get<Segment>(obj2).isInclude(std::get<Point>(obj1));

  // Bad case
  return false;
}

std::optional<Segment> intersection_triangle_line(const Triangle &t, const Line &l) {
  if (t.isBad() || l.isBad())
    return std::nullopt;

  Plane pt = t.findPlane();
  Vector n = pt.getN();
  double s = pt.getS();

  Point p0 = l.getP(); // any point on the line
  Vector v = l.getD(); // line direction (non-zero)

  // n·(p0 + t v) = s  ->  t = (s - n·p0) / (n·v)
  double nv = n * v; // dot
  double np0 = n * Vector(p0.getX(), p0.getY(), p0.getZ());
  if (compare_doubles(nv, 0.0)) {
    // Line is parallel to plane
    if (compare_doubles(np0 - s, 0.0)) {
      // Complanar case, on one plane
      std::optional<Segment> result = intersection_triangle_line_on_plane(t, l);
      if (result)
        return result.value();
      return std::nullopt;
    }
    // in parallel planes, no intersection
    return std::nullopt;
  }
  double tparam = (s - np0) / nv;
  Point x = (p0.toVector() + v * tparam).toPoint(); // point of line-plane intersection
  if (t.isInclude(x))
    return Segment(x, x);
  return std::nullopt;
}

bool intersection_triangle_segment(const Triangle &t, const Segment &s) {
  // Degenerate segment: reduce to point-in-triangle
  if (s.isDegenerate()) {
    return t.isInclude(s.getA());
  }

  Line l = s.findLine();
  l.print(std::cout);
  std::optional<Segment> opt_res = intersection_triangle_line(t, l);
  if (!opt_res)
    return false;

  Segment inter_line = *opt_res;
  return intersection_2segments_on_line(inter_line, s);
}