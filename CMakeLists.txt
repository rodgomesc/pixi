cmake_minimum_required(VERSION 3.12)

project(Pixiologist)

# Specify the C++ standard
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED True)
# set(BUILD_EXAMPLES OFF CACHE BOOL "Build the example programs" FORCE)

find_package(Halide REQUIRED PATHS 3rdparty/Halide-16.0.0-arm-64-osx/lib/cmake/Halide)


include_directories(
        include
        3rdparty/Halide-16.0.0-arm-64-osx/include
)


add_library(pixiologist STATIC
        src/generators/yuv420_to_rgba8888.cpp
        )


# Conditionally add the examples subdirectory
if(BUILD_EXAMPLES)
    add_executable(exe
            examples/main.cpp
            )
    # Link the executable to the library
    target_link_libraries(exe PRIVATE pixiologist Halide::Halide)
endif()
