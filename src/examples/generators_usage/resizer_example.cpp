#include <HalideBuffer.h>
#include <HalideRuntime.h>
#include <iostream>
#include <chrono>
#include <Halide.h>
#include "image_resizer.h"
#include <halide_image_io.h>

using namespace Halide;
using namespace Halide::Tools;

int main() {
    // Load the input image using Halide's tools
    Buffer<uint8_t> input_buffer = load_image("wallpaper.jpg");
    if (!input_buffer.data()) {
        std::cerr << "Failed to load input image." << std::endl;
        return 1;
    }

    int32_t output_width = 800;
    int32_t output_height = 600;

     Halide::Runtime::Buffer<uint8_t> output_buffer(output_width, output_height, input_buffer.channels());



    // Convert the Halide::Runtime::Buffer to halide_buffer_t for image_resizer
    halide_buffer_t *input_halide_buffer = input_buffer.raw_buffer();
    halide_buffer_t *output_halide_buffer = output_buffer.raw_buffer();

    auto start = std::chrono::high_resolution_clock::now();
    int resize_status = image_resizer(input_halide_buffer, output_width, output_height, output_halide_buffer);
    auto stop = std::chrono::high_resolution_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::microseconds>(stop - start);
    if (resize_status != 0) {
        std::cerr << "Image resizing failed." << std::endl;
        return 1;
    }



    std::cout << "Resizing took " << duration.count() << " microseconds." << std::endl;

    // Save the output image using Halide's tools
    save_image(output_buffer, "resized_output.png");

    return 0;
}
