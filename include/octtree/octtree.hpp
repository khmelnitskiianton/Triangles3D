#ifndef _INCLUDE_OCTTREE_OCTTREE_HPP
#define _INCLUDE_OCTTREE_OCTTREE_HPP

#include "octtree/bounding_box.hpp"
#include "primitives/triangle.hpp"
#include <algorithm>
#include <array>
#include <list>
#include <numeric>
#include <set>
#include <stack>
#include <vector>

namespace Tree {
template <typename T> class OctTree {
    // 1x1x1 with powers of 2(to divide by 2)
    static constexpr int SIDE_MIN_SIZE = 1;
    static constexpr int MIN_PRIMS_PER_LEAF = 64;
    using TriIndex = size_t;
    class Node {
        BoundingBox<T> region_; // bounding region
        const std::vector<Triangle<T>> &all_objects_;
        std::vector<TriIndex> objects_; // objects

        std::array<Node *, 8> child_ = {}; // child nodes
        Node *parent_ = nullptr;           // parent

        int active_nodes_ = 0; // bit mask for active nodes

      public:
        Node(const std::vector<Triangle<T>> &all_objects)
            : region_(BoundingBox(Point<T>::zeroPoint(), Point<T>::zeroPoint())), all_objects_(all_objects) {}
        Node(const BoundingBox<T> &region, const std::vector<Triangle<T>> &all_objects, const std::vector<TriIndex> &obj_list)
            : region_(region), all_objects_(all_objects), objects_(obj_list) {}

        void setParent(Node *parent) { parent_ = parent; }
        Node *getChild(size_t i) { return child_[i]; }
        int getActiveNodes() { return active_nodes_; }

        Node *createNode(const BoundingBox<T> &region, const std::vector<TriIndex> &obj_list);

        void buildTree() {
          // End of recursion
          if (objects_.size() <= 1)
            return;
          if (objects_.size() <= MIN_PRIMS_PER_LEAF)
            return;
          Vector dimensions = region_.max_ - region_.min_;
          // Check minimum size of BB
          if (dimensions.x_ <= SIDE_MIN_SIZE && dimensions.y_ <= SIDE_MIN_SIZE && dimensions.z_ <= SIDE_MIN_SIZE) {
            return;
          }
          Vector half = dimensions / 2.0;
          Vector center = region_.min_.toVector() + half;

          // Create parts of every octant
          std::array<BoundingBox<T>, 8> octant = {};
          octant[0] = BoundingBox(region_.min_.toVector(), center);
          octant[1] =
              BoundingBox(Vector(center.x_, region_.min_.y_, region_.min_.z_), Vector(region_.max_.x_, center.y_, center.z_));
          octant[2] =
              BoundingBox(Vector(center.x_, region_.min_.y_, center.z_), Vector(region_.max_.x_, center.y_, region_.max_.z_));
          octant[3] =
              BoundingBox(Vector(region_.min_.x_, region_.min_.y_, center.z_), Vector(center.x_, center.y_, region_.max_.z_));
          octant[4] =
              BoundingBox(Vector(region_.min_.x_, center.y_, region_.min_.z_), Vector(center.x_, region_.max_.y_, center.z_));
          octant[5] =
              BoundingBox(Vector(center.x_, center.y_, region_.min_.z_), Vector(region_.max_.x_, region_.max_.y_, center.z_));
          octant[6] = BoundingBox(center, region_.max_.toVector());
          octant[7] =
              BoundingBox(Vector(region_.min_.x_, center.y_, center.z_), Vector(center.x_, region_.max_.y_, region_.max_.z_));

          std::array<std::vector<TriIndex>, 8> oct_idxes;
          // Check which obj goes to which octant
          std::vector<TriIndex> stay;
          stay.reserve(objects_.size());

          for (auto idx : objects_) {
            const Triangle<T> &t = all_objects_[idx];

            bool down = false;
            for (size_t a = 0; a < 8; a++) {
              if (t.box_.min_ != t.box_.max_) {
                if (octant[a].inside(t.box_)) {
                  oct_idxes[a].push_back(idx);
                  down = true;
                  break;
                }
              } else if (!approx_zero(t.getBoundingSphere().radius_)) {
                if (octant[a].inside(t.getBoundingSphere())) {
                  oct_idxes[a].push_back(idx);
                  down = true;
                  break;
                }
              }
            }

            if (!down) {
              stay.push_back(idx);
            }
          }

          // Change current vector with nodes that stays in it
          objects_.swap(stay);

          // Create child nodes
          for (size_t a = 0; a < 8; a++) {
            if (!oct_idxes[a].empty()) {
              child_[a] = new Node(octant[a], all_objects_, std::move(oct_idxes[a]));
              child_[a]->setParent(this);
              active_nodes_ |= (int)(1 << a);
              child_[a]->buildTree();
            }
          }
        }

        void intersect(const std::vector<TriIndex> &parent_obj, std::vector<TriIndex> &result) {
          for (const TriIndex &idx_p : parent_obj) {
            for (const TriIndex &idx_l : objects_) {
              if (intersection_2triangles(all_objects_[idx_p], all_objects_[idx_l])) {
                result.push_back(idx_p);
                result.push_back(idx_l);
              }
            }
          }

          const size_t n = objects_.size();
          if (n > 1) {
            for (size_t i = 0; i < n; ++i) {
              TriIndex idx_a = objects_[i];
              const Triangle<T> &a = all_objects_[idx_a];
              for (size_t j = i + 1; j < n; ++j) {
                TriIndex idx_b = objects_[j];
                const Triangle<T> &b = all_objects_[idx_b];
                if (intersection_2triangles(a, b)) {
                  result.push_back(idx_a);
                  result.push_back(idx_b);
                }
              }
            }
          }

          if (active_nodes_ == 0) {
            return;
          }

          // New list for children
          std::vector<TriIndex> child_parent;
          child_parent.reserve(parent_obj.size() + objects_.size());
          child_parent.insert(child_parent.end(), parent_obj.begin(), parent_obj.end());
          child_parent.insert(child_parent.end(), objects_.begin(), objects_.end());

          for (int flags = active_nodes_, index = 0; flags > 0; flags >>= 1, index++) {
            if ((flags & 1) == 1) {
              child_[index]->intersect(child_parent, result);
            }
          }
        }
    };

    // Contains

    std::vector<Triangle<T>> all_objects_; // all triangles

    Node *root_ = nullptr; // root

    BoundingBox<T> rootBox() {
      BoundingBox<T> box = {};
      // Find limited box for every triangles
      for (const Triangle<T> &t : all_objects_) {
        box.expandBy(t.a_);
        box.expandBy(t.b_);
        box.expandBy(t.c_);
      }
      return box;
    }

    void freeTree() {
      if (root_ == nullptr)
        return;

      std::stack<Node *> node_stack;
      node_stack.push(root_);
      while (!node_stack.empty()) {
        Node *delete_node = node_stack.top();
        node_stack.pop();

        for (int flags = delete_node->getActiveNodes(), index = 0; flags > 0; flags >>= 1, index++) {
          if ((flags & 1) == 1) {
            if (delete_node->getChild(index) != nullptr) {
              node_stack.push(delete_node->getChild(index));
            }
          }
        }
        delete delete_node;
      }
    }

  public:
    template <typename It> OctTree(It first, It last) {
      for (auto it = first; it != last; ++it) {
        all_objects_.push_back(*it);
      }
      std::vector<TriIndex> indices(all_objects_.size());
      std::iota(indices.begin(), indices.end(), 0);
      root_ = new Node(rootBox(), all_objects_, indices);
      root_->buildTree();
    }
    template <typename C> OctTree(const C &container) : OctTree(std::begin(container), std::end(container)) {}

    ~OctTree() { freeTree(); }

    std::vector<size_t> intersection() {
      std::vector<TriIndex> empty_list;
      std::vector<TriIndex> result;
      root_->intersect(empty_list, result);
      std::sort(result.begin(), result.end());
      result.erase(std::unique(result.begin(), result.end()), result.end());
      return result;
    }
};
} // namespace Tree

#endif