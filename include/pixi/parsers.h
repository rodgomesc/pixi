//
// Created by rodrigo gomes on 15/11/23.
//

#ifndef PIXI_PARSERS_H
#define PIXI_PARSERS_H


#include "HalideBuffer.h"
#include <opencv2/opencv.hpp>

using namespace Halide;

// Function to convert a cv::Mat to a Halide::Buffer
Halide::Runtime::Buffer<uint8_t> cvMatToHalideBuffer(const cv::Mat &image);

// Function to convert a Halide::Buffer to a cv::Mat
cv::Mat halideBufferToCvMat(const Halide::Runtime::Buffer<uint8_t> &buffer);


#endif //PIXI_PARSERS_H
