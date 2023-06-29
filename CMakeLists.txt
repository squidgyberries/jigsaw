cmake_minimum_required(VERSION 3.20)
project(jigsaw LANGUAGES C)

set(CMAKE_C_STANDARD 17)
set(CMAKE_C_EXTENSIONS OFF)

set(GLFW_BUILD_DOCS OFF CACHE BOOL "" FORCE)
set(GLFW_BUILD_TESTS OFF CACHE BOOL "" FORCE)
set(GLFW_BUILD_EXAMPLES OFF CACHE BOOL "" FORCE)

add_subdirectory(deps/glfw)

set(CGLM_SHARED OFF CACHE BOOL "" FORCE)
set(CGLM_STATIC ON CACHE BOOL "" FORCE)

add_subdirectory(deps/cglm)

add_compile_options(-Wall -Wextra -Wpedantic -Wno-unused-parameter)

add_executable(jigsaw src/jigsaw.c src/shader.c src/stb_image.c deps/glad/src/gl.c)
target_include_directories(jigsaw PRIVATE deps/glad/include deps/stb)
target_link_libraries(jigsaw glfw cglm)