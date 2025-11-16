set(TARGET triangles)

set(SOURCES 
    ${PROJECT_SOURCE_DIR}/source/main.cpp

    # Primitives
    ${PROJECT_SOURCE_DIR}/source/primitives/triangle.cpp
    ${PROJECT_SOURCE_DIR}/source/primitives/vector.cpp
    ${PROJECT_SOURCE_DIR}/source/primitives/plane.cpp
    ${PROJECT_SOURCE_DIR}/source/primitives/point.cpp
    ${PROJECT_SOURCE_DIR}/source/primitives/line.cpp
    ${PROJECT_SOURCE_DIR}/source/primitives/segment.cpp
    
    ${PROJECT_SOURCE_DIR}/source/octtree/octtree.cpp
    ${PROJECT_SOURCE_DIR}/source/octtree/bounding_box.cpp
    ${PROJECT_SOURCE_DIR}/source/octtree/bounding_sphere.cpp
   )

add_executable(${TARGET} ${SOURCES})
target_include_directories(${TARGET} PRIVATE "${PROJECT_SOURCE_DIR}/include")

set_target_properties(${TARGET} PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin
)

install(TARGETS ${TARGET}
    RUNTIME DESTINATION bin
    LIBRARY DESTINATION lib
)

if (MSVC)
    target_compile_options(${TARGET} PRIVATE
        # Warnings
        /W4
        /WX
    )
else()
    target_compile_options(${TARGET} PRIVATE
        # Warnings
        -Wall 
        -Wextra  
        -Werror
        # Sanitizers 
        $<$<BOOL:${SANITIZE}>: -fsanitize=address,undefined,leak,alignment,bool,bounds,enum,float-cast-overflow,float-divide-by-zero,integer-divide-by-zero,nonnull-attribute,null,object-size,return,returns-nonnull-attribute,shift,signed-integer-overflow,unreachable,vla-bound,vptr>
    )
    target_link_options(${TARGET} PRIVATE
        # Sanitizers
        $<$<BOOL:${SANITIZE}>: -fsanitize=address,undefined,leak,alignment,bool,bounds,enum,float-cast-overflow,float-divide-by-zero,integer-divide-by-zero,nonnull-attribute,null,object-size,return,returns-nonnull-attribute,shift,signed-integer-overflow,unreachable,vla-bound,vptr>
    )
endif()
