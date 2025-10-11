set(TARGET triangles)

set(SOURCES 
    ${PROJECT_SOURCE_DIR}/source/main.cpp
    ${PROJECT_SOURCE_DIR}/source/triangle.cpp

    # Primitives
    ${PROJECT_SOURCE_DIR}/source/primitives/vector.cpp
    ${PROJECT_SOURCE_DIR}/source/primitives/plane.cpp
    ${PROJECT_SOURCE_DIR}/source/primitives/point.cpp
    ${PROJECT_SOURCE_DIR}/source/primitives/line.cpp
    ${PROJECT_SOURCE_DIR}/source/primitives/common.cpp
    ${PROJECT_SOURCE_DIR}/source/primitives/segment.cpp

   )

add_executable(${TARGET} ${SOURCES})

target_include_directories(${TARGET} PUBLIC "${PROJECT_SOURCE_DIR}/include")
set_target_properties(${TARGET} PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin
)

target_compile_options(${TARGET} PRIVATE
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
target_link_options(${TARGET} PRIVATE
    $<$<AND:$<CONFIG:Debug>,$<COMPILE_LANGUAGE:CXX>>:
        # Sanitizers
        -fsanitize=address,undefined,leak,alignment,bool,bounds,enum,float-cast-overflow,float-divide-by-zero,integer-divide-by-zero,nonnull-attribute,null,object-size,return,returns-nonnull-attribute,shift,signed-integer-overflow,unreachable,vla-bound,vptr
    >
)
target_compile_options(${TARGET} PRIVATE
    $<$<AND:$<CONFIG:Release>,$<COMPILE_LANGUAGE:CXX>>:
        # Optimizations & disabling checks
        -O3 -DNDEBUG
    >
)
