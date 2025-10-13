#include <array>
#include <iostream>
#include <set>
#include <vector>

#include "io_wrap.hpp"
#include "primitives/line.hpp"
#include "primitives/plane.hpp"
#include "primitives/point.hpp"
#include "primitives/segment.hpp"
#include "primitives/vector.hpp"
#include "triangle.hpp"


int main() {
  size_t triangles_amount = 0;
  try {
    IOWrap::GetFromInput<size_t>(&triangles_amount, std::cin);
  } catch (const std::ios_base::failure &e) {
    std::cerr << "Bad input in sizes: " << e.what() << std::endl;
    return 0;
  }
  // Add all trinagles
  std::vector<Triangle> triangles_set;
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
      triangles_set.emplace_back(Triangle(t_arr[0], t_arr[1], t_arr[2]));
    }
  } catch (const std::ios_base::failure &e) {
    std::cerr << "Bad input in data: " << e.what() << std::endl;
    return 0;
  }
  std::set<size_t> triangles_intersect_indexes;
  for (size_t i = 0; i < triangles_amount; ++i) {
    const auto &A = triangles_set[i];
    for (size_t j = i + 1; j < triangles_amount; ++j) {
      const auto &B = triangles_set[j];
      if (intersection_2triangles(A, B)) {
        triangles_intersect_indexes.insert(i);
        triangles_intersect_indexes.insert(j);
      }
    }
  }
  for (auto it = triangles_intersect_indexes.begin(); it != triangles_intersect_indexes.end(); ++it) {
    std::cout << *it << std::endl;
  }
  return 0;
}