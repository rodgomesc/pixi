#include <HalideBuffer.h>
#include <HalideRuntime.h>
#include <iostream>
#include <chrono>
#include <Halide.h>
#include "yuv_to_rgba888.h"
#include <halide_image_io.h>
#include <opencv2/opencv.hpp>

using namespace Halide;
using namespace Halide::Tools;

using namespace Halide;
using namespace Halide::Tools;

void BgrToY8Cb8Cr8_420(const cv::Mat &bgr_image, Buffer<uint8_t> &y_plane, Buffer<uint8_t> &cb_plane, Buffer<uint8_t> &cr_plane) {
    CV_Assert(bgr_image.depth() == CV_8U);

    const int width = bgr_image.cols;
    const int height = bgr_image.rows;

    y_plane = Buffer<uint8_t>(width, height);
    cb_plane = Buffer<uint8_t>(width / 2, height / 2);
    cr_plane = Buffer<uint8_t>(width / 2, height / 2);

    // Separate buffers for full resolution Cb and Cr
    auto temp_cb = new uint8_t[width * height];
    auto temp_cr = new uint8_t[width * height];

    for (int y = 0; y < height; y++) {
        for (int x = 0; x < width; x++) {
            cv::Vec3b bgr = bgr_image.at<cv::Vec3b>(y, x);
            float b = bgr[0];
            float g = bgr[1];
            float r = bgr[2];

            uint8_t Y = static_cast<uint8_t>(std::clamp(static_cast<int>(0.299 * r + 0.587 * g + 0.114 * b), 0, 255));
            uint8_t Cb = static_cast<uint8_t>(std::clamp(static_cast<int>(128 - 0.168736 * r - 0.331264 * g + 0.5 * b), 0, 255));
            uint8_t Cr = static_cast<uint8_t>(std::clamp(static_cast<int>(128 + 0.5 * r - 0.418688 * g - 0.081312 * b), 0, 255));

            y_plane(x, y) = Y;
            temp_cb[y * width + x] = Cb;
            temp_cr[y * width + x] = Cr;
        }
    }

    // Subsample Cb and Cr
    for (int y = 0; y < height; y += 2) {
        for (int x = 0; x < width; x += 2) {
            uint32_t sum_cb = 0;
            uint32_t sum_cr = 0;
            for (int dy = 0; dy < 2; dy++) {
                for (int dx = 0; dx < 2; dx++) {
                    int index = (y + dy) * width + (x + dx);
                    sum_cb += temp_cb[index];
                    sum_cr += temp_cr[index];
                }
            }
            cb_plane(x / 2, y / 2) = sum_cb / 4;
            cr_plane(x / 2, y / 2) = sum_cr / 4;
        }
    }

    delete[] temp_cb;
    delete[] temp_cr;
}

int main(int argc, char **argv) {
    cv::Mat bgr_image = cv::imread("wallpaper.png", cv::IMREAD_UNCHANGED);
    if (bgr_image.empty()) {
        std::cerr << "Failed to load image." << std::endl;
        return 1;
    }

    // Ensure the dimensions of the input image are even for YUV 4:2:0 format.
    if ((bgr_image.cols % 2 != 0) || (bgr_image.rows % 2 != 0)) {
        cv::resize(bgr_image, bgr_image, cv::Size(bgr_image.cols + (bgr_image.cols % 2),
                                                  bgr_image.rows + (bgr_image.rows % 2)));
    }

    // Perform BGR to Y8Cb8Cr8_420 conversion using our helper function.
    int width = bgr_image.cols;
    int height = bgr_image.rows;
    Buffer<uint8_t> y_plane, cb_plane, cr_plane;


    BgrToY8Cb8Cr8_420(bgr_image, y_plane, cb_plane, cr_plane);

    // Convert the Halide::Runtime::Buffer to halide_buffer_t for yuv_to_rgba888
    Buffer<uint32_t> output(width, height);

    halide_buffer_t *y_plane_buffer = y_plane.raw_buffer();
    halide_buffer_t *cb_plane_buffer = cb_plane.raw_buffer();
    halide_buffer_t *cr_plane_buffer = cr_plane.raw_buffer();
    halide_buffer_t *output_buffer = output.raw_buffer();

    auto start = std::chrono::high_resolution_clock::now();
    int result = yuv_to_rgba888(y_plane_buffer, cb_plane_buffer, cr_plane_buffer, output_buffer);
    auto stop = std::chrono::high_resolution_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::microseconds>(stop - start);

    // Output the execution time in microseconds
    std::cout << "Generator execution time: " << duration.count() << " microseconds." << std::endl;
    if (result != 0) {
        std::cerr << "Halide pipeline returned an error." << std::endl;
        return 1;
    }

    // The output buffer now contains the converted image.
    cv::Mat output_image(height, width, CV_8UC4, output_buffer->host);
    cv::imwrite("output_image.png", output_image);

    return 0;
}