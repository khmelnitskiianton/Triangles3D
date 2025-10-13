#include <gmock/gmock.h>
#include <gtest/gtest.h>
#include <optional>

#include "primitives/line.hpp"
#include "primitives/plane.hpp"
#include "primitives/point.hpp"
#include "primitives/segment.hpp"
#include "primitives/vector.hpp"
#include "triangle.hpp"

//=================================================================================================
// LINE TESTS

TEST(LineTests, equalTest) {
  bool res1 = equal(Line(Point(0, -1, 0), Point(-2, 0, 0)), Line(Point(-6, 2, 0), Point(-4, 1, 0)));
  ASSERT_EQ(res1, true);
}

TEST(LineTests, intersection_2linesTest) {
  // 1
  {
    SCOPED_TRACE("case 1");
    Line l1 = Line(Point(-2, 0, 0), Point(0, 1, 0));
    Line l2 = Line(Point(1, 0, 0), Point(0, -2, 0));
    std::pair<LineToLineOrientation, Point> result = intersection_2lines(l1, l2);
    Line prob_inter = Line(Point(-6, 2, 0), Point(-4, 1, 0));
    ASSERT_EQ(result.first, LineToLineOrientation::Intersect);
    ASSERT_TRUE(equal(result.second, Point(2, 2, 0)));
  }
  // 2
  {
    SCOPED_TRACE("case 2");
    Line l1 = Line(Point(-2, 0, 0), Point(0, 1, 0));
    Line l2 = Line(Point(4, 0, 0), Point(0, -2, 0));
    std::pair<LineToLineOrientation, Point> result = intersection_2lines(l1, l2);
    ASSERT_EQ(result.first, LineToLineOrientation::Parallel);
    ASSERT_TRUE(result.second.isBad());
  }
  // 3
  {
    SCOPED_TRACE("case 3");
    Line l1 = Line(Point(-2, 0, 0), Point(0, 1, 0));
    Line l2 = Line(Point(4, 3, 0), Point(2, 2, 0));
    std::pair<LineToLineOrientation, Point> result = intersection_2lines(l1, l2);
    ASSERT_EQ(result.first, LineToLineOrientation::Coincident);
    ASSERT_TRUE(result.second.isBad());
  }

  // 4
  {
    SCOPED_TRACE("case 4");
    Line l1 = Line(Point(-2, 0, 0), Point(0, 1, 0));
    Line l2 = Line(Point(1, 0, 1), Point(0, -2, 1));
    std::pair<LineToLineOrientation, Point> result = intersection_2lines(l1, l2);
    ASSERT_EQ(result.first, LineToLineOrientation::Skew);
    ASSERT_TRUE(result.second.isBad());
  }

  // 5
  {
    SCOPED_TRACE("case 5");
    Line l1 = Line(Point(-2, 0, 0), Point(0, 1, 0));
    Line l2 = Line::badLine();
    std::pair<LineToLineOrientation, Point> result = intersection_2lines(l1, l2);
    ASSERT_EQ(result.first, LineToLineOrientation::Invalid);
    ASSERT_TRUE(result.second.isBad());
  }
}

//=================================================================================================
// PLANE TESTS

TEST(PlaneTests, pointPositionTest) {
  Plane p1 = Plane(Point(-2, 0, 0), Point(0, 0, 1), Point(0, -1, 0));
  ASSERT_EQ(p1.pointPosition(Point(1, 0, 0)), PlaneSide::PositiveHalfSpace);
  ASSERT_EQ(p1.pointPosition(Point(1, 0, 1.5)), PlaneSide::BelongsToPlane);
  ASSERT_EQ(p1.pointPosition(Point(-4, 1, 0)), PlaneSide::BelongsToPlane);
  ASSERT_EQ(p1.pointPosition(Point(-3, 0, 0)), PlaneSide::NegativeHalfSpace);
}

TEST(PlaneTests, intersection_2planesTest) {
  // 1
  {
    SCOPED_TRACE("case 1");
    Plane p1 = Plane(Point(-2, 0, 0), Point(0, 0, 1), Point(0, -1, 0));
    Plane p2 = Plane(Point(0, 0, 0), Point(0, 1, 0), Point(-1, 0, 0));
    std::pair<PlaneToPlaneOrientation, Line> result = intersection_2planes(p1, p2);
    Line prob_inter = Line(Point(-6, 2, 0), Point(-4, 1, 0));
    ASSERT_EQ(result.first, PlaneToPlaneOrientation::Intersect);
    ASSERT_TRUE(equal(result.second, prob_inter));
  }
  // 2
  {
    SCOPED_TRACE("case 2");
    Plane p1 = Plane(Point(0, 0, 0), Point(0, 1, 0), Point(-1, 0, 0));
    Plane p2 = Plane(Point(2, 0, 1), Point(-2, 1, 1), Point(-1, 0, 1));
    std::pair<PlaneToPlaneOrientation, Line> result = intersection_2planes(p1, p2);
    Line prob_inter = Line(Point(-6, 2, 0), Point(-4, 1, 0));
    ASSERT_EQ(result.first, PlaneToPlaneOrientation::Parallel);
    ASSERT_TRUE(result.second.isBad());
  }

  // 3
  {
    SCOPED_TRACE("case 3");
    Plane p1 = Plane(Point(0, 0, 0), Point(0, 1, 0), Point(-1, 0, 0));
    Plane p2 = Plane(Point(2, 0, 0), Point(-2, 1, 0), Point(-1, 0, 0));
    std::pair<PlaneToPlaneOrientation, Line> result = intersection_2planes(p1, p2);
    Line prob_inter = Line(Point(-6, 2, 0), Point(-4, 1, 0));
    ASSERT_EQ(result.first, PlaneToPlaneOrientation::Coincident);
    ASSERT_TRUE(result.second.isBad());
  }
}

TEST(PlaneTests, BadZeroTest) {
  Plane bad_plane1 = Plane(Point(NAN, NAN, NAN), Point(NAN, NAN, NAN), Point(NAN, NAN, NAN));
  Plane bad_plane2 = Plane(Point(NAN, NAN, NAN), Vector(4, 3, 2));
  Plane bad_plane3 = Plane(Point(4, 3, 2), Vector(4, 3, NAN));

  Plane degenerate_plane1 = Plane(Point(3, 2, 4), Vector(0, 0, 0));

  ASSERT_TRUE(bad_plane1.isBad());
  ASSERT_TRUE(bad_plane2.isBad());
  ASSERT_TRUE(bad_plane3.isBad());
  ASSERT_TRUE(degenerate_plane1.isDegenerate());
}

//=================================================================================================
// Segment Tests
TEST(SegmentTests, equalTest) {
  // 1
  {
    SCOPED_TRACE("case 1");
    // On one line
    Segment s1 = Segment(Point(-2, 0, 0), Point(2, 2, 0));
    Segment s2 = Segment(Point(2, 2, 0), Point(-2, 0, 0));
    bool result = equal(s1, s2);
    ASSERT_EQ(result, true);
  }
  // 2
  {
    SCOPED_TRACE("case 2");
    // On one line
    Segment s1 = Segment(Point(-2, 0, 0), Point(2, 2, 0));
    Segment s2 = Segment(Point(-2, 0, 0), Point(2, 2, 0));
    bool result = equal(s1, s2);
    ASSERT_EQ(result, true);
  }

  // 3
  {
    SCOPED_TRACE("case 3");
    // On one line
    Segment s1 = Segment(Point(-2, 0, 0), Point(2, 2, 0));
    Segment s2 = Segment(Point(-2, 0, 1), Point(2, 2, 1));
    bool result = equal(s1, s2);
    ASSERT_EQ(result, false);
  }
}

TEST(SegmentTests, intersection_2segments_on_lineTest) {
  // 1
  {
    SCOPED_TRACE("case 1");
    // On one line
    Segment s1 = Segment(Point(-2, 0, 0), Point(2, 2, 0));
    Segment s2 = Segment(Point(0, 1, 0), Point(-4, -1, 0));
    bool result = intersection_2segments_on_line(s1, s2);
    ASSERT_EQ(result, true);
  }

  // 2
  {
    SCOPED_TRACE("case 2");
    // On one line
    Segment s1 = Segment(Point(-2, 0, 0), Point(2, 2, 0));
    Segment s2 = Segment(Point(2, 2, 0), Point(-4, -1, 0));
    bool result = intersection_2segments_on_line(s1, s2);
    ASSERT_EQ(result, true);
  }

  // 3
  {
    SCOPED_TRACE("case 3");
    // On one line
    Segment s1 = Segment(Point(-2, 0, 0), Point(-4, -1, 0));
    Segment s2 = Segment(Point(2, 2, 0), Point(-2, 0, 0));
    bool result = intersection_2segments_on_line(s1, s2);
    ASSERT_EQ(result, true);
  }

  // 4
  {
    SCOPED_TRACE("case 4");
    // On one line
    Segment s1 = Segment(Point(-2, 0, 0), Point(-4, -1, 0));
    Segment s2 = Segment(Point(2, 2, 0), Point(0, 1, 0));
    bool result = intersection_2segments_on_line(s1, s2);
    ASSERT_EQ(result, false);
  }
}

TEST(SegmentTests, intersection_2segmentsTest) {
  // 1
  {
    SCOPED_TRACE("case 1");
    Segment s1 = Segment(Point(-2, 0, 0), Point(0, 1, 0));
    Segment s2 = Segment(Point(-1, 0, 0), Point(0, -1, 0));
    bool result = intersection_2segments(s1, s2);
    ASSERT_EQ(result, false);
  }

  // 2
  {
    SCOPED_TRACE("case 2");
    Segment s1 = Segment(Point(-1, -2, 0), Point(0, 1, 0));
    Segment s2 = Segment(Point(-1, 0, 0), Point(0, -1, 0));
    bool result = intersection_2segments(s1, s2);
    ASSERT_EQ(result, true);
  }

  // 3
  {
    SCOPED_TRACE("case 3");
    Segment s1 = Segment(Point(-0.5, -0.5, 0), Point(0, 0, 1));
    Segment s2 = Segment(Point(-1, 0, 0), Point(0, -1, 0));
    bool result = intersection_2segments(s1, s2);
    ASSERT_EQ(result, true);
  }

  // 4
  {
    SCOPED_TRACE("case 4");
    Segment s1 = Segment(Point(-2, 0, 0), Point(-2, 0, 0));
    Segment s2 = Segment(Point(-1, 0, 0), Point(0, -1, 0));
    bool result = intersection_2segments(s1, s2);
    ASSERT_EQ(result, false);
  }

  // 5
  {
    SCOPED_TRACE("case 5");
    Segment s1 = Segment(Point(-0.5, -0.5, 0), Point(-0.5, -0.5, 0));
    Segment s2 = Segment(Point(-1, 0, 0), Point(0, -1, 0));
    bool result = intersection_2segments(s1, s2);
    ASSERT_EQ(result, true);
  }

  // 6
  {
    SCOPED_TRACE("case 6");
    Segment s1 = Segment(Point(-2, 1, 0), Point(-2, 1, 0));
    Segment s2 = Segment(Point(-1, 0, 0), Point(0, -1, 0));
    bool result = intersection_2segments(s1, s2);
    ASSERT_EQ(result, false);
  }
}

//=================================================================================================
// Triangle Tests

TEST(TriangleTests, isIncludeTest) {
  Triangle t = Triangle(Point(0, 0, 0), Point(0, 1, 0), Point(-2, 0, 0));
  // 1
  {
    SCOPED_TRACE("case 1");
    Point p = Point(0, 0, 0);
    bool result = t.isInclude(p);
    ASSERT_EQ(result, true);
  }

  // 2
  {
    SCOPED_TRACE("case 2");
    Point p = Point(1, 0, 0);
    bool result = t.isInclude(p);
    ASSERT_EQ(result, false);
  }

  // 3
  {
    SCOPED_TRACE("case 3");
    Point p = Point(-0.5, 0, 0);
    bool result = t.isInclude(p);
    ASSERT_EQ(result, true);
  }

  // 4
  {
    SCOPED_TRACE("case 4");
    Point p = Point(-0.1, 0.1, 0);
    bool result = t.isInclude(p);
    ASSERT_EQ(result, true);
  }
}

TEST(TriangleTests, collapseTest) {
  // 1
  {
    SCOPED_TRACE("case 1");
    Triangle t = Triangle(Point(0, 0, 0), Point(0, 1, 0), Point(-2, 0, 0));
    Collapsed res = t.collapsedTriangle();
    ASSERT_EQ(std::holds_alternative<std::monostate>(res), true);
  }

  // 2
  {
    SCOPED_TRACE("case 2");
    Triangle t = Triangle(Point(0, 0, 0), Point(0, 1, 0), Point(-2, 0, 1));
    Collapsed res = t.collapsedTriangle();
    ASSERT_EQ(std::holds_alternative<std::monostate>(res), true);
  }

  // 3
  {
    SCOPED_TRACE("case 3");
    Triangle t = Triangle(Point(0, 0, 0), Point(0, 0, 0), Point(0, 0, 0));
    Collapsed res = t.collapsedTriangle();
    ASSERT_EQ(std::holds_alternative<Point>(res), true);
    ASSERT_EQ(equal(std::get<Point>(res), Point(0, 0, 0)), true);
  }

  // 4
  {
    SCOPED_TRACE("case 4");
    Triangle t = Triangle(Point(1, 0, -1), Point(1, 0, -1), Point(1, 0, -1));
    Collapsed res = t.collapsedTriangle();
    ASSERT_EQ(std::holds_alternative<Point>(res), true);
    ASSERT_EQ(equal(std::get<Point>(res), Point(1, 0, -1)), true);
  }

  // 5
  {
    SCOPED_TRACE("case 5");
    Triangle t = Triangle(Point(1, 0, 0), Point(-1, 0, 0), Point(0, 0, 0));
    Collapsed res = t.collapsedTriangle();
    ASSERT_EQ(std::holds_alternative<Segment>(res), true);
    ASSERT_EQ(equal(std::get<Segment>(res), Segment(Point(1, 0, 0), Point(-1, 0, 0))), true);
  }

  // 6
  {
    SCOPED_TRACE("case 6");
    Triangle t = Triangle(Point(1, 0, 0), Point(-1, 0, 0), Point(0, 0, 0));
    Collapsed res = t.collapsedTriangle();
    ASSERT_EQ(std::holds_alternative<Segment>(res), true);
    ASSERT_EQ(equal(std::get<Segment>(res), Segment(Point(1, 0, 0), Point(-1, 0, 0))), true);
  }

  // 7
  {
    SCOPED_TRACE("case 7");
    Triangle t = Triangle(Point(-1, 1, 0), Point(0, 0, 0), Point(-2, 2, 0));
    Collapsed res = t.collapsedTriangle();
    ASSERT_EQ(std::holds_alternative<Segment>(res), true);
    ASSERT_EQ(equal(std::get<Segment>(res), Segment(Point(0, 0, 0), Point(-2, 2, 0))), true);
  }

  // 8
  {
    SCOPED_TRACE("case 8");
    Triangle t = Triangle(Point(-1, 1, 0), Point(0, 0, 0), Point(-2, 2, 0));
    Collapsed res = t.collapsedTriangle();
    ASSERT_EQ(std::holds_alternative<Segment>(res), true);
    ASSERT_EQ(equal(std::get<Segment>(res), Segment(Point(-2, 2, 0), Point(0, 0, 0))), true);
  }

  // 9
  {
    SCOPED_TRACE("case 9");
    Triangle t = Triangle(Point(-1, 1, 0), Point(0, 0, 1), Point(1, -1, 2));
    Collapsed res = t.collapsedTriangle();
    ASSERT_EQ(std::holds_alternative<Segment>(res), true);
    ASSERT_EQ(equal(std::get<Segment>(res), Segment(Point(1, -1, 2), Point(-1, 1, 0))), true);
  }

  // 10
  {
    SCOPED_TRACE("case 10");
    Triangle t = Triangle(Point(-1, 1, 0), Point(-1, 1, 0), Point(1, -1, 2));
    Collapsed res = t.collapsedTriangle();
    ASSERT_EQ(std::holds_alternative<Segment>(res), true);
    ASSERT_EQ(equal(std::get<Segment>(res), Segment(Point(1, -1, 2), Point(-1, 1, 0))), true);
  }

  // 11
  {
    SCOPED_TRACE("case 11");
    Triangle t = Triangle(Point(1, -1, 2), Point(-1, 1, 0), Point(1, -1, 2));
    Collapsed res = t.collapsedTriangle();
    ASSERT_EQ(std::holds_alternative<Segment>(res), true);
    ASSERT_EQ(equal(std::get<Segment>(res), Segment(Point(1, -1, 2), Point(-1, 1, 0))), true);
  }
}

TEST(TriangleTests, intersection_line_triangleTest) {
  // 1
  {
    SCOPED_TRACE("case 1");

    Triangle t = Triangle(Point(0, 0, 0), Point(0, 1, 0), Point(-2, 0, 0));
    Line l = Line(Point(1, 0, 0), Point(0, -1, 0));
    std::optional<Segment> result = intersection_triangle_line_on_plane(t, l);
    ASSERT_EQ(result, std::nullopt);
  }

  // 2
  {
    SCOPED_TRACE("case 2");
    Triangle t = Triangle(Point(0, 0, 0), Point(0, 1, 0), Point(-2, 0, 0));
    Line l = Line(Point(0, 1, 0), Point(0, 0, 0));
    std::optional<Segment> result = intersection_triangle_line_on_plane(t, l);
    ASSERT_NE(result, std::nullopt);
    ASSERT_EQ(equal(*result, Segment(Point(0, 1, 0), Point(0, 0, 0))), true);
  }

  // 3
  {
    SCOPED_TRACE("case 3");
    Triangle t = Triangle(Point(0, 0, 0), Point(0, 1, 0), Point(-2, 0, 0));
    Line l = Line(Point(-1, 0, 0), Vector(0, 3, 0));
    std::optional<Segment> result = intersection_triangle_line_on_plane(t, l);
    ASSERT_NE(result, std::nullopt);
    ASSERT_EQ(equal(*result, Segment(Point(-1, 0.5, 0), Point(-1, 0, 0))), true);
  }

  // 4
  {
    SCOPED_TRACE("case 4");
    Triangle t = Triangle(Point(0, 0, 0), Point(0, 1, 0), Point(-2, 0, 0));
    Line l = Line(Point(1, 1, 0), Vector(2, 2, 0));
    std::optional<Segment> result = intersection_triangle_line_on_plane(t, l);
    ASSERT_NE(result, std::nullopt);
    ASSERT_EQ(result->isDegenerate(), true);
  }
}

TEST(TriangleTests, intersection_2triangles_intersect_planesTest) {
  // 1
  {
    SCOPED_TRACE("case 1");
    Triangle t1 = Triangle(Point(0, 0, 0), Point(0, 1, 0), Point(-2, 0, 0));
    Triangle t2 = Triangle(Point(0, 0, 1), Point(0, 1, 0), Point(-2, 0, 0));
    bool result = intersection_2triangles_intersect_planes(t1, t2);
    ASSERT_EQ(result, true);
  }

  // 2
  {
    SCOPED_TRACE("case 2");
    Triangle t1 = Triangle(Point(-3, 0, 0), Point(0, -1, 0), Point(0, 0, 1));
    Triangle t2 = Triangle(Point(0, 0, 0), Point(0, 1, 0), Point(-2, 0, 0));
    bool result = intersection_2triangles_intersect_planes(t1, t2);
    ASSERT_EQ(result, false);
  }

  // 3
  {
    SCOPED_TRACE("case 3");
    Triangle t1 = Triangle(Point(0, 0, 0), Point(-3, 0, 0), Point(0, 0, 2));
    Triangle t2 = Triangle(Point(0, 0, 0), Point(0, 1, 0), Point(-2, 0, 0));
    bool result = intersection_2triangles_intersect_planes(t1, t2);
    ASSERT_EQ(result, true);
  }

  // 4
  {
    SCOPED_TRACE("case 4");
    Triangle t1 = Triangle(Point(1, 0, 0), Point(-3, 0, 0), Point(0, 0, 2));
    Triangle t2 = Triangle(Point(0, 0, 0), Point(0, 1, 0), Point(-2, 0, 0));
    bool result = intersection_2triangles_intersect_planes(t1, t2);
    ASSERT_EQ(result, true);
  }

  // 5
  {
    SCOPED_TRACE("case 5");
    Triangle t1 = Triangle(Point(-2, 0, 0), Point(-3, 0, 0), Point(0, 0, 2));
    Triangle t2 = Triangle(Point(0, 0, 0), Point(0, 1, 0), Point(-2, 0, 0));
    bool result = intersection_2triangles_intersect_planes(t1, t2);
    ASSERT_EQ(result, true);
  }

  // 6
  {
    SCOPED_TRACE("case 6");
    Triangle t1 = Triangle(Point(-3, 0, 0), Point(-4, 0, 0), Point(0, 0, 2));
    Triangle t2 = Triangle(Point(0, 0, 0), Point(0, 1, 0), Point(-2, 0, 0));
    bool result = intersection_2triangles_intersect_planes(t1, t2);
    ASSERT_EQ(result, false);
  }

  // 7
  {
    SCOPED_TRACE("case 7");
    Triangle t1 = Triangle(Point(-4, 0, 0), Point(0, 2, 0), Point(0, 0, 2));
    Triangle t2 = Triangle(Point(0, 0, 0), Point(0, 1, 0), Point(-2, 0, 0));
    bool result = intersection_2triangles_intersect_planes(t1, t2);
    ASSERT_EQ(result, false);
  }

  // 8
  {
    SCOPED_TRACE("case 8");
    Triangle t1 = Triangle(Point(2, 1, 0), Point(-2, -1, 0), Point(0, 0, 2));
    Triangle t2 = Triangle(Point(0, 0, 0), Point(0, 1, 0), Point(-2, 0, 0));
    bool result = intersection_2triangles_intersect_planes(t1, t2);
    ASSERT_EQ(result, true);
  }

  // 9
  {
    SCOPED_TRACE("case 9");
    Triangle t1 = Triangle(Point(1, 1, 0), Point(-1, 1, 0), Point(0, 0, 0));
    Triangle t2 = Triangle(Point(0, 0, 0), Point(0, 0, 2), Point(2, 0, 2));
    bool result = intersection_2triangles_intersect_planes(t1, t2);
    ASSERT_EQ(result, true);
  }

  // 10
  {
    SCOPED_TRACE("case 10");
    Triangle t1 = Triangle(Point(1, 1, 0), Point(-1, 1, 0), Point(0, 0, 0));
    Triangle t2 = Triangle(Point(-1, 0, 0), Point(0, 0, 2), Point(2, 0, 2));
    bool result = intersection_2triangles_intersect_planes(t1, t2);
    ASSERT_EQ(result, false);
  }

  // 11
  {
    SCOPED_TRACE("case 11");
    Triangle t1 = Triangle(Point(1, 1, 0), Point(-1, 1, 0), Point(0, 0, 0));
    Triangle t2 = Triangle(Point(0, 1, 0), Point(0, 0, 2), Point(2, 0, 2));
    bool result = intersection_2triangles_intersect_planes(t1, t2);
    ASSERT_EQ(result, true);
  }

  // 12
  {
    SCOPED_TRACE("case 12");
    Triangle t1 = Triangle(Point(1, 1, 0), Point(-1, 1, 0), Point(0, 0, 0));
    Triangle t2 = Triangle(Point(2, 1, 0), Point(0, 0, 2), Point(2, 0, 2));
    bool result = intersection_2triangles_intersect_planes(t1, t2);
    ASSERT_EQ(result, false);
  }
}

TEST(TriangleTests, intersection_2triangles_coincident_planesTest) {
  // 1
  {
    SCOPED_TRACE("case 1");
    Triangle t1 = Triangle(Point(0, 0, 0), Point(-1, 0, 0), Point(0, -1, 0));
    Triangle t2 = Triangle(Point(-2, 0, 0), Point(0, 1, 0), Point(0, 2, 0));
    bool result = intersection_2triangles_coincident_planes(t1, t2);
    ASSERT_EQ(result, false);
  }

  // 2
  {
    SCOPED_TRACE("case 2");
    Triangle t1 = Triangle(Point(0, 0, 0), Point(-1, 0, 0), Point(0, -1, 0));
    Triangle t2 = Triangle(Point(-1, 0, 0), Point(0, 1, 0), Point(0, 2, 0));
    bool result = intersection_2triangles_coincident_planes(t1, t2);
    ASSERT_EQ(result, true);
  }

  // 3
  {
    SCOPED_TRACE("case 3");
    Triangle t1 = Triangle(Point(0, 0, 0), Point(-1, 0, 0), Point(0, -1, 0));
    Triangle t2 = Triangle(Point(-0.5, 0, 0), Point(0, 1, 0), Point(0, 2, 0));
    bool result = intersection_2triangles_coincident_planes(t1, t2);
    ASSERT_EQ(result, true);
  }

  // 4
  {
    SCOPED_TRACE("case 4");
    Triangle t1 = Triangle(Point(0, 0, 0), Point(-1, 0, 0), Point(0, -1, 0));
    Triangle t2 = Triangle(Point(-3, 0, 0), Point(0, -1.5, 0), Point(0, -0.5, 0));
    bool result = intersection_2triangles_coincident_planes(t1, t2);
    ASSERT_EQ(result, true);
  }

  // 5
  {
    SCOPED_TRACE("case 5");
    Triangle t1 = Triangle(Point(0, 0, 0), Point(-2, 0, 0), Point(0, -2, 0));
    Triangle t2 = Triangle(Point(-2, -2, 0), Point(0, 1, 0), Point(1, 0, 0));
    bool result = intersection_2triangles_coincident_planes(t1, t2);
    ASSERT_EQ(result, true);
  }

  // 6
  {
    SCOPED_TRACE("case 6");
    Triangle t1 = Triangle(Point(0, 0, 0), Point(-2, 0, 0), Point(0, -2, 0));
    Triangle t2 = Triangle(Point(-3, 0, 0), Point(0, -3, 0), Point(1, 1, 0));
    bool result = intersection_2triangles_coincident_planes(t1, t2);
    ASSERT_EQ(result, true);
  }

  // 7
  {
    SCOPED_TRACE("case 7");
    Triangle t1 = Triangle(Point(0, 0, 0), Point(-2, 0, 0), Point(0, -2, 0));
    Triangle t2 = Triangle(Point(-3, 0, 0), Point(0, -3, 0), Point(-1, -1, 0));
    bool result = intersection_2triangles_coincident_planes(t1, t2);
    ASSERT_EQ(result, true);
  }

  // 8
  {
    SCOPED_TRACE("case 8");
    Triangle t1 = Triangle(Point(0, 0, 0), Point(-2, 0, 0), Point(0, -2, 0));
    Triangle t2 = Triangle(Point(-3, 0, 0), Point(0, -3, 0), Point(-2, -2, 0));
    bool result = intersection_2triangles_coincident_planes(t1, t2);
    ASSERT_EQ(result, false);
  }
}

TEST(TriangleTests, intersection_2trianglesTest) {
  //=================================================================================================
  // Coincident

  // 1
  {
    SCOPED_TRACE("case 1");
    Triangle t1 = Triangle(Point(0, 0, 0), Point(-1, 0, 0), Point(0, -1, 0));
    Triangle t2 = Triangle(Point(-2, 0, 0), Point(0, 1, 0), Point(0, 2, 0));
    bool result = intersection_2triangles(t1, t2);
    ASSERT_EQ(result, false);
  }

  // 2
  {
    SCOPED_TRACE("case 2");
    Triangle t1 = Triangle(Point(0, 0, 0), Point(-1, 0, 0), Point(0, -1, 0));
    Triangle t2 = Triangle(Point(-1, 0, 0), Point(0, 1, 0), Point(0, 2, 0));
    bool result = intersection_2triangles(t1, t2);
    ASSERT_EQ(result, true);
  }

  // 3
  {
    SCOPED_TRACE("case 3");
    Triangle t1 = Triangle(Point(0, 0, 0), Point(-1, 0, 0), Point(0, -1, 0));
    Triangle t2 = Triangle(Point(-0.5, 0, 0), Point(0, 1, 0), Point(0, 2, 0));
    bool result = intersection_2triangles(t1, t2);
    ASSERT_EQ(result, true);
  }

  // 4
  {
    SCOPED_TRACE("case 4");
    Triangle t1 = Triangle(Point(0, 0, 0), Point(-1, 0, 0), Point(0, -1, 0));
    Triangle t2 = Triangle(Point(-3, 0, 0), Point(0, -1.5, 0), Point(0, -0.5, 0));
    bool result = intersection_2triangles(t1, t2);
    ASSERT_EQ(result, true);
  }

  // 5
  {
    SCOPED_TRACE("case 5");
    Triangle t1 = Triangle(Point(0, 0, 0), Point(-2, 0, 0), Point(0, -2, 0));
    Triangle t2 = Triangle(Point(-2, -2, 0), Point(0, 1, 0), Point(1, 0, 0));
    bool result = intersection_2triangles(t1, t2);
    ASSERT_EQ(result, true);
  }

  // 6
  {
    SCOPED_TRACE("case 6");
    Triangle t1 = Triangle(Point(0, 0, 0), Point(-2, 0, 0), Point(0, -2, 0));
    Triangle t2 = Triangle(Point(-3, 0, 0), Point(0, -3, 0), Point(1, 1, 0));
    bool result = intersection_2triangles(t1, t2);
    ASSERT_EQ(result, true);
  }

  // 7
  {
    SCOPED_TRACE("case 7");
    Triangle t1 = Triangle(Point(0, 0, 0), Point(-2, 0, 0), Point(0, -2, 0));
    Triangle t2 = Triangle(Point(-3, 0, 0), Point(0, -3, 0), Point(-1, -1, 0));
    bool result = intersection_2triangles(t1, t2);
    ASSERT_EQ(result, true);
  }

  // 8
  {
    SCOPED_TRACE("case 8");
    Triangle t1 = Triangle(Point(0, 0, 0), Point(-2, 0, 0), Point(0, -2, 0));
    Triangle t2 = Triangle(Point(-3, 0, 0), Point(0, -3, 0), Point(-2, -2, 0));
    bool result = intersection_2triangles(t1, t2);
    ASSERT_EQ(result, false);
  }

  //=================================================================================================
  // Intersection

  // 9
  {
    SCOPED_TRACE("case 9");
    Triangle t1 = Triangle(Point(0, 0, 0), Point(0, 1, 0), Point(-2, 0, 0));
    Triangle t2 = Triangle(Point(0, 0, 1), Point(0, 1, 0), Point(-2, 0, 0));
    bool result = intersection_2triangles(t1, t2);
    ASSERT_EQ(result, true);
  }

  // 10
  {
    SCOPED_TRACE("case 10");
    Triangle t1 = Triangle(Point(-3, 0, 0), Point(0, -1, 0), Point(0, 0, 1));
    Triangle t2 = Triangle(Point(0, 0, 0), Point(0, 1, 0), Point(-2, 0, 0));
    bool result = intersection_2triangles(t1, t2);
    ASSERT_EQ(result, false);
  }

  // 11
  {
    SCOPED_TRACE("case 11");
    Triangle t1 = Triangle(Point(0, 0, 0), Point(-3, 0, 0), Point(0, 0, 2));
    Triangle t2 = Triangle(Point(0, 0, 0), Point(0, 1, 0), Point(-2, 0, 0));
    bool result = intersection_2triangles(t1, t2);
    ASSERT_EQ(result, true);
  }

  // 12
  {
    SCOPED_TRACE("case 12");
    Triangle t1 = Triangle(Point(1, 0, 0), Point(-3, 0, 0), Point(0, 0, 2));
    Triangle t2 = Triangle(Point(0, 0, 0), Point(0, 1, 0), Point(-2, 0, 0));
    bool result = intersection_2triangles(t1, t2);
    ASSERT_EQ(result, true);
  }

  // 13
  {
    SCOPED_TRACE("case 13");
    Triangle t1 = Triangle(Point(-2, 0, 0), Point(-3, 0, 0), Point(0, 0, 2));
    Triangle t2 = Triangle(Point(0, 0, 0), Point(0, 1, 0), Point(-2, 0, 0));
    bool result = intersection_2triangles(t1, t2);
    ASSERT_EQ(result, true);
  }

  // 14
  {
    SCOPED_TRACE("case 14");
    Triangle t1 = Triangle(Point(-3, 0, 0), Point(-4, 0, 0), Point(0, 0, 2));
    Triangle t2 = Triangle(Point(0, 0, 0), Point(0, 1, 0), Point(-2, 0, 0));
    bool result = intersection_2triangles(t1, t2);
    ASSERT_EQ(result, false);
  }

  // 15
  {
    SCOPED_TRACE("case 15");
    Triangle t1 = Triangle(Point(-4, 0, 0), Point(0, 2, 0), Point(0, 0, 2));
    Triangle t2 = Triangle(Point(0, 0, 0), Point(0, 1, 0), Point(-2, 0, 0));
    bool result = intersection_2triangles(t1, t2);
    ASSERT_EQ(result, false);
  }

  // 16
  {
    SCOPED_TRACE("case 16");
    Triangle t1 = Triangle(Point(2, 1, 0), Point(-2, -1, 0), Point(0, 0, 2));
    Triangle t2 = Triangle(Point(0, 0, 0), Point(0, 1, 0), Point(-2, 0, 0));
    bool result = intersection_2triangles(t1, t2);
    ASSERT_EQ(result, true);
  }

  // 17
  {
    SCOPED_TRACE("case 17");
    Triangle t1 = Triangle(Point(1, 1, 0), Point(-1, 1, 0), Point(0, 0, 0));
    Triangle t2 = Triangle(Point(0, 0, 0), Point(0, 0, 2), Point(2, 0, 2));
    bool result = intersection_2triangles(t1, t2);
    ASSERT_EQ(result, true);
  }

  // 18
  {
    SCOPED_TRACE("case 18");
    Triangle t1 = Triangle(Point(1, 1, 0), Point(-1, 1, 0), Point(0, 0, 0));
    Triangle t2 = Triangle(Point(-1, 0, 0), Point(0, 0, 2), Point(2, 0, 2));
    bool result = intersection_2triangles(t1, t2);
    ASSERT_EQ(result, false);
  }

  // 19
  {
    SCOPED_TRACE("case 19");
    Triangle t1 = Triangle(Point(1, 1, 0), Point(-1, 1, 0), Point(0, 0, 0));
    Triangle t2 = Triangle(Point(0, 1, 0), Point(0, 0, 2), Point(2, 0, 2));
    bool result = intersection_2triangles(t1, t2);
    ASSERT_EQ(result, true);
  }

  // 20
  {
    SCOPED_TRACE("case 20");
    Triangle t1 = Triangle(Point(1, 1, 0), Point(-1, 1, 0), Point(0, 0, 0));
    Triangle t2 = Triangle(Point(2, 1, 0), Point(0, 0, 2), Point(2, 0, 2));
    bool result = intersection_2triangles(t1, t2);
    ASSERT_EQ(result, false);
  }

  //=================================================================================================
  // Degenerate cases

  // Segment & Segment

  // 21
  {
    SCOPED_TRACE("case 21");
    Triangle t1 = Triangle(Point(-2, 2, 0), Point(-1, 1, 0), Point(0, 0, 0));
    Triangle t2 = Triangle(Point(0, 0, 1), Point(1, -1, 2), Point(-2, 2, -1));
    bool result = intersection_2triangles(t1, t2);
    ASSERT_EQ(result, true);
  }

  // 22
  {
    SCOPED_TRACE("case 22");
    Triangle t1 = Triangle(Point(-2, 2, 0), Point(-1, 1, 0), Point(0, 0, 0));
    Triangle t2 = Triangle(Point(0, 0, 1), Point(1, -1, 2), Point(0, 0, 1));
    bool result = intersection_2triangles(t1, t2);
    ASSERT_EQ(result, false); // segments dont intersect
  }

  // 23
  {
    SCOPED_TRACE("case 23");
    Triangle t1 = Triangle(Point(-2, 2, 0), Point(-1, 1, 0), Point(0, 0, 0));
    Triangle t2 = Triangle(Point(0, 0, 1), Point(-2, 2, -1), Point(0, 0, 1));
    bool result = intersection_2triangles(t1, t2);
    ASSERT_EQ(result, true);
  }

  // 24
  {
    SCOPED_TRACE("case 24");
    Triangle t1 = Triangle(Point(-2, 2, 0), Point(-1, 1, 0), Point(0, 0, 0));
    Triangle t2 = Triangle(Point(-2, 2, 1), Point(-1, 1, 1), Point(0, 0, 1));
    bool result = intersection_2triangles(t1, t2);
    ASSERT_EQ(result, false);
  }

  // 25
  {
    SCOPED_TRACE("case 25");
    Triangle t1 = Triangle(Point(-2, 2, 0), Point(1, -1, 0), Point(1, -1, 0));
    Triangle t2 = Triangle(Point(0, 0, -1), Point(0, 0, 2), Point(0, 0, 2));
    bool result = intersection_2triangles(t1, t2);
    ASSERT_EQ(result, true);
  }

  // Point & Point

  // 26
  {
    SCOPED_TRACE("case 26");
    Triangle t1 = Triangle(Point(-2, 2, 0), Point(-2, 2, 0), Point(-2, 2, 0));
    Triangle t2 = Triangle(Point(0, 0, 1), Point(0, 0, 1), Point(0, 0, 1));
    bool result = intersection_2triangles(t1, t2);
    ASSERT_EQ(result, false);
  }

  // 27
  {
    SCOPED_TRACE("case 27");
    Triangle t1 = Triangle(Point(0, 0, 1), Point(0, 0, 1), Point(0, 0, 1));
    Triangle t2 = Triangle(Point(0, 0, 1), Point(0, 0, 1), Point(0, 0, 1));
    bool result = intersection_2triangles(t1, t2);
    ASSERT_EQ(result, true);
  }

  // Segment & Point

  // 28
  {
    SCOPED_TRACE("case 28");
    Triangle t1 = Triangle(Point(-2, 2, 0), Point(-1, 1, 0), Point(0, 0, 0));
    Triangle t2 = Triangle(Point(-2, 2, 0), Point(-2, 2, 0), Point(-2, 2, 0));
    bool result = intersection_2triangles(t1, t2);
    ASSERT_EQ(result, true);
  }

  // 29
  {
    SCOPED_TRACE("case 29");
    Triangle t1 = Triangle(Point(-2, 2, 0), Point(-1, 1, 0), Point(0, 0, 0));
    Triangle t2 = Triangle(Point(-1, 1, 0), Point(-1, 1, 0), Point(-1, 1, 0));
    bool result = intersection_2triangles(t1, t2);
    ASSERT_EQ(result, true);
  }

  // 30
  {
    SCOPED_TRACE("case 30");
    Triangle t1 = Triangle(Point(-2, 2, 0), Point(-1, 1, 0), Point(0, 0, 0));
    Triangle t2 = Triangle(Point(1, -1, 0), Point(1, -1, 0), Point(1, -1, 0));
    bool result = intersection_2triangles(t1, t2);
    ASSERT_EQ(result, false);
  }

  // Triangle & Point

  // 31
  {
    SCOPED_TRACE("case 31");
    Triangle t1 = Triangle(Point(0, 0, 0), Point(-1, 0, 0), Point(0, 1, 0));
    Triangle t2 = Triangle(Point(0, 0, 0), Point(0, 0, 0), Point(0, 0, 0));
    bool result = intersection_2triangles(t1, t2);
    ASSERT_EQ(result, true);
  }

  // 32
  {
    SCOPED_TRACE("case 32");
    Triangle t1 = Triangle(Point(0, 0, 0), Point(-1, 0, 0), Point(0, 1, 0));
    Triangle t2 = Triangle(Point(0, 0, 1), Point(0, 0, 1), Point(0, 0, 1));
    bool result = intersection_2triangles(t1, t2);
    ASSERT_EQ(result, false);
  }

  // 33
  {
    SCOPED_TRACE("case 33");
    Triangle t1 = Triangle(Point(0, 0, 0), Point(-1, 0, 0), Point(0, 1, 0));
    Triangle t2 = Triangle(Point(-0.5, 0.5, 0), Point(-0.5, 0.5, 0), Point(-0.5, 0.5, 0));
    bool result = intersection_2triangles(t1, t2);
    ASSERT_EQ(result, true);
  }

  // Triangle & Segment

  // 34
  {
    SCOPED_TRACE("case 34");
    Triangle t1 = Triangle(Point(0, 0, 0), Point(-1, 0, 0), Point(0, 1, 0));
    Triangle t2 = Triangle(Point(-0.5, 0.5, 0), Point(-0.5, 0.5, 0), Point(0, 0, 1));
    bool result = intersection_2triangles(t1, t2);
    ASSERT_EQ(result, true);
  }

  // 35
  {
    SCOPED_TRACE("case 35");
    Triangle t1 = Triangle(Point(0, 0, 0), Point(-1, 0, 0), Point(0, 1, 0));
    Triangle t2 = Triangle(Point(-1, 1, -1), Point(-1, 1, -1), Point(0, 0, 1));
    bool result = intersection_2triangles(t1, t2);
    ASSERT_EQ(result, true);
  }

  // 36
  {
    SCOPED_TRACE("case 36");
    Triangle t1 = Triangle(Point(0, 0, 0), Point(-1, 0, 0), Point(0, 1, 0));
    Triangle t2 = Triangle(Point(-1, 1, -1), Point(-1, 1, -1), Point(0, 0, 2));
    bool result = intersection_2triangles(t1, t2);
    ASSERT_EQ(result, false);
  }

  // 37
  {
    SCOPED_TRACE("case 37");
    Triangle t1 = Triangle(Point(0, 0, 0), Point(-1, 0, 0), Point(0, 1, 0));
    Triangle t2 = Triangle(Point(0, 2, 0), Point(0, 2, 0), Point(0, 0, 2));
    bool result = intersection_2triangles(t1, t2);
    ASSERT_EQ(result, false);
  }

  // 38
  {
    SCOPED_TRACE("case 38");
    Triangle t1 = Triangle(Point(0, 0, 0), Point(-1, 0, 0), Point(0, 1, 0));
    Triangle t2 = Triangle(Point(0, 2, 0), Point(0, 2, 0), Point(0, -1, 0));
    bool result = intersection_2triangles(t1, t2);
    ASSERT_EQ(result, true);
  }

  // 39
  {
    SCOPED_TRACE("case 39");
    Triangle t1 = Triangle(Point(0, 0, 0), Point(-1, 0, 0), Point(0, 1, 0));
    Triangle t2 = Triangle(Point(-1, -0.5, 0), Point(0.5, 1, 0), Point(-1, -0.5, 0));
    bool result = intersection_2triangles(t1, t2);
    ASSERT_EQ(result, true);
  }

  // 40
  {
    SCOPED_TRACE("case 40");
    Triangle t1 = Triangle(Point(0, 0, 0), Point(-1, 0, 0), Point(0, 1, 0));
    Triangle t2 = Triangle(Point(-2, -0.5, 0), Point(1, 1, 0), Point(-2, -0.5, 0));
    bool result = intersection_2triangles(t1, t2);
    ASSERT_EQ(result, true);
  }

  // 41
  {
    SCOPED_TRACE("case 41");
    Triangle t1 = Triangle(Point(0, 0, 0), Point(-1, 0, 0), Point(0, 1, 0));
    Triangle t2 = Triangle(Point(-1, 0, 0), Point(0, 1, 0), Point(0, 1, 0));
    bool result = intersection_2triangles(t1, t2);
    ASSERT_EQ(result, true);
  }

  // 42
  {
    SCOPED_TRACE("case 42");
    Triangle t1 = Triangle(Point(0, 0, 0), Point(-1, 0, 0), Point(0, 1, 0));
    Triangle t2 = Triangle(Point(-2, 0, 0), Point(-2, 0, 0), Point(0, 2, 0));
    bool result = intersection_2triangles(t1, t2);
    ASSERT_EQ(result, false);
  }

  // 43
  {
    SCOPED_TRACE("case 43");
    Triangle t1 = Triangle(Point(0, 0, 0), Point(-1, 0, 0), Point(0, 1, 0));
    Triangle t2 = Triangle(Point(1, 1, 0), Point(0.5, 1, 0), Point(-1, 1, 0));
    bool result = intersection_2triangles(t1, t2);
    ASSERT_EQ(result, true);
  }

  // 44
  {
    SCOPED_TRACE("case 44");
    Triangle t1 = Triangle(Point(0, 0, 0), Point(-1, 0, 0), Point(0, 1, 0));
    Triangle t2 = Triangle(Point(1, 1, 1), Point(0.5, 1, 1), Point(-1, 1, 1));
    bool result = intersection_2triangles(t1, t2);
    ASSERT_EQ(result, false);
  }

  // 44
  {
    SCOPED_TRACE("case 45");
    Triangle t1 = Triangle(Point(1, 1, 0), Point(3, 1, 0), Point(1, 3, 0));
    Triangle t2 = Triangle(Point(1, 1, 0),  Point(1, 2, 3), Point(5, 4, 8));
    bool result = intersection_2triangles(t1, t2);
    ASSERT_EQ(result, true);
  }
}

int main(int argc, char **argv) {
  ::testing::InitGoogleTest(&argc, argv);
  return RUN_ALL_TESTS();
}