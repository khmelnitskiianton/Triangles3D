#ifndef _INCLUDE_OCTTREE_BOUNDING_BOX_HPP_
#define _INCLUDE_OCTTREE_BOUNDING_BOX_HPP_

#include "primitives/point.hpp"
#include "primitives/vector.hpp"

class Triangle;
class BoundingSphere;

class BoundingBox {
    Point min_;
    Point max_;

  public:
    BoundingBox() : BoundingBox(Point::zeroPoint(), Point::zeroPoint()) {}
    BoundingBox(const Point &minp, const Point &maxp) : min_(minp), max_(maxp) {}
    BoundingBox(const Vector &minp, const Vector &maxp) : min_(minp.toPoint()), max_(maxp.toPoint()) {}
    BoundingBox(const BoundingBox &rhs) : min_(rhs.min_), max_(rhs.max_) {}
    BoundingBox(const Point &a, const Point &b, const Point &c);

    ~BoundingBox() = default;
    BoundingBox &operator=(const BoundingBox &bb) = default;

    // Strict inside
    bool inside(const Point &t) const;
    bool inside(const Triangle &t) const;
    bool inside(const BoundingBox &t) const;
    bool inside(const BoundingSphere &t) const;

    inline Point getMax() const { return max_; };
    inline Point getMin() const { return min_; };

    void expandBy(const Point &p);
};

inline bool overlap(const BoundingBox &a, const BoundingBox &b) noexcept {
  const Point &amin = a.getMin();
  const Point &amax = a.getMax();
  const Point &bmin = b.getMin();
  const Point &bmax = b.getMax();

  // Пересечение по всем трём осям (границы считаем пересечением)
  return (amax.getX() >= bmin.getX() && bmax.getX() >= amin.getX() && amax.getY() >= bmin.getY() && bmax.getY() >= amin.getY() &&
          amax.getZ() >= bmin.getZ() && bmax.getZ() >= amin.getZ());
}

#endif