# Triangles3D

[![MIPT](https://img.shields.io/endpoint?style=plastic&url=https%3A%2F%2Fraw.githubusercontent.com%2Fkhmelnitskiianton%2FTriangles3D%2Fmain%2F.github%2Fbadge%2Fmipt-badge.json)](#)

[![License](https://img.shields.io/github/license/khmelnitskiianton/mega-humidifier)](#)
[![GitHub Actions](https://img.shields.io/badge/GitHub_Actions-2088FF?logo=github-actions&logoColor=white)](#)

[![C++](https://img.shields.io/badge/C++-%2300599C.svg?logo=c%2B%2B&logoColor=white)](#)
[![CMake](https://img.shields.io/badge/CMake-064F8C?logo=cmake&logoColor=fff)](#)
[![Markdown](https://img.shields.io/badge/Markdown-%23000000.svg?logo=markdown&logoColor=white)](#)

Docs for geometry in [docs](docs/)

## Dependencies

Compiler c/c++(clang preferable), cmake, python, gtest

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
cmake -G Ninja -DCMAKE_BUILD_TYPE=Debug -DCMAKE_EXPORT_COMPILE_COMMANDS=ON -DBUILD_TESTS=ON -DSANITIZE=ON -DCMAKE_CXX_COMPILER=clang++ -S . -B build
```

*Release:*
```shell
cmake -G Ninja -DCMAKE_BUILD_TYPE=Release -DCMAKE_CXX_COMPILER=clang++ -S . -B build
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

## Workflow

Pipeline runs in 2 stages:
- `build`: debug build project & create artifact.
- `tests`: run tests for build
    + `UnitTests`: tests/unittests run with GTest
    + `End2EndTests`: tests/e2e run with Python