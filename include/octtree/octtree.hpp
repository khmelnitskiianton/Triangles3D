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
      private:
        BoundingBox<T> region_; // bounding region
        std::vector<TriIndex> objects_; // objects
        Node *parent_ = nullptr;           // parent
        std::array<Node *, 8> child_ = {}; // child nodes
        int active_nodes_ = 0; // bit mask for active nodes

      public:
        Node() : region_(BoundingBox(Point<T>::zeroPoint(), Point<T>::zeroPoint())) {}
        Node(const BoundingBox<T> &region, const std::vector<TriIndex> &obj_list) : region_(region), objects_(obj_list) {}
        
        Node(const Node& rhs) = delete;
        Node(Node&& other) = delete;
        Node& operator=(const Node& rhs) = delete;
        Node& operator=(Node&& rhs) = delete;
        
        ~Node() = default;

        void setParent(Node *parent) { parent_ = parent; }
        Node *getChild(size_t i) { return child_[i]; }
        int getActiveNodes() { return active_nodes_; }

        friend class OctTree<T>;
      };

    /*
        Fields
    */

    std::vector<Triangle<T>> all_objects_; // all triangles

    Node *root_ = nullptr; // root

    /*
        Methods
    */
    void intersect(const Node& node , const std::vector<TriIndex> &parent_obj, std::vector<TriIndex> &result) {
      for (const TriIndex &idx_p : parent_obj) {
        for (const TriIndex &idx_l : node.objects_) {
          if (intersection_2triangles(all_objects_[idx_p], all_objects_[idx_l])) {
            result.push_back(idx_p);
            result.push_back(idx_l);
          }
        }
      }

      const size_t n = node.objects_.size();
      if (n > 1) {
        for (size_t i = 0; i < n; ++i) {
          TriIndex idx_a = node.objects_[i];
          const Triangle<T> &a = all_objects_[idx_a];
          for (size_t j = i + 1; j < n; ++j) {
            TriIndex idx_b = node.objects_[j];
            const Triangle<T> &b = all_objects_[idx_b];
            if (intersection_2triangles(a, b)) {
              result.push_back(idx_a);
              result.push_back(idx_b);
            }
          }
        }
      }

      if (node.active_nodes_ == 0) {
        return;
      }

      // New list for children
      std::vector<TriIndex> child_parent;
      child_parent.reserve(parent_obj.size() + node.objects_.size());
      child_parent.insert(child_parent.end(), parent_obj.begin(), parent_obj.end());
      child_parent.insert(child_parent.end(), node.objects_.begin(), node.objects_.end());

      for (int flags = node.active_nodes_, index = 0; flags > 0; flags >>= 1, index++) {
        if ((flags & 1) == 1) {
          intersect(*node.child_[index], child_parent, result);
        }
      }
    }

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

    void buildTree(Node& node) {
      // End of recursion
      if (node.objects_.size() <= 1)
        return;
      if (node.objects_.size() <= MIN_PRIMS_PER_LEAF)
        return;
      Vector dimensions = node.region_.max_ - node.region_.min_;
      // Check minimum size of BB
      if (dimensions.x_ <= SIDE_MIN_SIZE && dimensions.y_ <= SIDE_MIN_SIZE && dimensions.z_ <= SIDE_MIN_SIZE) {
        return;
      }
      Vector half = dimensions / 2.0;
      Vector center = node.region_.min_.toVector() + half;

      // Create parts of every octant
      std::array<BoundingBox<T>, 8> octant = {};
      octant[0] = BoundingBox(node.region_.min_.toVector(), center);
      octant[1] =
          BoundingBox(Vector(center.x_, node.region_.min_.y_, node.region_.min_.z_), Vector(node.region_.max_.x_, center.y_, center.z_));
      octant[2] =
          BoundingBox(Vector(center.x_, node.region_.min_.y_, center.z_), Vector(node.region_.max_.x_, center.y_, node.region_.max_.z_));
      octant[3] =
          BoundingBox(Vector(node.region_.min_.x_, node.region_.min_.y_, center.z_), Vector(center.x_, center.y_, node.region_.max_.z_));
      octant[4] =
          BoundingBox(Vector(node.region_.min_.x_, center.y_, node.region_.min_.z_), Vector(center.x_, node.region_.max_.y_, center.z_));
      octant[5] =
          BoundingBox(Vector(center.x_, center.y_, node.region_.min_.z_), Vector(node.region_.max_.x_, node.region_.max_.y_, center.z_));
      octant[6] = BoundingBox(center, node.region_.max_.toVector());
      octant[7] =
          BoundingBox(Vector(node.region_.min_.x_, center.y_, center.z_), Vector(center.x_, node.region_.max_.y_, node.region_.max_.z_));

      std::array<std::vector<TriIndex>, 8> oct_idxes;
      // Check which obj goes to which octant
      std::vector<TriIndex> stay;
      stay.reserve(node.objects_.size());

      for (auto idx : node.objects_) {
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
      node.objects_.swap(stay);

      // Create child nodes
      for (size_t a = 0; a < 8; a++) {
        if (!oct_idxes[a].empty()) {
          node.child_[a] = new Node(octant[a], std::move(oct_idxes[a]));
          node.child_[a]->setParent(&node);
          node.active_nodes_ |= (int)(1 << a);
          buildTree(*node.child_[a]);
        }
      }
    }

  public:
    template <typename It> OctTree(It first, It last) {
      for (auto it = first; it != last; ++it) {
        all_objects_.push_back(*it);
      }
      std::vector<TriIndex> indices(all_objects_.size());
      std::iota(indices.begin(), indices.end(), 0);
      root_ = new Node(rootBox(), indices);
      buildTree(*root_);
    }
    template <typename C> OctTree(const C &container) : OctTree(std::begin(container), std::end(container)) {}
    
    OctTree(const OctTree& rhs) = delete;
    OctTree(OctTree&& rhs) = delete;
    OctTree& operator=(const OctTree& rhs) = delete;
    OctTree& operator=(OctTree&& rhs) = delete;

    ~OctTree() {   
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

    std::vector<size_t> intersection() {
      std::vector<TriIndex> empty_list;
      std::vector<TriIndex> result;
      intersect(*root_, empty_list, result);
      std::sort(result.begin(), result.end());
      result.erase(std::unique(result.begin(), result.end()), result.end());
      return result;
    }
};
} // namespace Tree

#endif