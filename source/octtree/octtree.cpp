#include "octtree/octtree.hpp"
#include "common.hpp"
#include "octtree/bounding_box.hpp"
#include "primitives/triangle.hpp"
#include "primitives/vector.hpp"

#include <array>
#include <stack>
#include <vector>

using namespace Tree;

void OctTree::Node::buildTree() {
  // End of recursion
  if (objects_.size() <= 1)
    return;
  if (objects_.size() <= MIN_PRIMS_PER_LEAF)
    return;
  Vector dimensions = region_.getMax() - region_.getMin();
  // Check minimum size of BB
  if (dimensions.getX() <= SIDE_MIN_SIZE && dimensions.getY() <= SIDE_MIN_SIZE && dimensions.getZ() <= SIDE_MIN_SIZE) {
    return;
  }
  Vector half = dimensions / 2.0;
  Vector center = region_.getMin().toVector() + half;

  // Create parts of every octant
  std::array<BoundingBox, 8> octant = {};
  octant[0] = BoundingBox(region_.getMin().toVector(), center);
  octant[1] = BoundingBox(Vector(center.getX(), region_.getMin().getY(), region_.getMin().getZ()),
                          Vector(region_.getMax().getX(), center.getY(), center.getZ()));
  octant[2] = BoundingBox(Vector(center.getX(), region_.getMin().getY(), center.getZ()),
                          Vector(region_.getMax().getX(), center.getY(), region_.getMax().getZ()));
  octant[3] = BoundingBox(Vector(region_.getMin().getX(), region_.getMin().getY(), center.getZ()),
                          Vector(center.getX(), center.getY(), region_.getMax().getZ()));
  octant[4] = BoundingBox(Vector(region_.getMin().getX(), center.getY(), region_.getMin().getZ()),
                          Vector(center.getX(), region_.getMax().getY(), center.getZ()));
  octant[5] = BoundingBox(Vector(center.getX(), center.getY(), region_.getMin().getZ()),
                          Vector(region_.getMax().getX(), region_.getMax().getY(), center.getZ()));
  octant[6] = BoundingBox(center, region_.getMax().toVector());
  octant[7] = BoundingBox(Vector(region_.getMin().getX(), center.getY(), center.getZ()),
                          Vector(center.getX(), region_.getMax().getY(), region_.getMax().getZ()));

  std::array<std::vector<TriIndex>, 8> oct_idxes;
  // Check which obj goes to which octant
  std::vector<TriIndex> stay;
  stay.reserve(objects_.size());

  for (auto idx : objects_) {
    const Triangle &t = all_objects_[idx];

    bool down = false;
    for (size_t a = 0; a < 8; a++) {
      if (t.getBox().getMin() != t.getBox().getMax()) {
        if (octant[a].inside(t.getBox())) {
          oct_idxes[a].push_back(idx);
          down = true;
          break;
        }
      } else if (!approx_zero(t.getBoundingSphere().getRadius())) {
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

BoundingBox OctTree::rootBox() {
  BoundingBox box = {};
  // Find limited box for every triangles
  for (const Triangle &t : all_objects_) {
    box.expandBy(t.getA());
    box.expandBy(t.getB());
    box.expandBy(t.getC());
  }
  return box;
}

void OctTree::Node::intersect(const std::vector<TriIndex> &parent_obj, std::vector<TriIndex> &result) {
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
      const Triangle &a = all_objects_[idx_a];
      for (size_t j = i + 1; j < n; ++j) {
        TriIndex idx_b = objects_[j];
        const Triangle &b = all_objects_[idx_b];
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

void OctTree::freeTree() {
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