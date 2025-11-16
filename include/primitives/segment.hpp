#ifndef _INCLUDE_PRIMITIVES_SEGMENT_HPP_
#define _INCLUDE_PRIMITIVES_SEGMENT_HPP_

#include "primitives/line.hpp"
#include "primitives/point.hpp"

// ab segment
class Segment {
    Point a_;
    Point b_;

  public:
    Segment(const Point &a, const Point &b) : a_(a), b_(b) {}
    Segment(const Segment &s) : a_(s.a_), b_(s.b_) {}

    static Segment badSegment();
    inline bool isBad() const noexcept { return a_.isBad() || b_.isBad(); }

    inline Point getA() const noexcept { return a_; }
    inline Point getB() const noexcept { return b_; }

    inline Line findLine() const noexcept { return Line(a_, b_); }

    bool isInclude(const Point &p) const noexcept;

    bool isDegenerate() const noexcept { return a_ == b_; }
    void print(std::ostream &out) const noexcept;
};

bool equal(const Segment &s1, const Segment &s2) noexcept;

/// Intersection of 2 segments lying on one line(also degenerated cases)
bool intersection_2segments_on_line(const Segment &s1, const Segment &s2);
/// Intersection of 2 segments in space(also degenerated cases)
bool intersection_2segments(const Segment &s1, const Segment &s2);

#endif