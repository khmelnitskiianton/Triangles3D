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
  bool res1 = equal(Line(Point(0, -1, 0), Point(-2, 0, 0)), 
                    Line(Point(-6, 2, 0), Point(-4, 1, 0)));
  ASSERT_EQ(res1, true);
}

TEST(LineTests, intersection_2linesTest) {
  // 1
{ SCOPED_TRACE("case 1");
  Line l1 = Line(Point(-2, 0, 0), Point(0,1,0));
  Line l2 = Line(Point(1, 0, 0), Point(0,-2,0));
  std::pair<LineToLineOrientation, Point> result = intersection_2lines(l1, l2);
  Line prob_inter = Line(Point(-6, 2, 0), Point(-4, 1, 0));
  ASSERT_EQ(result.first, LineToLineOrientation::Intersect);
  ASSERT_TRUE(equal(result.second, Point(2, 2, 0)));
}
  // 2
{ SCOPED_TRACE("case 2");
  Line l1 = Line(Point(-2, 0, 0), Point(0,1,0));
  Line l2 = Line(Point(4, 0, 0), Point(0,-2,0));
  std::pair<LineToLineOrientation, Point> result = intersection_2lines(l1, l2);
  ASSERT_EQ(result.first, LineToLineOrientation::Parallel);
  ASSERT_TRUE(result.second.isBad());
}
  // 3
{ SCOPED_TRACE("case 3");
  Line l1 = Line(Point(-2, 0, 0), Point(0,1,0));
  Line l2 = Line(Point(4,3,0), Point(2, 2, 0));
  std::pair<LineToLineOrientation, Point> result = intersection_2lines(l1, l2);
  ASSERT_EQ(result.first, LineToLineOrientation::Coincident);
  ASSERT_TRUE(result.second.isBad());
}

  // 4
{ SCOPED_TRACE("case 4");
  Line l1 = Line(Point(-2, 0, 0), Point(0,1,0));
  Line l2 = Line(Point(1, 0, 1), Point(0,-2,1));
  std::pair<LineToLineOrientation, Point> result = intersection_2lines(l1, l2);
  ASSERT_EQ(result.first, LineToLineOrientation::Skew);
  ASSERT_TRUE(result.second.isBad());
}

  // 5
{ SCOPED_TRACE("case 5");
  Line l1 = Line(Point(-2, 0, 0), Point(0,1,0));
  Line l2 = Line::badLine();
  std::pair<LineToLineOrientation, Point> result = intersection_2lines(l1, l2);
  ASSERT_EQ(result.first, LineToLineOrientation::Invalid);
  ASSERT_TRUE(result.second.isBad());
}
}

//=================================================================================================
// PLANE TESTS

TEST(PlaneTests, pointPositionTest) {
  Plane p1 = Plane(Point(-2, 0, 0), Point(0,0,1), Point(0,-1,0));
  ASSERT_EQ(p1.pointPosition(Point(1, 0, 0)),PlaneSide::PositiveHalfSpace);
  ASSERT_EQ(p1.pointPosition(Point(1, 0, 1.5)),PlaneSide::BelongsToPlane);
  ASSERT_EQ(p1.pointPosition(Point(-4, 1, 0)),PlaneSide::BelongsToPlane);
  ASSERT_EQ(p1.pointPosition(Point(-3, 0, 0)),PlaneSide::NegativeHalfSpace);
}

TEST(PlaneTests, intersection_2planesTest) {
  // 1
{ SCOPED_TRACE("case 1");
  Plane p1 = Plane(Point(-2, 0, 0), Point(0,0,1), Point(0,-1,0));
  Plane p2 = Plane(Point(0, 0, 0), Point(0,1,0), Point(-1,0,0));
  std::pair<PlaneToPlaneOrientation, Line> result = intersection_2planes(p1, p2);
  Line prob_inter = Line(Point(-6, 2, 0), Point(-4, 1, 0));
  ASSERT_EQ(result.first, PlaneToPlaneOrientation::Intersect);
  ASSERT_TRUE(equal(result.second, prob_inter));
}
  // 2 
{ SCOPED_TRACE("case 2");
  Plane p1 = Plane(Point(0, 0, 0), Point(0,1,0), Point(-1,0,0));
  Plane p2 = Plane(Point(2, 0, 1), Point(-2,1,1), Point(-1,0,1));
  std::pair<PlaneToPlaneOrientation, Line> result = intersection_2planes(p1, p2);
  Line prob_inter = Line(Point(-6, 2, 0), Point(-4, 1, 0));
  ASSERT_EQ(result.first, PlaneToPlaneOrientation::Parallel);
  ASSERT_TRUE(result.second.isBad());
}

  // 3 
{ SCOPED_TRACE("case 3");
  Plane p1 = Plane(Point(0, 0, 0), Point(0,1,0), Point(-1,0,0));
  Plane p2 = Plane(Point(2, 0, 0), Point(-2,1,0), Point(-1,0,0));
  std::pair<PlaneToPlaneOrientation, Line> result = intersection_2planes(p1, p2);
  Line prob_inter = Line(Point(-6, 2, 0), Point(-4, 1, 0));
  ASSERT_EQ(result.first, PlaneToPlaneOrientation::Coincident);
  ASSERT_TRUE(result.second.isBad());
}

}

TEST (PlaneTests, BadZeroTest) {
  Plane bad_plane1 = Plane(Point(NAN, NAN, NAN), Point(NAN, NAN, NAN), Point(NAN, NAN, NAN));
  Plane bad_plane2 = Plane(Point(NAN, NAN, NAN), Vector(4,3,2));
  Plane bad_plane3 = Plane(Point(4, 3, 2), Vector(4,3,NAN));

  Plane degenerate_plane1 = Plane(Point(3,2,4), Vector(0,0,0));

  ASSERT_TRUE(bad_plane1.isBad());
  ASSERT_TRUE(bad_plane2.isBad());
  ASSERT_TRUE(bad_plane3.isBad());
  ASSERT_TRUE(degenerate_plane1.isDegenerate());
}

//=================================================================================================
// Segment Tests
TEST (SegmentTests, equalTest) {
  // 1
{ SCOPED_TRACE("case 1");
  // On one line
  Segment s1 = Segment(Point(-2, 0, 0), Point(2,2,0));
  Segment s2 = Segment(Point(2,2,0), Point(-2, 0, 0));
  bool result = equal(s1, s2);
  ASSERT_EQ(result, true);
}
  // 2
{ SCOPED_TRACE("case 2");
  // On one line
  Segment s1 = Segment(Point(-2, 0, 0), Point(2,2,0));
  Segment s2 = Segment(Point(-2, 0, 0), Point(2,2,0));
  bool result = equal(s1, s2);
  ASSERT_EQ(result, true);
}

  // 3
{ SCOPED_TRACE("case 3");
  // On one line
  Segment s1 = Segment(Point(-2, 0, 0), Point(2,2,0));
  Segment s2 = Segment(Point(-2, 0, 1), Point(2,2,1));
  bool result = equal(s1, s2);
  ASSERT_EQ(result, false);
}
}

TEST (SegmentTests, intersection_2segmentsTest) {
  // 1
{ SCOPED_TRACE("case 1");
  // On one line
  Segment s1 = Segment(Point(-2, 0, 0), Point(2,2,0));
  Segment s2 = Segment(Point(0, 1, 0), Point(-4,-1,0));
  bool result = intersection_2segments(s1, s2);
  ASSERT_EQ(result, true);
}

  // 2
{ SCOPED_TRACE("case 2");
  // On one line
  Segment s1 = Segment(Point(-2, 0, 0), Point(2,2,0));
  Segment s2 = Segment(Point(2, 2, 0), Point(-4,-1,0));
  bool result = intersection_2segments(s1, s2);
  ASSERT_EQ(result, true);
}

  // 3
{ SCOPED_TRACE("case 3");
  // On one line
  Segment s1 = Segment(Point(-2, 0, 0), Point(-4,-1,0));
  Segment s2 = Segment(Point(2, 2, 0), Point(-2, 0, 0));
  bool result = intersection_2segments(s1, s2);
  ASSERT_EQ(result, true);
}

  // 4
{ SCOPED_TRACE("case 4");
  // On one line
  Segment s1 = Segment(Point(-2, 0, 0), Point(-4,-1,0));
  Segment s2 = Segment(Point(2, 2, 0), Point(0, 1, 0));
  bool result = intersection_2segments(s1, s2);
  ASSERT_EQ(result, false);
}

}

//=================================================================================================
// Triangle Tests

TEST(TriangleTests, intersection_line_triangleTest) {
  // 1
{ SCOPED_TRACE("case 1");

  Triangle t = Triangle(Point(0, 0, 0), Point(0,1,0), Point(-2,0,0));
  Line l = Line(Point(1, 0, 0), Point(0,-1,0));
  std::optional<Segment> result = intersection_triangle_line(t, l);
  ASSERT_EQ(result, std::nullopt);
}

  // 2
{ SCOPED_TRACE("case 2");
  Triangle t = Triangle(Point(0, 0, 0), Point(0,1,0), Point(-2,0,0));
  Line l = Line(Point(0, 1, 0), Point(0,0,0));
  std::optional<Segment> result = intersection_triangle_line(t, l);
  ASSERT_NE(result, std::nullopt);
  ASSERT_EQ(equal(*result, Segment(Point(0, 1, 0), Point(0,0,0))), true);
}

  // 3
{ SCOPED_TRACE("case 3");
  Triangle t = Triangle(Point(0, 0, 0), Point(0,1,0), Point(-2,0,0));
  Line l = Line(Point(-1, 0, 0), Vector(0,3,0));
  std::optional<Segment> result = intersection_triangle_line(t, l);
  ASSERT_NE(result, std::nullopt);
  ASSERT_EQ(equal(*result, Segment(Point(-1, 0.5, 0), Point(-1,0,0))), true);
}

  // 4
{ SCOPED_TRACE("case 4");
  Triangle t = Triangle(Point(0, 0, 0), Point(0,1,0), Point(-2,0,0));
  Line l = Line(Point(1, 1, 0), Vector(2,2,0));
  std::optional<Segment> result = intersection_triangle_line(t, l);
  ASSERT_NE(result, std::nullopt);
  result->print(std::cout);
  ASSERT_EQ(result->isDegenerate(), true);
}

}

TEST(TriangleTests, intersection_2triangles_intersect_planesTest) {
  // 1
{ SCOPED_TRACE("case 1");

  Triangle t1 = Triangle(Point(0, 0, 0), Point(0,1,0), Point(-2,0,0));
  Triangle t2 = Triangle(Point(0, 0, 1), Point(0,1,0), Point(-2,0,0));
  bool result = intersection_2triangles_intersect_planes(t1, t2);
  ASSERT_EQ(result, true);
}

  // 2
{ SCOPED_TRACE("case 2");

  Triangle t1 = Triangle(Point(-3, 0, 0), Point(0,-1,0), Point(0,0,1));
  Triangle t2 = Triangle(Point(0, 0, 0), Point(0,1,0), Point(-2,0,0));
  bool result = intersection_2triangles_intersect_planes(t1, t2);
  ASSERT_EQ(result, false);
}

  // 3
{ SCOPED_TRACE("case 3");

  Triangle t1 = Triangle(Point(0, 0, 0), Point(-3,0,0), Point(0,0,2));
  Triangle t2 = Triangle(Point(0, 0, 0), Point(0,1,0), Point(-2,0,0));
  bool result = intersection_2triangles_intersect_planes(t1, t2);
  ASSERT_EQ(result, true);
}

  // 4
{ SCOPED_TRACE("case 4");

  Triangle t1 = Triangle(Point(1, 0, 0), Point(-3,0,0), Point(0,0,2));
  Triangle t2 = Triangle(Point(0, 0, 0), Point(0,1,0), Point(-2,0,0));
  bool result = intersection_2triangles_intersect_planes(t1, t2);
  ASSERT_EQ(result, true);
}

  // 5
{ SCOPED_TRACE("case 5");

  Triangle t1 = Triangle(Point(-2, 0, 0), Point(-3,0,0), Point(0,0,2));
  Triangle t2 = Triangle(Point(0, 0, 0), Point(0,1,0), Point(-2,0,0));
  bool result = intersection_2triangles_intersect_planes(t1, t2);
  ASSERT_EQ(result, true);
}

  // 6
{ SCOPED_TRACE("case 6");

  Triangle t1 = Triangle(Point(-3, 0, 0), Point(-4,0,0), Point(0,0,2));
  Triangle t2 = Triangle(Point(0, 0, 0), Point(0,1,0), Point(-2,0,0));
  bool result = intersection_2triangles_intersect_planes(t1, t2);
  ASSERT_EQ(result, false);
}

  // 7
{ SCOPED_TRACE("case 7");

  Triangle t1 = Triangle(Point(-4, 0, 0), Point(0,2,0), Point(0,0,2));
  Triangle t2 = Triangle(Point(0, 0, 0), Point(0,1,0), Point(-2,0,0));
  bool result = intersection_2triangles_intersect_planes(t1, t2);
  ASSERT_EQ(result, false);
}

  // 8
{ SCOPED_TRACE("case 8");

  Triangle t1 = Triangle(Point(2, 1, 0), Point(-2,-1,0), Point(0,0,2));
  Triangle t2 = Triangle(Point(0, 0, 0), Point(0,1,0), Point(-2,0,0));
  bool result = intersection_2triangles_intersect_planes(t1, t2);
  ASSERT_EQ(result, true);
}

}

int main(int argc, char **argv) {
  ::testing::InitGoogleTest(&argc, argv);
  return RUN_ALL_TESTS();
}