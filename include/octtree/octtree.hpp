#ifndef _INCLUDE_OCTTREE_OCTTREE_HPP
#define _INCLUDE_OCTTREE_OCTTREE_HPP

#include "octtree/bounding_box.hpp"
#include "primitives/triangle.hpp"
#include <algorithm>
#include <array>
#include <list>
#include <numeric>
#include <set>
#include <vector>

namespace Tree {
class OctTree {
    // 1x1x1 with powers of 2(to divide by 2)
    static constexpr int SIDE_MIN_SIZE = 1;
    static constexpr int MIN_PRIMS_PER_LEAF = 8;
    using TriIndex = size_t;
    class Node {
        BoundingBox region_; // bounding region
        const std::vector<Triangle> &all_objects_;
        std::vector<TriIndex> objects_; // objects

        std::array<Node *, 8> child_ = {}; // child nodes
        Node *parent_ = nullptr;           // parent

        int active_nodes_ = 0; // bit mask for active nodes

      public:
        Node(const std::vector<Triangle> &all_objects)
            : region_(BoundingBox(Point::zeroPoint(), Point::zeroPoint())), all_objects_(all_objects) {}
        Node(const BoundingBox &region, const std::vector<Triangle> &all_objects, const std::vector<TriIndex> &obj_list)
            : region_(region), all_objects_(all_objects), objects_(obj_list) {}

        inline void setParent(Node *parent) { parent_ = parent; }
        inline Node *getChild(size_t i) { return child_[i]; }
        inline int getActiveNodes() { return active_nodes_; }

        Node *createNode(const BoundingBox &region, const std::vector<TriIndex> &obj_list);
        void buildTree();
        void intersect(const std::vector<TriIndex> &parent_obj, std::vector<TriIndex> &result);
    };

    std::vector<Triangle> all_objects_;

    Node *root_ = nullptr;

    BoundingBox rootBox();
    void freeTree();

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