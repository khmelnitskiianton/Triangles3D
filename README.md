# Triangles3D

<p align="center">
  <a href="https://github.com/khmelnitskiianton/Triangles3D/actions">
    <img src="https://img.shields.io/github/actions/workflow/status/khmelnitskiianton/Triangles3D/ci.yml?style=for-the-badge"
         alt="Build Status" />
  </a>&emsp;
  <a href="#">
    <img src="https://img.shields.io/endpoint?url=https%3A%2F%2Fraw.githubusercontent.com%2Fkhmelnitskiianton%2FTriangles3D%2Fmain%2F.github%2Fbadge%2Fmipt-badge.json&style=for-the-badge"
         alt="MIPT" />
  </a>&emsp;
  <a href="LICENSE">
    <img src="https://img.shields.io/github/license/khmelnitskiianton/Triangles3D?style=for-the-badge"
         alt="License" />
  </a>
</p>

<p align="center">
  <img src="https://img.shields.io/badge/C++-00599C?style=for-the-badge&logo=c%2B%2B&logoColor=white" alt="C++" />&emsp;
  <img src="https://img.shields.io/badge/CMake-064F8C?style=for-the-badge&logo=cmake&logoColor=white" alt="CMake" />&emsp;
  <img src="https://img.shields.io/badge/Markdown-000000?style=for-the-badge&logo=markdown&logoColor=white" alt="Markdown" />
</p>

Project of simulation 3D triangles intersection.

Docs for geometry, BVH and speed up tips in [docs](docs/)

## Dependencies

Compiler c/c++, cmake, python, gtest

```shell
apt-get install build-essential clang ninja-build make cmake python3
apt-get install libgtest-dev libgmock-dev libtbb-dev
```

## Building

Using CMake, you can specify 2 options:
- `SANITIZE`: enables sanitizers for gcc/clang compilers.
- `BUILD_TESTS`: build unit tests with GTest.

### Config

*Debug:*
```shell
cmake -G Ninja -DCMAKE_BUILD_TYPE=Debug -DCMAKE_EXPORT_COMPILE_COMMANDS=ON -DBUILD_TESTS=ON -DSANITIZE=ON -S . -B build
```

*Release:*
```shell
cmake -G Ninja -DCMAKE_BUILD_TYPE=Release -S . -B build
```

### Build
```shell
cmake --build build
```

Binaries are located in `build/bin/`

### Install 

```shell
cmake --install build --prefix ~/my_software
```

## Usage

Format of input (stdin): amount of triangles, next for each triangle pass 3 point, each point - 3 double

```
8
1 1 0 3 1 0 1 3 0
0 0 0 1 0 0 0 1 0
1 0.5 0 1 0.5 1 0 0 0.5
1 0 0 0 1 0 0 0 1
0 0 0 0 3 3 0 0 3
1 1 0 1 2 3 5 4 8
9 9 9 9 9 9 9 9 9
8 8 8 8 8 8 -10 8 8
```

Format of output: id's of triangles that has intersection (each on new line)

```
0
1
2
3
4
5
```

## Tests

Google Tests, for run do:

```shell
ctest --test-dir build/tests/unittests --output-on-failure
```

Python Tests, for run do:

```shell
python3 tests/e2e/run.py --bin build/bin/triangles
```

Useful archive with tests set and script for run triangles - `tests/e2e.tar.gz`

## Project Structure

```
Triangles3D/
├── CMakeLists.txt
├── docs
│   ├── GeometryToolsForComputerGraphics.pdf
│   ├── README.md
│   └── TriangleIntersectionAlgorithm.pdf
├── include
│   ├── common.hpp
│   ├── io_wrap.hpp
│   ├── octtree
│   │   ├── bounding_box.hpp
│   │   ├── bounding_sphere.hpp
│   │   └── octtree.hpp
│   └── primitives
│       ├── line.hpp
│       ├── plane.hpp
│       ├── point.hpp
│       ├── segment.hpp
│       ├── triangle.hpp
│       └── vector.hpp
├── LICENSE
├── README.md
├── source
│   ├── CMakeLists.txt
│   ├── main.cpp
│   ├── octtree
│   │   ├── bounding_box.cpp
│   │   ├── bounding_sphere.cpp
│   │   └── octtree.cpp
│   └── primitives
│       ├── line.cpp
│       ├── plane.cpp
│       ├── point.cpp
│       ├── segment.cpp
│       ├── triangle.cpp
│       └── vector.cpp
└── tests
    ├── e2e
    │   ├── keys
    │   ├── run.py
    │   └── tests
    └── unittests
        ├── CMakeLists.txt
        └── unittests.cpp
```

## Workflow

Pipeline runs in 2 stages:
- `build`: debug build project & create artifact.
- `tests`: run tests for build
    + `UnitTests`: tests/unittests run with GTest
    + `End2EndTests`: tests/e2e run with Python