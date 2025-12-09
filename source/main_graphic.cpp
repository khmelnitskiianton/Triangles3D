#include <array>
#include <iostream>
#include <vector>

#include "io_wrap.hpp"
#include "octtree/octtree.hpp"
#include "primitives/point.hpp"
#include "primitives/triangle.hpp"

#include "graphic/renderer.h"

int main() {
  size_t triangles_amount = 0;
  try {
    IOWrap::GetFromInput<size_t>(&triangles_amount, std::cin);
  } catch (const std::ios_base::failure &e) {
    std::cerr << "Bad input in sizes: " << e.what() << std::endl;
    return 0;
  }
  // Add all trinagles
  std::vector<Triangle> triangles;
  try {
    for (size_t i = 0; i < triangles_amount; i++) {
      std::array<Point, 3> t_arr = {Point::badPoint(), Point::badPoint(), Point::badPoint()};
      for (size_t j = 0; j < 3; j++) {
        std::array<double, 3> coords = {NAN, NAN, NAN};
        for (size_t k = 0; k < 3; k++) {
          IOWrap::GetFromInput<double>(&coords[k], std::cin);
        }
        t_arr[j] = Point(coords[0], coords[1], coords[2]);
      }
      triangles.emplace_back(Triangle(t_arr[0], t_arr[1], t_arr[2]));
    }
  } catch (const std::ios_base::failure &e) {
    std::cerr << "Bad input in data: " << e.what() << std::endl;
    return 0;
  }
  Tree::OctTree tree{triangles};
  std::vector<size_t> triangles_intersect_indexes = tree.intersection();
  Renderer render = Renderer("shaders/direct_lighting.vs", "shaders/direct_lighting.fs");
  render.fillTrianglesBuffer(triangles, triangles_intersect_indexes);
  render.renderWindow();
  return 0;
}