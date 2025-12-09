set(TARGET triangles)

set(SOURCES 
    ${PROJECT_SOURCE_DIR}/source/main.cpp
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

#===================
# GRAPHIC TARGET
#===================

# Enable only with graphic
if (GRAPHIC)
    message(STATUS "Build graphic module...")

    # OpenGL, GLFW, GLM setup
    message(STATUS "Finding OpenGL,GLFW,GLM...")
    find_package(OpenGL REQUIRED)
    find_package(glfw3 REQUIRED)
    find_package(glm REQUIRED)

    # GLAD v.1 setup (https://lucatironi.net/tutorial/2023/12/20/manage_dependencies_cmake_fetchcontent_opengl_cpp/)
    message(STATUS "Fetching glad...")
    include(FetchContent)
    FetchContent_Declare(
        glad
        GIT_REPOSITORY https://github.com/Dav1dde/glad
        GIT_TAG        v0.1.36
        GIT_SHALLOW    TRUE
        GIT_PROGRESS   TRUE
    )
    FetchContent_MakeAvailable(glad)

    set(GRAPHIC_TARGET triangles-3d)

    set(GRAPHIC_SOURCES 
        ${PROJECT_SOURCE_DIR}/source/main_graphic.cpp
        # Primitives
        ${PROJECT_SOURCE_DIR}/source/primitives/triangle.cpp
        ${PROJECT_SOURCE_DIR}/source/primitives/vector.cpp
        ${PROJECT_SOURCE_DIR}/source/primitives/plane.cpp
        ${PROJECT_SOURCE_DIR}/source/primitives/point.cpp
        ${PROJECT_SOURCE_DIR}/source/primitives/line.cpp
        ${PROJECT_SOURCE_DIR}/source/primitives/segment.cpp
    
        # Octree
        ${PROJECT_SOURCE_DIR}/source/octtree/octtree.cpp
        ${PROJECT_SOURCE_DIR}/source/octtree/bounding_box.cpp
        ${PROJECT_SOURCE_DIR}/source/octtree/bounding_sphere.cpp

        # OpenGL
        
    )

    add_executable(${GRAPHIC_TARGET} ${GRAPHIC_SOURCES})
    target_include_directories(${GRAPHIC_TARGET} PRIVATE "${PROJECT_SOURCE_DIR}/include")

    target_link_libraries(${GRAPHIC_TARGET} PRIVATE 
      glfw
      glad
      glm::glm
      OpenGL::GL
    )

    set_target_properties(${GRAPHIC_TARGET} PROPERTIES
        RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin
    )

    install(TARGETS ${GRAPHIC_TARGET}
        RUNTIME DESTINATION bin
        LIBRARY DESTINATION lib
    )

    if (MSVC)
        target_compile_options(${GRAPHIC_TARGET} PRIVATE
            # Warnings
            /W4
            /WX
        )
    else()
        target_compile_options(${GRAPHIC_TARGET} PRIVATE
            # Warnings
            -Wall 
            -Wextra  
            -Werror
            # Sanitizers 
            $<$<BOOL:${SANITIZE}>: -fsanitize=address,undefined,leak,alignment,bool,bounds,enum,float-cast-overflow,float-divide-by-zero,integer-divide-by-zero,nonnull-attribute,null,object-size,return,returns-nonnull-attribute,shift,signed-integer-overflow,unreachable,vla-bound,vptr>
        )
        target_link_options(${GRAPHIC_TARGET} PRIVATE
            # Sanitizers
            $<$<BOOL:${SANITIZE}>: -fsanitize=address,undefined,leak,alignment,bool,bounds,enum,float-cast-overflow,float-divide-by-zero,integer-divide-by-zero,nonnull-attribute,null,object-size,return,returns-nonnull-attribute,shift,signed-integer-overflow,unreachable,vla-bound,vptr>
        )
    endif()
endif()