#include <array>
#include <iostream>
#include <optional>

#include "primitives/line.hpp"
#include "primitives/plane.hpp"
#include "primitives/point.hpp"
#include "primitives/segment.hpp"
#include "triangle.hpp"

void Triangle::print(std::ostream &out) const {
  out << "a: ";
  a_.print(out);
  out << "b: ";
  b_.print(out);
  out << "c: ";
  c_.print(out);
  out << std::endl;
}

Triangle Triangle::badTriangle() { return Triangle(Point::badPoint(), Point::badPoint(), Point::badPoint()); }

bool Triangle::isBad() const { return a_.isBad() || b_.isBad() || c_.isBad(); }

Plane find_plane(const Triangle &t) {
  if (t.isBad())
    return Plane::badPlane();

  double x1 = t.getA().getX(), y1 = t.getA().getY(), z1 = t.getA().getZ();
  double x2 = t.getB().getX(), y2 = t.getB().getY(), z2 = t.getB().getZ();
  double x3 = t.getC().getX(), y3 = t.getC().getY(), z3 = t.getC().getZ();

  /// or [B-A x C-A]
  double a = (y2 - y1) * (z3 - z1) - (z2 - z1) * (y3 - y1);
  double b = (z2 - z1) * (x3 - x1) - (x2 - x1) * (z3 - z1);
  double c = (x2 - x1) * (y3 - y1) - (y2 - y1) * (x3 - x1);

  Vector n = Vector(a, b, c);
  return Plane(t.getA(), n);
}

std::optional<Segment> intersection_triangle_line(const Triangle &t, const Line &l) {
  if (t.isBad() || l.isBad())
    return std::nullopt;

  Line ab = Line(t.getA(), t.getB());
  Line ac = Line(t.getA(), t.getC());
  Line bc = Line(t.getB(), t.getC());

  size_t idx = 0;
  std::array<Point, 2> intersect_points{Point::badPoint(), Point::badPoint()};

  // Intersection line ab with l
  std::pair<LineToLineOrientation, Point> pair_intersect_ab = intersection_2lines(ab, l);
  if (pair_intersect_ab.first == LineToLineOrientation::Coincident) {
    return Segment(t.getA(), t.getB());
  }
  if (pair_intersect_ab.first == LineToLineOrientation::Intersect) {
    Point inter_ab = pair_intersect_ab.second;
    if (Segment(t.getA(), t.getB()).isInclude(inter_ab)) {
      intersect_points[idx] = inter_ab;
      idx++;
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
      intersect_points[idx] = inter_ac;
      idx++;
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
      intersect_points[idx] = inter_bc;
      idx++;
    }
  }

  // Check if there one or more intersect points
  if (idx == 0) {
    return std::nullopt;
  } else if (idx == 1) {
    return Segment(intersect_points[0], intersect_points[0]); // degenerated segment
  } else if (idx == 2) {
    return Segment(intersect_points[0], intersect_points[1]); // segment on different triangle sides
  }

  // Bad
  return std::nullopt;
}

/// This method for case intersection 2 planes
bool intersection_2triangles_intersect_planes(const Triangle &t1, const Triangle &t2) {
  Plane p1 = find_plane(t1);
  Plane p2 = find_plane(t2);
  std::pair<PlaneToPlaneOrientation, Line> inter = intersection_2planes(p1, p2);

  // This method for case intersection 2 planes
  if (inter.first != PlaneToPlaneOrientation::Intersect) {
    return false;
  }

  Line l = inter.second;
  std::optional<Segment> opt_seg1 = intersection_triangle_line(t1, l);
  std::optional<Segment> opt_seg2 = intersection_triangle_line(t2, l);
  if (!opt_seg1 || !opt_seg2)
    return false;
  Segment seg1 = opt_seg1.value();
  Segment seg2 = opt_seg2.value();
  return intersection_2segments(seg1, seg2);
}