cmake_minimum_required(VERSION 3.12)

project(Pixiologist)

# Specify the C++ standard
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED True)
# set(BUILD_EXAMPLES OFF CACHE BOOL "Build the example programs" FORCE)



# Add the directory where FindHalide.cmake is located to the module path
add_subdirectory(3rdparty/Halide)

include_directories(include)


add_library(pixiologist STATIC
        src/color_space/yuv420_to_rgba8888.cc
        )


# Conditionally add the examples subdirectory
if(BUILD_EXAMPLES)
    add_executable(yuv420_to_rgba_example examples/yuv420_to_rgba_example.cpp)
    # Link the executable to the library
    target_link_libraries(yuv420_to_rgba_example PRIVATE pixiologist Halide::Halide)
endif()
