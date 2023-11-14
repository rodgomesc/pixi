#!/bin/bash

# Halide tutorial lesson 10: AOT compilation automation script

# Define the paths to Halide header and library
HALIDE_INCLUDE_PATH="../../3rdparty/Halide-16.0.0-arm-64-osx/include/"
HALIDE_LIB_PATH="../../3rdparty/Halide-16.0.0-arm-64-osx/lib/"

HALIDE_LIB_NAME="libHalide.dylib"


g++ yuv420_to_rgba8888.cpp -g -std=c++17 -I $HALIDE_INCLUDE_PATH -L $HALIDE_LIB_PATH -lHalide -lpthread -ldl -o yuv420_to_rgba8888_generate


#./yuv420_to_rgba8888_generate -g yuv_to_rgba888 -o output -f yuv_to_rgba8888 -e static_library,h,assembly,stmt,stmt_html target=host
./yuv420_to_rgba8888_generate -g yuv_to_rgba888 -o output -f yuv_to_rgba8888 -e static_library,h target=arm-64-android
