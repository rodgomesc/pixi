#include <HalideBuffer.h>
#include <HalideRuntime.h>
#include <iostream>
#include <opencv2/opencv.hpp>
#include <chrono>
#include "pixiologist/parsers.h"
#include "image_resizer.h"

int main()
{

    cv::Mat input_image = cv::imread("wallpaper.png", cv::IMREAD_UNCHANGED);
    if (input_image.empty())
    {
        std::cerr << "Failed to load input image." << std::endl;
        return 1;
    }

    Halide::Runtime::Buffer<uint8_t> halide_input_image = cvMatToHalideBuffer(input_image);

    int32_t output_width = 800;
    int32_t output_height = 600;

    Halide::Runtime::Buffer<uint8_t> halide_output_image(output_width, output_height, input_image.channels());

    auto start = std::chrono::high_resolution_clock::now();

    int resize_status = image_resizer(halide_input_image, output_width, output_height, halide_output_image);

    auto stop = std::chrono::high_resolution_clock::now();

    if (resize_status != 0)
    {
        std::cerr << "Image resizing failed." << std::endl;
        return 1;
    }

    auto duration = std::chrono::duration_cast<std::chrono::microseconds>(stop - start);
    std::cout << "Resizing took " << duration.count() << " microseconds." << std::endl;

    cv::Mat output_image = halideBufferToCvMat(halide_output_image);

    bool success = cv::imwrite("resized_output.png", output_image);
    if (!success)
    {
        std::cerr << "Failed to save output image." << std::endl;
        return 1;
    }

    return 0;
}
