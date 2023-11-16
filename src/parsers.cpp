//
// Created by rodrigo gomes on 15/11/23.
//
#include "pixi/parsers.h"

using namespace std;
using namespace Halide::Runtime;
using namespace cv;

Buffer<uint8_t> cvMatToHalideBuffer(const cv::Mat &image)
{
    int width = image.cols;
    int height = image.rows;
    int channels = image.channels();

    Buffer<uint8_t> halide_buffer(width, height, channels);

    for (int y = 0; y < height; ++y)
    {
        for (int x = 0; x < width; ++x)
        {
            for (int c = 0; c < channels; ++c)
            {
                halide_buffer(x, y, c) = image.at<uint8_t>(y, x * channels + c);
            }
        }
    }

    return halide_buffer;
}

Mat halideBufferToCvMat(const Buffer<uint8_t> &buffer)
{
    int width = buffer.width();
    int height = buffer.height();
    int channels = buffer.channels();

    Mat image(height, width, CV_MAKETYPE(CV_8U, channels));

    for (int y = 0; y < height; ++y)
    {
        for (int x = 0; x < width; ++x)
        {
            for (int c = 0; c < channels; ++c)
            {
                image.at<uint8_t>(y, x * channels + c) = buffer(x, y, c);
            }
        }
    }

    return image;
}
