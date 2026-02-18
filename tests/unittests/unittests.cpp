#include <gmock/gmock.h>
#include <gtest/gtest.h>

#include <limits>
#include <optional>
#include <utility>
#include <variant>

#include "primitives/line.hpp"
#include "primitives/plane.hpp"
#include "primitives/point.hpp"
#include "primitives/segment.hpp"
#include "primitives/triangle.hpp"
#include "primitives/vector.hpp"

// Use one scalar type everywhere in tests
using T = double;

using P  = Point<T>;
using V  = Vector<T>;
using L  = Line<T>;
using Pl = Plane<T>;
using S  = Segment<T>;
using Tr = Triangle<T>;

static constexpr T NaN() { return std::numeric_limits<T>::quiet_NaN(); }

//=================================================================================================
// LINE TESTS

TEST(LineTests, equalTest) {
  bool res1 = equal(
      L(P(0, -1, 0), P(-2, 0, 0)),
      L(P(-6, 2, 0), P(-4, 1, 0)));
  ASSERT_EQ(res1, true);
}

TEST(LineTests, intersection_2linesTest) {
  // 1
  {
    SCOPED_TRACE("case 1");
    L l1 = L(P(-2, 0, 0), P(0, 1, 0));
    L l2 = L(P(1, 0, 0), P(0, -2, 0));
    std::pair<LineToLineOrientation, P> result = intersection_2lines<T>(l1, l2);
    ASSERT_EQ(result.first, LineToLineOrientation::Intersect);
    ASSERT_TRUE(equal(result.second, P(2, 2, 0)));
  }
  // 2
  {
    SCOPED_TRACE("case 2");
    L l1 = L(P(-2, 0, 0), P(0, 1, 0));
    L l2 = L(P(4, 0, 0), P(0, -2, 0));
    std::pair<LineToLineOrientation, P> result = intersection_2lines<T>(l1, l2);
    ASSERT_EQ(result.first, LineToLineOrientation::Parallel);
    ASSERT_TRUE(result.second.isBad());
  }
  // 3
  {
    SCOPED_TRACE("case 3");
    L l1 = L(P(-2, 0, 0), P(0, 1, 0));
    L l2 = L(P(4, 3, 0), P(2, 2, 0));
    std::pair<LineToLineOrientation, P> result = intersection_2lines<T>(l1, l2);
    ASSERT_EQ(result.first, LineToLineOrientation::Coincident);
    ASSERT_TRUE(result.second.isBad());
  }
  // 4
  {
    SCOPED_TRACE("case 4");
    L l1 = L(P(-2, 0, 0), P(0, 1, 0));
    L l2 = L(P(1, 0, 1), P(0, -2, 1));
    std::pair<LineToLineOrientation, P> result = intersection_2lines<T>(l1, l2);
    ASSERT_EQ(result.first, LineToLineOrientation::Skew);
    ASSERT_TRUE(result.second.isBad());
  }
  // 5
  {
    SCOPED_TRACE("case 5");
    L l1 = L(P(-2, 0, 0), P(0, 1, 0));
    L l2 = L::badLine();
    std::pair<LineToLineOrientation, P> result = intersection_2lines<T>(l1, l2);
    ASSERT_EQ(result.first, LineToLineOrientation::Invalid);
    ASSERT_TRUE(result.second.isBad());
  }
}

//=================================================================================================
// PLANE TESTS

TEST(PlaneTests, pointPositionTest) {
  Pl p1 = Pl(P(-2, 0, 0), P(0, 0, 1), P(0, -1, 0));
  ASSERT_EQ(p1.pointPosition(P(1, 0, 0)), PlaneSide::PositiveHalfSpace);
  ASSERT_EQ(p1.pointPosition(P(1, 0, 1.5)), PlaneSide::BelongsToPlane);
  ASSERT_EQ(p1.pointPosition(P(-4, 1, 0)), PlaneSide::BelongsToPlane);
  ASSERT_EQ(p1.pointPosition(P(-3, 0, 0)), PlaneSide::NegativeHalfSpace);
}

TEST(PlaneTests, intersection_2planesTest) {
  // 1
  {
    SCOPED_TRACE("case 1");
    Pl p1 = Pl(P(-2, 0, 0), P(0, 0, 1), P(0, -1, 0));
    Pl p2 = Pl(P(0, 0, 0), P(0, 1, 0), P(-1, 0, 0));
    std::pair<PlaneToPlaneOrientation, L> result = intersection_2planes<T>(p1, p2);
    L prob_inter = L(P(-6, 2, 0), P(-4, 1, 0));
    ASSERT_EQ(result.first, PlaneToPlaneOrientation::Intersect);
    ASSERT_TRUE(equal(result.second, prob_inter));
  }
  // 2
  {
    SCOPED_TRACE("case 2");
    Pl p1 = Pl(P(0, 0, 0), P(0, 1, 0), P(-1, 0, 0));
    Pl p2 = Pl(P(2, 0, 1), P(-2, 1, 1), P(-1, 0, 1));
    std::pair<PlaneToPlaneOrientation, L> result = intersection_2planes<T>(p1, p2);
    ASSERT_EQ(result.first, PlaneToPlaneOrientation::Parallel);
    ASSERT_TRUE(result.second.isBad());
  }
  // 3
  {
    SCOPED_TRACE("case 3");
    Pl p1 = Pl(P(0, 0, 0), P(0, 1, 0), P(-1, 0, 0));
    Pl p2 = Pl(P(2, 0, 0), P(-2, 1, 0), P(-1, 0, 0));
    std::pair<PlaneToPlaneOrientation, L> result = intersection_2planes<T>(p1, p2);
    ASSERT_EQ(result.first, PlaneToPlaneOrientation::Coincident);
    ASSERT_TRUE(result.second.isBad());
  }
}

TEST(PlaneTests, BadZeroTest) {
  Pl bad_plane1 = Pl(P(NaN(), NaN(), NaN()), P(NaN(), NaN(), NaN()), P(NaN(), NaN(), NaN()));
  Pl bad_plane2 = Pl(P(NaN(), NaN(), NaN()), V(4, 3, 2));
  Pl bad_plane3 = Pl(P(4, 3, 2), V(4, 3, NaN()));

  Pl degenerate_plane1 = Pl(P(3, 2, 4), V(0, 0, 0));

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
    S s1 = S(P(-2, 0, 0), P(2, 2, 0));
    S s2 = S(P(2, 2, 0), P(-2, 0, 0));
    bool result = equal(s1, s2);
    ASSERT_EQ(result, true);
  }
  // 2
  {
    SCOPED_TRACE("case 2");
    S s1 = S(P(-2, 0, 0), P(2, 2, 0));
    S s2 = S(P(-2, 0, 0), P(2, 2, 0));
    bool result = equal(s1, s2);
    ASSERT_EQ(result, true);
  }
  // 3
  {
    SCOPED_TRACE("case 3");
    S s1 = S(P(-2, 0, 0), P(2, 2, 0));
    S s2 = S(P(-2, 0, 1), P(2, 2, 1));
    bool result = equal(s1, s2);
    ASSERT_EQ(result, false);
  }
}

TEST(SegmentTests, intersection_2segments_on_lineTest) {
  // 1
  {
    SCOPED_TRACE("case 1");
    S s1 = S(P(-2, 0, 0), P(2, 2, 0));
    S s2 = S(P(0, 1, 0), P(-4, -1, 0));
    bool result = intersection_2segments_on_line<T>(s1, s2);
    ASSERT_EQ(result, true);
  }
  // 2
  {
    SCOPED_TRACE("case 2");
    S s1 = S(P(-2, 0, 0), P(2, 2, 0));
    S s2 = S(P(2, 2, 0), P(-4, -1, 0));
    bool result = intersection_2segments_on_line<T>(s1, s2);
    ASSERT_EQ(result, true);
  }
  // 3
  {
    SCOPED_TRACE("case 3");
    S s1 = S(P(-2, 0, 0), P(-4, -1, 0));
    S s2 = S(P(2, 2, 0), P(-2, 0, 0));
    bool result = intersection_2segments_on_line<T>(s1, s2);
    ASSERT_EQ(result, true);
  }
  // 4
  {
    SCOPED_TRACE("case 4");
    S s1 = S(P(-2, 0, 0), P(-4, -1, 0));
    S s2 = S(P(2, 2, 0), P(0, 1, 0));
    bool result = intersection_2segments_on_line<T>(s1, s2);
    ASSERT_EQ(result, false);
  }
}

TEST(SegmentTests, intersection_2segmentsTest) {
  // 1
  {
    SCOPED_TRACE("case 1");
    S s1 = S(P(-2, 0, 0), P(0, 1, 0));
    S s2 = S(P(-1, 0, 0), P(0, -1, 0));
    bool result = intersection_2segments<T>(s1, s2);
    ASSERT_EQ(result, false);
  }
  // 2
  {
    SCOPED_TRACE("case 2");
    S s1 = S(P(-1, -2, 0), P(0, 1, 0));
    S s2 = S(P(-1, 0, 0), P(0, -1, 0));
    bool result = intersection_2segments<T>(s1, s2);
    ASSERT_EQ(result, true);
  }
  // 3
  {
    SCOPED_TRACE("case 3");
    S s1 = S(P(-0.5, -0.5, 0), P(0, 0, 1));
    S s2 = S(P(-1, 0, 0), P(0, -1, 0));
    bool result = intersection_2segments<T>(s1, s2);
    ASSERT_EQ(result, true);
  }
  // 4
  {
    SCOPED_TRACE("case 4");
    S s1 = S(P(-2, 0, 0), P(-2, 0, 0));
    S s2 = S(P(-1, 0, 0), P(0, -1, 0));
    bool result = intersection_2segments<T>(s1, s2);
    ASSERT_EQ(result, false);
  }
  // 5
  {
    SCOPED_TRACE("case 5");
    S s1 = S(P(-0.5, -0.5, 0), P(-0.5, -0.5, 0));
    S s2 = S(P(-1, 0, 0), P(0, -1, 0));
    bool result = intersection_2segments<T>(s1, s2);
    ASSERT_EQ(result, true);
  }
  // 6
  {
    SCOPED_TRACE("case 6");
    S s1 = S(P(-2, 1, 0), P(-2, 1, 0));
    S s2 = S(P(-1, 0, 0), P(0, -1, 0));
    bool result = intersection_2segments<T>(s1, s2);
    ASSERT_EQ(result, false);
  }
}

//=================================================================================================
// Triangle Tests

TEST(TriangleTests, isIncludeTest) {
  Tr t = Tr(P(0, 0, 0), P(0, 1, 0), P(-2, 0, 0));
  // 1
  {
    SCOPED_TRACE("case 1");
    P p = P(0, 0, 0);
    bool result = t.isInclude(p);
    ASSERT_EQ(result, true);
  }
  // 2
  {
    SCOPED_TRACE("case 2");
    P p = P(1, 0, 0);
    bool result = t.isInclude(p);
    ASSERT_EQ(result, false);
  }
  // 3
  {
    SCOPED_TRACE("case 3");
    P p = P(-0.5, 0, 0);
    bool result = t.isInclude(p);
    ASSERT_EQ(result, true);
  }
  // 4
  {
    SCOPED_TRACE("case 4");
    P p = P(-0.1, 0.1, 0);
    bool result = t.isInclude(p);
    ASSERT_EQ(result, true);
  }
}

TEST(TriangleTests, collapseTest) {
  // NOTE: this assumes Triangle<T>::collapsed_ is std::variant<std::monostate, Point<T>, Segment<T>>
  using CollapsedT = decltype(std::declval<Tr>().collapsed_);

  // 1
  {
    SCOPED_TRACE("case 1");
    Tr t = Tr(P(0, 0, 0), P(0, 1, 0), P(-2, 0, 0));
    CollapsedT res = t.collapsed_;
    ASSERT_TRUE(std::holds_alternative<std::monostate>(res));
  }
  // 2
  {
    SCOPED_TRACE("case 2");
    Tr t = Tr(P(0, 0, 0), P(0, 1, 0), P(-2, 0, 1));
    CollapsedT res = t.collapsed_;
    ASSERT_TRUE(std::holds_alternative<std::monostate>(res));
  }
  // 3
  {
    SCOPED_TRACE("case 3");
    Tr t = Tr(P(0, 0, 0), P(0, 0, 0), P(0, 0, 0));
    CollapsedT res = t.collapsed_;
    ASSERT_TRUE(std::holds_alternative<P>(res));
    ASSERT_TRUE(equal(std::get<P>(res), P(0, 0, 0)));
  }
  // 4
  {
    SCOPED_TRACE("case 4");
    Tr t = Tr(P(1, 0, -1), P(1, 0, -1), P(1, 0, -1));
    CollapsedT res = t.collapsed_;
    ASSERT_TRUE(std::holds_alternative<P>(res));
    ASSERT_TRUE(equal(std::get<P>(res), P(1, 0, -1)));
  }
  // 5
  {
    SCOPED_TRACE("case 5");
    Tr t = Tr(P(1, 0, 0), P(-1, 0, 0), P(0, 0, 0));
    CollapsedT res = t.collapsed_;
    ASSERT_TRUE(std::holds_alternative<S>(res));
    ASSERT_TRUE(equal(std::get<S>(res), S(P(1, 0, 0), P(-1, 0, 0))));
  }
  // 6 (duplicate in your original, kept)
  {
    SCOPED_TRACE("case 6");
    Tr t = Tr(P(1, 0, 0), P(-1, 0, 0), P(0, 0, 0));
    CollapsedT res = t.collapsed_;
    ASSERT_TRUE(std::holds_alternative<S>(res));
    ASSERT_TRUE(equal(std::get<S>(res), S(P(1, 0, 0), P(-1, 0, 0))));
  }
  // 7
  {
    SCOPED_TRACE("case 7");
    Tr t = Tr(P(-1, 1, 0), P(0, 0, 0), P(-2, 2, 0));
    CollapsedT res = t.collapsed_;
    ASSERT_TRUE(std::holds_alternative<S>(res));
    ASSERT_TRUE(equal(std::get<S>(res), S(P(0, 0, 0), P(-2, 2, 0))));
  }
  // 8
  {
    SCOPED_TRACE("case 8");
    Tr t = Tr(P(-1, 1, 0), P(0, 0, 0), P(-2, 2, 0));
    CollapsedT res = t.collapsed_;
    ASSERT_TRUE(std::holds_alternative<S>(res));
    ASSERT_TRUE(equal(std::get<S>(res), S(P(-2, 2, 0), P(0, 0, 0))));
  }
  // 9
  {
    SCOPED_TRACE("case 9");
    Tr t = Tr(P(-1, 1, 0), P(0, 0, 1), P(1, -1, 2));
    CollapsedT res = t.collapsed_;
    ASSERT_TRUE(std::holds_alternative<S>(res));
    ASSERT_TRUE(equal(std::get<S>(res), S(P(1, -1, 2), P(-1, 1, 0))));
  }
  // 10
  {
    SCOPED_TRACE("case 10");
    Tr t = Tr(P(-1, 1, 0), P(-1, 1, 0), P(1, -1, 2));
    CollapsedT res = t.collapsed_;
    ASSERT_TRUE(std::holds_alternative<S>(res));
    ASSERT_TRUE(equal(std::get<S>(res), S(P(1, -1, 2), P(-1, 1, 0))));
  }
  // 11
  {
    SCOPED_TRACE("case 11");
    Tr t = Tr(P(1, -1, 2), P(-1, 1, 0), P(1, -1, 2));
    CollapsedT res = t.collapsed_;
    ASSERT_TRUE(std::holds_alternative<S>(res));
    ASSERT_TRUE(equal(std::get<S>(res), S(P(1, -1, 2), P(-1, 1, 0))));
  }
}

TEST(TriangleTests, intersection_line_triangleTest) {
  // 1
  {
    SCOPED_TRACE("case 1");
    Tr t = Tr(P(0, 0, 0), P(0, 1, 0), P(-2, 0, 0));
    L  l = L(P(1, 0, 0), P(0, -1, 0));
    std::optional<S> result = intersection_triangle_line_on_plane<T>(t, l);
    ASSERT_EQ(result, std::nullopt);
  }
  // 2
  {
    SCOPED_TRACE("case 2");
    Tr t = Tr(P(0, 0, 0), P(0, 1, 0), P(-2, 0, 0));
    L  l = L(P(0, 1, 0), P(0, 0, 0));
    std::optional<S> result = intersection_triangle_line_on_plane<T>(t, l);
    ASSERT_NE(result, std::nullopt);
    ASSERT_TRUE(equal(*result, S(P(0, 1, 0), P(0, 0, 0))));
  }
  // 3
  {
    SCOPED_TRACE("case 3");
    Tr t = Tr(P(0, 0, 0), P(0, 1, 0), P(-2, 0, 0));
    L  l = L(P(-1, 0, 0), V(0, 3, 0));
    std::optional<S> result = intersection_triangle_line_on_plane<T>(t, l);
    ASSERT_NE(result, std::nullopt);
    ASSERT_TRUE(equal(*result, S(P(-1, 0.5, 0), P(-1, 0, 0))));
  }
  // 4
  {
    SCOPED_TRACE("case 4");
    Tr t = Tr(P(0, 0, 0), P(0, 1, 0), P(-2, 0, 0));
    L  l = L(P(1, 1, 0), V(2, 2, 0));
    std::optional<S> result = intersection_triangle_line_on_plane<T>(t, l);
    ASSERT_NE(result, std::nullopt);
    ASSERT_TRUE(result->isDegenerate());
  }
}

TEST(TriangleTests, intersection_2triangles_intersect_planesTest) {
  // 1
  {
    SCOPED_TRACE("case 1");
    Tr t1 = Tr(P(0, 0, 0), P(0, 1, 0), P(-2, 0, 0));
    Tr t2 = Tr(P(0, 0, 1), P(0, 1, 0), P(-2, 0, 0));
    Pl p1 = t1.plane_;
    Pl p2 = t2.plane_;
    std::pair<PlaneToPlaneOrientation, L> inter = intersection_2planes<T>(p1, p2);
    bool result = intersection_2triangles_intersect_planes<T>(t1, t2, inter.second);
    ASSERT_EQ(result, true);
  }

  // 2
  {
    SCOPED_TRACE("case 2");
    Tr t1 = Tr(P(-3, 0, 0), P(0, -1, 0), P(0, 0, 1));
    Tr t2 = Tr(P(0, 0, 0), P(0, 1, 0), P(-2, 0, 0));
    Pl p1 = t1.plane_;
    Pl p2 = t2.plane_;
    std::pair<PlaneToPlaneOrientation, L> inter = intersection_2planes<T>(p1, p2);
    bool result = intersection_2triangles_intersect_planes<T>(t1, t2, inter.second);
    ASSERT_EQ(result, false);
  }

  // 3
  {
    SCOPED_TRACE("case 3");
    Tr t1 = Tr(P(0, 0, 0), P(-3, 0, 0), P(0, 0, 2));
    Tr t2 = Tr(P(0, 0, 0), P(0, 1, 0), P(-2, 0, 0));
    Pl p1 = t1.plane_;
    Pl p2 = t2.plane_;
    std::pair<PlaneToPlaneOrientation, L> inter = intersection_2planes<T>(p1, p2);
    bool result = intersection_2triangles_intersect_planes<T>(t1, t2, inter.second);
    ASSERT_EQ(result, true);
  }

  // 4
  {
    SCOPED_TRACE("case 4");
    Tr t1 = Tr(P(1, 0, 0), P(-3, 0, 0), P(0, 0, 2));
    Tr t2 = Tr(P(0, 0, 0), P(0, 1, 0), P(-2, 0, 0));
    Pl p1 = t1.plane_;
    Pl p2 = t2.plane_;
    std::pair<PlaneToPlaneOrientation, L> inter = intersection_2planes<T>(p1, p2);
    bool result = intersection_2triangles_intersect_planes<T>(t1, t2, inter.second);
    ASSERT_EQ(result, true);
  }

  // 5
  {
    SCOPED_TRACE("case 5");
    Tr t1 = Tr(P(-2, 0, 0), P(-3, 0, 0), P(0, 0, 2));
    Tr t2 = Tr(P(0, 0, 0), P(0, 1, 0), P(-2, 0, 0));
    Pl p1 = t1.plane_;
    Pl p2 = t2.plane_;
    std::pair<PlaneToPlaneOrientation, L> inter = intersection_2planes<T>(p1, p2);
    bool result = intersection_2triangles_intersect_planes<T>(t1, t2, inter.second);
    ASSERT_EQ(result, true);
  }

  // 6
  {
    SCOPED_TRACE("case 6");
    Tr t1 = Tr(P(-3, 0, 0), P(-4, 0, 0), P(0, 0, 2));
    Tr t2 = Tr(P(0, 0, 0), P(0, 1, 0), P(-2, 0, 0));
    Pl p1 = t1.plane_;
    Pl p2 = t2.plane_;
    std::pair<PlaneToPlaneOrientation, L> inter = intersection_2planes<T>(p1, p2);
    bool result = intersection_2triangles_intersect_planes<T>(t1, t2, inter.second);
    ASSERT_EQ(result, false);
  }

  // 7
  {
    SCOPED_TRACE("case 7");
    Tr t1 = Tr(P(-4, 0, 0), P(0, 2, 0), P(0, 0, 2));
    Tr t2 = Tr(P(0, 0, 0), P(0, 1, 0), P(-2, 0, 0));
    Pl p1 = t1.plane_;
    Pl p2 = t2.plane_;
    std::pair<PlaneToPlaneOrientation, L> inter = intersection_2planes<T>(p1, p2);
    bool result = intersection_2triangles_intersect_planes<T>(t1, t2, inter.second);
    ASSERT_EQ(result, false);
  }

  // 8
  {
    SCOPED_TRACE("case 8");
    Tr t1 = Tr(P(2, 1, 0), P(-2, -1, 0), P(0, 0, 2));
    Tr t2 = Tr(P(0, 0, 0), P(0, 1, 0), P(-2, 0, 0));
    Pl p1 = t1.plane_;
    Pl p2 = t2.plane_;
    std::pair<PlaneToPlaneOrientation, L> inter = intersection_2planes<T>(p1, p2);
    bool result = intersection_2triangles_intersect_planes<T>(t1, t2, inter.second);
    ASSERT_EQ(result, true);
  }

  // 9
  {
    SCOPED_TRACE("case 9");
    Tr t1 = Tr(P(1, 1, 0), P(-1, 1, 0), P(0, 0, 0));
    Tr t2 = Tr(P(0, 0, 0), P(0, 0, 2), P(2, 0, 2));
    Pl p1 = t1.plane_;
    Pl p2 = t2.plane_;
    std::pair<PlaneToPlaneOrientation, L> inter = intersection_2planes<T>(p1, p2);
    bool result = intersection_2triangles_intersect_planes<T>(t1, t2, inter.second);
    ASSERT_EQ(result, true);
  }

  // 10
  {
    SCOPED_TRACE("case 10");
    Tr t1 = Tr(P(1, 1, 0), P(-1, 1, 0), P(0, 0, 0));
    Tr t2 = Tr(P(-1, 0, 0), P(0, 0, 2), P(2, 0, 2));
    Pl p1 = t1.plane_;
    Pl p2 = t2.plane_;
    std::pair<PlaneToPlaneOrientation, L> inter = intersection_2planes<T>(p1, p2);
    bool result = intersection_2triangles_intersect_planes<T>(t1, t2, inter.second);
    ASSERT_EQ(result, false);
  }

  // 11
  {
    SCOPED_TRACE("case 11");
    Tr t1 = Tr(P(1, 1, 0), P(-1, 1, 0), P(0, 0, 0));
    Tr t2 = Tr(P(0, 1, 0), P(0, 0, 2), P(2, 0, 2));
    Pl p1 = t1.plane_;
    Pl p2 = t2.plane_;
    std::pair<PlaneToPlaneOrientation, L> inter = intersection_2planes<T>(p1, p2);
    bool result = intersection_2triangles_intersect_planes<T>(t1, t2, inter.second);
    ASSERT_EQ(result, true);
  }

  // 12
  {
    SCOPED_TRACE("case 12");
    Tr t1 = Tr(P(1, 1, 0), P(-1, 1, 0), P(0, 0, 0));
    Tr t2 = Tr(P(2, 1, 0), P(0, 0, 2), P(2, 0, 2));
    Pl p1 = t1.plane_;
    Pl p2 = t2.plane_;
    std::pair<PlaneToPlaneOrientation, L> inter = intersection_2planes<T>(p1, p2);
    bool result = intersection_2triangles_intersect_planes<T>(t1, t2, inter.second);
    ASSERT_EQ(result, false);
  }
}

TEST(TriangleTests, intersection_2triangles_coincident_planesTest) {
  // 1
  {
    SCOPED_TRACE("case 1");
    Tr t1 = Tr(P(0, 0, 0), P(-1, 0, 0), P(0, -1, 0));
    Tr t2 = Tr(P(-2, 0, 0), P(0, 1, 0), P(0, 2, 0));
    bool result = intersection_2triangles_coincident_planes<T>(t1, t2);
    ASSERT_EQ(result, false);
  }

  // 2
  {
    SCOPED_TRACE("case 2");
    Tr t1 = Tr(P(0, 0, 0), P(-1, 0, 0), P(0, -1, 0));
    Tr t2 = Tr(P(-1, 0, 0), P(0, 1, 0), P(0, 2, 0));
    bool result = intersection_2triangles_coincident_planes<T>(t1, t2);
    ASSERT_EQ(result, true);
  }

  // 3
  {
    SCOPED_TRACE("case 3");
    Tr t1 = Tr(P(0, 0, 0), P(-1, 0, 0), P(0, -1, 0));
    Tr t2 = Tr(P(-0.5, 0, 0), P(0, 1, 0), P(0, 2, 0));
    bool result = intersection_2triangles_coincident_planes<T>(t1, t2);
    ASSERT_EQ(result, true);
  }

  // 4
  {
    SCOPED_TRACE("case 4");
    Tr t1 = Tr(P(0, 0, 0), P(-1, 0, 0), P(0, -1, 0));
    Tr t2 = Tr(P(-3, 0, 0), P(0, -1.5, 0), P(0, -0.5, 0));
    bool result = intersection_2triangles_coincident_planes<T>(t1, t2);
    ASSERT_EQ(result, true);
  }

  // 5
  {
    SCOPED_TRACE("case 5");
    Tr t1 = Tr(P(0, 0, 0), P(-2, 0, 0), P(0, -2, 0));
    Tr t2 = Tr(P(-2, -2, 0), P(0, 1, 0), P(1, 0, 0));
    bool result = intersection_2triangles_coincident_planes<T>(t1, t2);
    ASSERT_EQ(result, true);
  }

  // 6
  {
    SCOPED_TRACE("case 6");
    Tr t1 = Tr(P(0, 0, 0), P(-2, 0, 0), P(0, -2, 0));
    Tr t2 = Tr(P(-3, 0, 0), P(0, -3, 0), P(1, 1, 0));
    bool result = intersection_2triangles_coincident_planes<T>(t1, t2);
    ASSERT_EQ(result, true);
  }

  // 7
  {
    SCOPED_TRACE("case 7");
    Tr t1 = Tr(P(0, 0, 0), P(-2, 0, 0), P(0, -2, 0));
    Tr t2 = Tr(P(-3, 0, 0), P(0, -3, 0), P(-1, -1, 0));
    bool result = intersection_2triangles_coincident_planes<T>(t1, t2);
    ASSERT_EQ(result, true);
  }

  // 8
  {
    SCOPED_TRACE("case 8");
    Tr t1 = Tr(P(0, 0, 0), P(-2, 0, 0), P(0, -2, 0));
    Tr t2 = Tr(P(-3, 0, 0), P(0, -3, 0), P(-2, -2, 0));
    bool result = intersection_2triangles_coincident_planes<T>(t1, t2);
    ASSERT_EQ(result, false);
  }
}

TEST(TriangleTests, intersection_2trianglesTest) {
  //=================================================================================================
  // Coincident

  // 1
  {
    SCOPED_TRACE("case 1");
    Tr t1 = Tr(P(0, 0, 0), P(-1, 0, 0), P(0, -1, 0));
    Tr t2 = Tr(P(-2, 0, 0), P(0, 1, 0), P(0, 2, 0));
    bool result = intersection_2triangles<T>(t1, t2);
    ASSERT_EQ(result, false);
  }

  // 2
  {
    SCOPED_TRACE("case 2");
    Tr t1 = Tr(P(0, 0, 0), P(-1, 0, 0), P(0, -1, 0));
    Tr t2 = Tr(P(-1, 0, 0), P(0, 1, 0), P(0, 2, 0));
    bool result = intersection_2triangles<T>(t1, t2);
    ASSERT_EQ(result, true);
  }

  // 3
  {
    SCOPED_TRACE("case 3");
    Tr t1 = Tr(P(0, 0, 0), P(-1, 0, 0), P(0, -1, 0));
    Tr t2 = Tr(P(-0.5, 0, 0), P(0, 1, 0), P(0, 2, 0));
    bool result = intersection_2triangles<T>(t1, t2);
    ASSERT_EQ(result, true);
  }

  // 4
  {
    SCOPED_TRACE("case 4");
    Tr t1 = Tr(P(0, 0, 0), P(-1, 0, 0), P(0, -1, 0));
    Tr t2 = Tr(P(-3, 0, 0), P(0, -1.5, 0), P(0, -0.5, 0));
    bool result = intersection_2triangles<T>(t1, t2);
    ASSERT_EQ(result, true);
  }

  // 5
  {
    SCOPED_TRACE("case 5");
    Tr t1 = Tr(P(0, 0, 0), P(-2, 0, 0), P(0, -2, 0));
    Tr t2 = Tr(P(-2, -2, 0), P(0, 1, 0), P(1, 0, 0));
    bool result = intersection_2triangles<T>(t1, t2);
    ASSERT_EQ(result, true);
  }

  // 6
  {
    SCOPED_TRACE("case 6");
    Tr t1 = Tr(P(0, 0, 0), P(-2, 0, 0), P(0, -2, 0));
    Tr t2 = Tr(P(-3, 0, 0), P(0, -3, 0), P(1, 1, 0));
    bool result = intersection_2triangles<T>(t1, t2);
    ASSERT_EQ(result, true);
  }

  // 7
  {
    SCOPED_TRACE("case 7");
    Tr t1 = Tr(P(0, 0, 0), P(-2, 0, 0), P(0, -2, 0));
    Tr t2 = Tr(P(-3, 0, 0), P(0, -3, 0), P(-1, -1, 0));
    bool result = intersection_2triangles<T>(t1, t2);
    ASSERT_EQ(result, true);
  }

  // 8
  {
    SCOPED_TRACE("case 8");
    Tr t1 = Tr(P(0, 0, 0), P(-2, 0, 0), P(0, -2, 0));
    Tr t2 = Tr(P(-3, 0, 0), P(0, -3, 0), P(-2, -2, 0));
    bool result = intersection_2triangles<T>(t1, t2);
    ASSERT_EQ(result, false);
  }

  //=================================================================================================
  // Intersection

  // 9
  {
    SCOPED_TRACE("case 9");
    Tr t1 = Tr(P(0, 0, 0), P(0, 1, 0), P(-2, 0, 0));
    Tr t2 = Tr(P(0, 0, 1), P(0, 1, 0), P(-2, 0, 0));
    bool result = intersection_2triangles<T>(t1, t2);
    ASSERT_EQ(result, true);
  }

  // 10
  {
    SCOPED_TRACE("case 10");
    Tr t1 = Tr(P(-3, 0, 0), P(0, -1, 0), P(0, 0, 1));
    Tr t2 = Tr(P(0, 0, 0), P(0, 1, 0), P(-2, 0, 0));
    bool result = intersection_2triangles<T>(t1, t2);
    ASSERT_EQ(result, false);
  }

  // 11
  {
    SCOPED_TRACE("case 11");
    Tr t1 = Tr(P(0, 0, 0), P(-3, 0, 0), P(0, 0, 2));
    Tr t2 = Tr(P(0, 0, 0), P(0, 1, 0), P(-2, 0, 0));
    bool result = intersection_2triangles<T>(t1, t2);
    ASSERT_EQ(result, true);
  }

  // 12
  {
    SCOPED_TRACE("case 12");
    Tr t1 = Tr(P(1, 0, 0), P(-3, 0, 0), P(0, 0, 2));
    Tr t2 = Tr(P(0, 0, 0), P(0, 1, 0), P(-2, 0, 0));
    bool result = intersection_2triangles<T>(t1, t2);
    ASSERT_EQ(result, true);
  }

  // 13
  {
    SCOPED_TRACE("case 13");
    Tr t1 = Tr(P(-2, 0, 0), P(-3, 0, 0), P(0, 0, 2));
    Tr t2 = Tr(P(0, 0, 0), P(0, 1, 0), P(-2, 0, 0));
    bool result = intersection_2triangles<T>(t1, t2);
    ASSERT_EQ(result, true);
  }

  // 14
  {
    SCOPED_TRACE("case 14");
    Tr t1 = Tr(P(-3, 0, 0), P(-4, 0, 0), P(0, 0, 2));
    Tr t2 = Tr(P(0, 0, 0), P(0, 1, 0), P(-2, 0, 0));
    bool result = intersection_2triangles<T>(t1, t2);
    ASSERT_EQ(result, false);
  }

  // 15
  {
    SCOPED_TRACE("case 15");
    Tr t1 = Tr(P(-4, 0, 0), P(0, 2, 0), P(0, 0, 2));
    Tr t2 = Tr(P(0, 0, 0), P(0, 1, 0), P(-2, 0, 0));
    bool result = intersection_2triangles<T>(t1, t2);
    ASSERT_EQ(result, false);
  }

  // 16
  {
    SCOPED_TRACE("case 16");
    Tr t1 = Tr(P(2, 1, 0), P(-2, -1, 0), P(0, 0, 2));
    Tr t2 = Tr(P(0, 0, 0), P(0, 1, 0), P(-2, 0, 0));
    bool result = intersection_2triangles<T>(t1, t2);
    ASSERT_EQ(result, true);
  }

  // 17
  {
    SCOPED_TRACE("case 17");
    Tr t1 = Tr(P(1, 1, 0), P(-1, 1, 0), P(0, 0, 0));
    Tr t2 = Tr(P(0, 0, 0), P(0, 0, 2), P(2, 0, 2));
    bool result = intersection_2triangles<T>(t1, t2);
    ASSERT_EQ(result, true);
  }

  // 18
  {
    SCOPED_TRACE("case 18");
    Tr t1 = Tr(P(1, 1, 0), P(-1, 1, 0), P(0, 0, 0));
    Tr t2 = Tr(P(-1, 0, 0), P(0, 0, 2), P(2, 0, 2));
    bool result = intersection_2triangles<T>(t1, t2);
    ASSERT_EQ(result, false);
  }

  // 19
  {
    SCOPED_TRACE("case 19");
    Tr t1 = Tr(P(1, 1, 0), P(-1, 1, 0), P(0, 0, 0));
    Tr t2 = Tr(P(0, 1, 0), P(0, 0, 2), P(2, 0, 2));
    bool result = intersection_2triangles<T>(t1, t2);
    ASSERT_EQ(result, true);
  }

  // 20
  {
    SCOPED_TRACE("case 20");
    Tr t1 = Tr(P(1, 1, 0), P(-1, 1, 0), P(0, 0, 0));
    Tr t2 = Tr(P(2, 1, 0), P(0, 0, 2), P(2, 0, 2));
    bool result = intersection_2triangles<T>(t1, t2);
    ASSERT_EQ(result, false);
  }

  //=================================================================================================
  // Degenerate cases

  // Segment & Segment

  // 21
  {
    SCOPED_TRACE("case 21");
    Tr t1 = Tr(P(-2, 2, 0), P(-1, 1, 0), P(0, 0, 0));
    Tr t2 = Tr(P(0, 0, 1), P(1, -1, 2), P(-2, 2, -1));
    bool result = intersection_2triangles<T>(t1, t2);
    ASSERT_EQ(result, true);
  }

  // 22
  {
    SCOPED_TRACE("case 22");
    Tr t1 = Tr(P(-2, 2, 0), P(-1, 1, 0), P(0, 0, 0));
    Tr t2 = Tr(P(0, 0, 1), P(1, -1, 2), P(0, 0, 1));
    bool result = intersection_2triangles<T>(t1, t2);
    ASSERT_EQ(result, false); // segments dont intersect
  }

  // 23
  {
    SCOPED_TRACE("case 23");
    Tr t1 = Tr(P(-2, 2, 0), P(-1, 1, 0), P(0, 0, 0));
    Tr t2 = Tr(P(0, 0, 1), P(-2, 2, -1), P(0, 0, 1));
    bool result = intersection_2triangles<T>(t1, t2);
    ASSERT_EQ(result, true);
  }

  // 24
  {
    SCOPED_TRACE("case 24");
    Tr t1 = Tr(P(-2, 2, 0), P(-1, 1, 0), P(0, 0, 0));
    Tr t2 = Tr(P(-2, 2, 1), P(-1, 1, 1), P(0, 0, 1));
    bool result = intersection_2triangles<T>(t1, t2);
    ASSERT_EQ(result, false);
  }

  // 25
  {
    SCOPED_TRACE("case 25");
    Tr t1 = Tr(P(-2, 2, 0), P(1, -1, 0), P(1, -1, 0));
    Tr t2 = Tr(P(0, 0, -1), P(0, 0, 2), P(0, 0, 2));
    bool result = intersection_2triangles<T>(t1, t2);
    ASSERT_EQ(result, true);
  }

  // Point & Point

  // 26
  {
    SCOPED_TRACE("case 26");
    Tr t1 = Tr(P(-2, 2, 0), P(-2, 2, 0), P(-2, 2, 0));
    Tr t2 = Tr(P(0, 0, 1), P(0, 0, 1), P(0, 0, 1));
    bool result = intersection_2triangles<T>(t1, t2);
    ASSERT_EQ(result, false);
  }

  // 27
  {
    SCOPED_TRACE("case 27");
    Tr t1 = Tr(P(0, 0, 1), P(0, 0, 1), P(0, 0, 1));
    Tr t2 = Tr(P(0, 0, 1), P(0, 0, 1), P(0, 0, 1));
    bool result = intersection_2triangles<T>(t1, t2);
    ASSERT_EQ(result, true);
  }

  // Segment & Point

  // 28
  {
    SCOPED_TRACE("case 28");
    Tr t1 = Tr(P(-2, 2, 0), P(-1, 1, 0), P(0, 0, 0));
    Tr t2 = Tr(P(-2, 2, 0), P(-2, 2, 0), P(-2, 2, 0));
    bool result = intersection_2triangles<T>(t1, t2);
    ASSERT_EQ(result, true);
  }

  // 29
  {
    SCOPED_TRACE("case 29");
    Tr t1 = Tr(P(-2, 2, 0), P(-1, 1, 0), P(0, 0, 0));
    Tr t2 = Tr(P(-1, 1, 0), P(-1, 1, 0), P(-1, 1, 0));
    bool result = intersection_2triangles<T>(t1, t2);
    ASSERT_EQ(result, true);
  }

  // 30
  {
    SCOPED_TRACE("case 30");
    Tr t1 = Tr(P(-2, 2, 0), P(-1, 1, 0), P(0, 0, 0));
    Tr t2 = Tr(P(1, -1, 0), P(1, -1, 0), P(1, -1, 0));
    bool result = intersection_2triangles<T>(t1, t2);
    ASSERT_EQ(result, false);
  }

  // Triangle & Point

  // 31
  {
    SCOPED_TRACE("case 31");
    Tr t1 = Tr(P(0, 0, 0), P(-1, 0, 0), P(0, 1, 0));
    Tr t2 = Tr(P(0, 0, 0), P(0, 0, 0), P(0, 0, 0));
    bool result = intersection_2triangles<T>(t1, t2);
    ASSERT_EQ(result, true);
  }

  // 32
  {
    SCOPED_TRACE("case 32");
    Tr t1 = Tr(P(0, 0, 0), P(-1, 0, 0), P(0, 1, 0));
    Tr t2 = Tr(P(0, 0, 1), P(0, 0, 1), P(0, 0, 1));
    bool result = intersection_2triangles<T>(t1, t2);
    ASSERT_EQ(result, false);
  }

  // 33
  {
    SCOPED_TRACE("case 33");
    Tr t1 = Tr(P(0, 0, 0), P(-1, 0, 0), P(0, 1, 0));
    Tr t2 = Tr(P(-0.5, 0.5, 0), P(-0.5, 0.5, 0), P(-0.5, 0.5, 0));
    bool result = intersection_2triangles<T>(t1, t2);
    ASSERT_EQ(result, true);
  }

  // Triangle & Segment

  // 34
  {
    SCOPED_TRACE("case 34");
    Tr t1 = Tr(P(0, 0, 0), P(-1, 0, 0), P(0, 1, 0));
    Tr t2 = Tr(P(-0.5, 0.5, 0), P(-0.5, 0.5, 0), P(0, 0, 1));
    bool result = intersection_2triangles<T>(t1, t2);
    ASSERT_EQ(result, true);
  }

  // 35
  {
    SCOPED_TRACE("case 35");
    Tr t1 = Tr(P(0, 0, 0), P(-1, 0, 0), P(0, 1, 0));
    Tr t2 = Tr(P(-1, 1, -1), P(-1, 1, -1), P(0, 0, 1));
    bool result = intersection_2triangles<T>(t1, t2);
    ASSERT_EQ(result, true);
  }

  // 36
  {
    SCOPED_TRACE("case 36");
    Tr t1 = Tr(P(0, 0, 0), P(-1, 0, 0), P(0, 1, 0));
    Tr t2 = Tr(P(-1, 1, -1), P(-1, 1, -1), P(0, 0, 2));
    bool result = intersection_2triangles<T>(t1, t2);
    ASSERT_EQ(result, false);
  }

  // 37
  {
    SCOPED_TRACE("case 37");
    Tr t1 = Tr(P(0, 0, 0), P(-1, 0, 0), P(0, 1, 0));
    Tr t2 = Tr(P(0, 2, 0), P(0, 2, 0), P(0, 0, 2));
    bool result = intersection_2triangles<T>(t1, t2);
    ASSERT_EQ(result, false);
  }

  // 38
  {
    SCOPED_TRACE("case 38");
    Tr t1 = Tr(P(0, 0, 0), P(-1, 0, 0), P(0, 1, 0));
    Tr t2 = Tr(P(0, 2, 0), P(0, 2, 0), P(0, -1, 0));
    bool result = intersection_2triangles<T>(t1, t2);
    ASSERT_EQ(result, true);
  }

  // 39
  {
    SCOPED_TRACE("case 39");
    Tr t1 = Tr(P(0, 0, 0), P(-1, 0, 0), P(0, 1, 0));
    Tr t2 = Tr(P(-1, -0.5, 0), P(0.5, 1, 0), P(-1, -0.5, 0));
    bool result = intersection_2triangles<T>(t1, t2);
    ASSERT_EQ(result, true);
  }

  // 40
  {
    SCOPED_TRACE("case 40");
    Tr t1 = Tr(P(0, 0, 0), P(-1, 0, 0), P(0, 1, 0));
    Tr t2 = Tr(P(-2, -0.5, 0), P(1, 1, 0), P(-2, -0.5, 0));
    bool result = intersection_2triangles<T>(t1, t2);
    ASSERT_EQ(result, true);
  }

  // 41
  {
    SCOPED_TRACE("case 41");
    Tr t1 = Tr(P(0, 0, 0), P(-1, 0, 0), P(0, 1, 0));
    Tr t2 = Tr(P(-1, 0, 0), P(0, 1, 0), P(0, 1, 0));
    bool result = intersection_2triangles<T>(t1, t2);
    ASSERT_EQ(result, true);
  }

  // 42
  {
    SCOPED_TRACE("case 42");
    Tr t1 = Tr(P(0, 0, 0), P(-1, 0, 0), P(0, 1, 0));
    Tr t2 = Tr(P(-2, 0, 0), P(-2, 0, 0), P(0, 2, 0));
    bool result = intersection_2triangles<T>(t1, t2);
    ASSERT_EQ(result, false);
  }

  // 43
  {
    SCOPED_TRACE("case 43");
    Tr t1 = Tr(P(0, 0, 0), P(-1, 0, 0), P(0, 1, 0));
    Tr t2 = Tr(P(1, 1, 0), P(0.5, 1, 0), P(-1, 1, 0));
    bool result = intersection_2triangles<T>(t1, t2);
    ASSERT_EQ(result, true);
  }

  // 44
  {
    SCOPED_TRACE("case 44");
    Tr t1 = Tr(P(0, 0, 0), P(-1, 0, 0), P(0, 1, 0));
    Tr t2 = Tr(P(1, 1, 1), P(0.5, 1, 1), P(-1, 1, 1));
    bool result = intersection_2triangles<T>(t1, t2);
    ASSERT_EQ(result, false);
  }

  // 45
  {
    SCOPED_TRACE("case 45");
    Tr t1 = Tr(P(1, 1, 0), P(3, 1, 0), P(1, 3, 0));
    Tr t2 = Tr(P(1, 1, 0), P(1, 2, 3), P(5, 4, 8));
    bool result = intersection_2triangles<T>(t1, t2);
    ASSERT_EQ(result, true);
  }
}

int main(int argc, char **argv) {
  ::testing::InitGoogleTest(&argc, argv);
  return RUN_ALL_TESTS();
}
