cmake_minimum_required(VERSION 3.12)
project(Pixi)

list(APPEND CMAKE_PREFIX_PATH ${HOME}/vcpkg/installed/arm64-osx/share/HalideHelpers)


set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED YES)
set(CMAKE_CXX_EXTENSIONS NO)

# Find packages
find_package(OpenCV CONFIG REQUIRED)
find_package(Halide REQUIRED)

find_package(Stb REQUIRED)


# Include directories
include_directories(
        include
        ${OpenCV_INCLUDE_DIRS}
        ${HALIDE_INCLUDE_DIR}
)

add_executable(generators
        ${CMAKE_CURRENT_SOURCE_DIR}/src/generators/yuv420_to_rgba8888.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/src/generators/image_resizer.cpp
)

add_halide_library(yuv_to_rgba888 FROM generators)
add_halide_library(image_resizer FROM generators)


target_link_libraries(generators PRIVATE
        Halide::Halide
        Halide::Generator
        Halide::Tools
        ${OpenCV_LIBS}
        ${HALIDE_LIBRARY})

# ============ examples ============

# Example executable that uses the image_resizer
add_executable(image_resizer_example
        ${CMAKE_CURRENT_SOURCE_DIR}/src/parsers.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/src/examples/generators_usage/resizer_example.cpp
)

# Link the image_resizer static library and include its header
target_link_libraries(image_resizer_example PRIVATE
        image_resizer
        ${OpenCV_LIBS}

)

# Include the directory where the image_resizer.h will be generated
target_include_directories(image_resizer_example PRIVATE

        ${CMAKE_CURRENT_BINARY_DIR}
        ${Stb_INCLUDE_DIR}


)


# =====================================