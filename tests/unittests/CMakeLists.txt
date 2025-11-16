enable_testing()
find_package(GTest REQUIRED)
include(GoogleTest)

add_executable(unittests 
  unittests.cpp
  # Geometry
  # Primitives
  ${PROJECT_SOURCE_DIR}/source/octtree/bounding_box.cpp
  ${PROJECT_SOURCE_DIR}/source/octtree/bounding_sphere.cpp

  ${PROJECT_SOURCE_DIR}/source/primitives/triangle.cpp
  ${PROJECT_SOURCE_DIR}/source/primitives/vector.cpp
  ${PROJECT_SOURCE_DIR}/source/primitives/plane.cpp
  ${PROJECT_SOURCE_DIR}/source/primitives/point.cpp
  ${PROJECT_SOURCE_DIR}/source/primitives/line.cpp
  ${PROJECT_SOURCE_DIR}/source/primitives/segment.cpp
)

target_include_directories(unittests PUBLIC "${PROJECT_SOURCE_DIR}/include")
target_link_libraries(
    unittests 
    PRIVATE
    GTest::gtest
)

if (MSVC)
    target_compile_options(unittests PRIVATE
        # Warnings
        /W4
        /WX
    )
else()
    target_compile_options(unittests PRIVATE
        # Warnings
        -Wall 
        -Wextra 
        -Werror
        # Sanitizers
        $<$<BOOL:${SANITIZE}>:-fsanitize=address,undefined,leak,alignment,bool,bounds,enum,float-cast-overflow,float-divide-by-zero,integer-divide-by-zero,nonnull-attribute,null,object-size,return,returns-nonnull-attribute,shift,signed-integer-overflow,unreachable,vla-bound,vptr>
    )
    target_link_options(unittests PRIVATE
        # Sanitizers
        $<$<BOOL:${SANITIZE}>:-fsanitize=address,undefined,leak,alignment,bool,bounds,enum,float-cast-overflow,float-divide-by-zero,integer-divide-by-zero,nonnull-attribute,null,object-size,return,returns-nonnull-attribute,shift,signed-integer-overflow,unreachable,vla-bound,vptr>
    )
endif()

gtest_discover_tests(unittests)