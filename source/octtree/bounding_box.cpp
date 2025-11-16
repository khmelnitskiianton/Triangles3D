#include "octtree/bounding_box.hpp"
#include "common.hpp"
#include "octtree/bounding_sphere.hpp"
#include "primitives/triangle.hpp"

BoundingBox::BoundingBox(const Point &a, const Point &b, const Point &c) {
  min_ = Point(std::min(std::min(a.getX(), b.getX()), c.getX()), std::min(std::min(a.getY(), b.getY()), c.getY()),
               std::min(std::min(a.getZ(), b.getZ()), c.getZ()));
  max_ = Point(std::max(std::max(a.getX(), b.getX()), c.getX()), std::max(std::max(a.getY(), b.getY()), c.getY()),
               std::max(std::max(a.getZ(), b.getZ()), c.getZ()));
}

bool BoundingBox::inside(const Point &p) const {
  return ge_approx(p.getX(), min_.getX()) && le_approx(p.getX(), max_.getX()) && ge_approx(p.getY(), min_.getY()) &&
         le_approx(p.getY(), max_.getY()) && ge_approx(p.getZ(), min_.getZ()) && le_approx(p.getZ(), max_.getZ());
}

bool BoundingBox::inside(const Triangle &t) const { return inside(t.getA()) && inside(t.getB()) && inside(t.getC()); }

bool BoundingBox::inside(const BoundingBox &bb) const {
  return ge_approx(bb.min_.getX(), min_.getX()) && le_approx(bb.max_.getX(), max_.getX()) &&
         ge_approx(bb.min_.getY(), min_.getY()) && le_approx(bb.max_.getY(), max_.getY()) &&
         ge_approx(bb.min_.getZ(), min_.getZ()) && le_approx(bb.max_.getZ(), max_.getZ());
}

bool BoundingBox::inside(const BoundingSphere &bs) const {
  return ge_approx(bs.getCenter().getX() - bs.getRadius(), min_.getX()) &&
         le_approx(bs.getCenter().getX() + bs.getRadius(), max_.getX()) &&
         ge_approx(bs.getCenter().getY() - bs.getRadius(), min_.getY()) &&
         le_approx(bs.getCenter().getY() + bs.getRadius(), max_.getY()) &&
         ge_approx(bs.getCenter().getZ() - bs.getRadius(), min_.getZ()) &&
         le_approx(bs.getCenter().getZ() + bs.getRadius(), max_.getZ());
}

void BoundingBox::expandBy(const Point &p) {
  if (!std::isfinite(p.getX()) || !std::isfinite(p.getY()) || !std::isfinite(p.getZ()))
    return;

  min_.setX(std::min(min_.getX(), p.getX()));
  min_.setY(std::min(min_.getY(), p.getY()));
  min_.setZ(std::min(min_.getZ(), p.getZ()));

  max_.setX(std::max(max_.getX(), p.getX()));
  max_.setY(std::max(max_.getY(), p.getY()));
  max_.setZ(std::max(max_.getZ(), p.getZ()));
}