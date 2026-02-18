#ifndef _INCLUDE_OCTTREE_BOUNDING_BOX_HPP_
#define _INCLUDE_OCTTREE_BOUNDING_BOX_HPP_

#include "primitives/point.hpp"
#include "primitives/vector.hpp"

template <typename T> struct Triangle;

template <typename T> struct BoundingSphere;

template <typename T> struct BoundingBox {
    Point<T> min_;
    Point<T> max_;

    BoundingBox() : BoundingBox(Point<T>::zeroPoint(), Point<T>::zeroPoint()) {}
    BoundingBox(const Point<T> &minp, const Point<T> &maxp) : min_(minp), max_(maxp) {}
    BoundingBox(const Vector<T> &minp, const Vector<T> &maxp) : min_(minp.toPoint()), max_(maxp.toPoint()) {}
    BoundingBox(const BoundingBox &rhs) : min_(rhs.min_), max_(rhs.max_) {}
    BoundingBox(const Point<T> &a, const Point<T> &b, const Point<T> &c) {
      min_ =
          Point(std::min(std::min(a.x_, b.x_), c.x_), std::min(std::min(a.y_, b.y_), c.y_), std::min(std::min(a.z_, b.z_), c.z_));
      max_ =
          Point(std::max(std::max(a.x_, b.x_), c.x_), std::max(std::max(a.y_, b.y_), c.y_), std::max(std::max(a.z_, b.z_), c.z_));
    }

    ~BoundingBox() = default;
    BoundingBox &operator=(const BoundingBox &bb) = default;

    // Strict inside
    bool inside(const Point<T> &p) const {
      return ge_approx(p.x_, min_.x_) && le_approx(p.x_, max_.x_) && ge_approx(p.y_, min_.y_) && le_approx(p.y_, max_.y_) &&
             ge_approx(p.z_, min_.z_) && le_approx(p.z_, max_.z_);
    }

    bool inside(const Triangle<T> &t) const { return inside(t.a_) && inside(t.b_) && inside(t.c_); }

    bool inside(const BoundingBox &bb) const {
      return ge_approx(bb.min_.x_, min_.x_) && le_approx(bb.max_.x_, max_.x_) && ge_approx(bb.min_.y_, min_.y_) &&
             le_approx(bb.max_.y_, max_.y_) && ge_approx(bb.min_.z_, min_.z_) && le_approx(bb.max_.z_, max_.z_);
    }

    bool inside(const BoundingSphere<T> &bs) const {
      return ge_approx(bs.center_.x_ - bs.radius_, min_.x_) && le_approx(bs.center_.x_ + bs.radius_, max_.x_) &&
             ge_approx(bs.center_.y_ - bs.radius_, min_.y_) && le_approx(bs.center_.y_ + bs.radius_, max_.y_) &&
             ge_approx(bs.center_.z_ - bs.radius_, min_.z_) && le_approx(bs.center_.z_ + bs.radius_, max_.z_);
    }

    void expandBy(const Point<T> &p) {
      if (!std::isfinite(p.x_) || !std::isfinite(p.y_) || !std::isfinite(p.z_))
        return;

      min_.x_ = std::min(min_.x_, p.x_);
      min_.y_ = std::min(min_.y_, p.y_);
      min_.z_ = std::min(min_.z_, p.z_);

      max_.x_ = std::max(max_.x_, p.x_);
      max_.y_ = std::max(max_.y_, p.y_);
      max_.z_ = std::max(max_.z_, p.z_);
    }
};

template <typename T> inline bool overlap(const BoundingBox<T> &a, const BoundingBox<T> &b) noexcept {
  const Point<T> &amin = a.min_;
  const Point<T> &amax = a.max_;
  const Point<T> &bmin = b.min_;
  const Point<T> &bmax = b.max_;

  // Пересечение по всем трём осям (границы считаем пересечением)
  return (amax.x_ >= bmin.x_ && bmax.x_ >= amin.x_ && amax.y_ >= bmin.y_ && bmax.y_ >= amin.y_ && amax.z_ >= bmin.z_ &&
          bmax.z_ >= amin.z_);
}

#endif