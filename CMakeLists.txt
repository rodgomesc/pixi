cmake_minimum_required(VERSION 3.12)
project(Pixi)

# set the path to your Halide installation
set(HALIDE_DIR "${HOME}/workspace/personal/Halide-16.0.0-arm-64-osx")
set(HALIDE_TOOLS_DIR "${HALIDE_DIR}/share/Halide/tools")
list(APPEND CMAKE_PREFIX_PATH "${HALIDE_DIR}/lib/cmake")

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED YES)
set(CMAKE_CXX_EXTENSIONS NO)

# Find packages
find_package(OpenCV CONFIG REQUIRED)
find_package(Halide REQUIRED)


include_directories(
        include
        ${OpenCV_INCLUDE_DIRS}
        ${HALIDE_INCLUDE_DIR}
        ${HALIDE_TOOLS_DIR}
)

add_executable(generators
        ${CMAKE_CURRENT_SOURCE_DIR}/src/generators/yuv420_to_rgba8888.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/src/generators/image_resizer.cpp
)

add_halide_library(yuv_to_rgba888 FROM generators)
add_halide_library(image_resizer FROM generators)
#add_halide_library(image_resizer FROM generators TARGETS host-metal)


target_link_libraries(generators PRIVATE
        Halide::Halide
        Halide::Generator
        Halide::Tools
        ${OpenCV_LIBS})

# ============ examples ============

# Example executable that uses the image_resizer
add_executable(image_resizer_example
        ${CMAKE_CURRENT_SOURCE_DIR}/src/examples/generators_usage/resizer_example.cpp
)


# Link the image_resizer static library and include its header
target_link_libraries(image_resizer_example PRIVATE
        image_resizer
        ${OpenCV_LIBS}
        Halide::Halide
        Halide::Tools

)






add_executable(yuv_420_to_rgba8888_example
        ${CMAKE_CURRENT_SOURCE_DIR}/src/examples/generators_usage/yuv_420_to_rgba8888_example.cpp
)

target_link_libraries(yuv_420_to_rgba8888_example PRIVATE
        yuv_to_rgba888
        ${OpenCV_LIBS}
        Halide::Halide
        Halide::Tools
)



# Include the directory where the image_resizer.h will be generated
target_include_directories(image_resizer_example PRIVATE
        ${CMAKE_CURRENT_BINARY_DIR}
)


# =====================================