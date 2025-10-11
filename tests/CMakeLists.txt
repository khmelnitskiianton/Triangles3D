enable_testing()

include(FetchContent)
FetchContent_Declare(
  googletest
  GIT_REPOSITORY https://github.com/google/googletest.git
  GIT_TAG release-1.12.1
)
set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)
FetchContent_MakeAvailable(googletest)

get_filename_component(TESTS_DIR "${PROJECT_SOURCE_DIR}/tests/tests" ABSOLUTE)
get_filename_component(KEYS_DIR  "${PROJECT_SOURCE_DIR}/tests/keys"  ABSOLUTE)

# Test Primitives

add_executable(unittests 
  unittests.cpp

  ${PROJECT_SOURCE_DIR}/source/triangle.cpp
  
  # Primitives
  ${PROJECT_SOURCE_DIR}/source/primitives/vector.cpp
  ${PROJECT_SOURCE_DIR}/source/primitives/plane.cpp
  ${PROJECT_SOURCE_DIR}/source/primitives/point.cpp
  ${PROJECT_SOURCE_DIR}/source/primitives/line.cpp
  ${PROJECT_SOURCE_DIR}/source/primitives/common.cpp
  ${PROJECT_SOURCE_DIR}/source/primitives/segment.cpp
)
target_include_directories(unittests PUBLIC "${PROJECT_SOURCE_DIR}/include")
target_compile_options(unittests PRIVATE
    $<$<AND:$<CONFIG:Debug>,$<COMPILE_LANGUAGE:CXX>>:
        # Debug codegen
        -D _DEBUG 
        -g 
        -ggdb3
        -O0
        -fno-omit-frame-pointer
        -fno-optimize-sibling-calls
            
        # Warnings
        -Wall -Wextra -Wpedantic
        -Wconversion -Wsign-conversion -Wshadow
        -Wnon-virtual-dtor -Woverloaded-virtual
        -Wcast-align -Wcast-qual
        -Wmissing-declarations -Wundef
        -Wformat -Wformat-security
        -Wswitch-default -Wswitch-enum
        -Wunreachable-code
        -Wno-missing-field-initializers
        -Wno-old-style-cast
        -Wvariadic-macros

        # Sanitizers 
        -fsanitize=address,undefined,leak,alignment,bool,bounds,enum,float-cast-overflow,float-divide-by-zero,integer-divide-by-zero,nonnull-attribute,null,object-size,return,returns-nonnull-attribute,shift,signed-integer-overflow,unreachable,vla-bound,vptr
    >
)
target_link_options(unittests PRIVATE
    $<$<AND:$<CONFIG:Debug>,$<COMPILE_LANGUAGE:CXX>>:
        # Sanitizers
        -fsanitize=address,undefined,leak,alignment,bool,bounds,enum,float-cast-overflow,float-divide-by-zero,integer-divide-by-zero,nonnull-attribute,null,object-size,return,returns-nonnull-attribute,shift,signed-integer-overflow,unreachable,vla-bound,vptr
    >
)

target_link_libraries(
    unittests 
    PRIVATE
    GTest::gtest
    GTest::gmock
)

include(GoogleTest)
gtest_discover_tests(unittests)