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
    bool isBad() const;

    Point getA() const { return a_; }
    Point getB() const { return b_; }

    Line findLine() const { return Line(a_, b_); }

    bool isInclude(const Point &p) const;

    bool isDegenerate() const;

    void print(std::ostream &out) const;
};

bool equal(const Segment &s1, const Segment &s2);

/// Belongs to one line!
bool intersection_2segments(const Segment &s1, const Segment &s2);

#endif