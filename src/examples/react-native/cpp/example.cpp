#include <jni.h>
#include <android/log.h>
#include <stdexcept>
#include <android/asset_manager.h>
#include <android/asset_manager_jni.h>
#include <opencv2/opencv.hpp>
#include <image_resizer.h>
#include "HalideBuffer.h"
#include "pixi/parsers.h"

extern "C" JNIEXPORT jboolean JNICALL
Java_com_example_pixi_PixiModule_nativeInstall(JNIEnv *env, jobject thiz, jlong jsi_ptr, jobject assetManager)
{
    try
    {
        __android_log_print(ANDROID_LOG_INFO, "PixiTag", "Initializing with Asset Manager");

        AAssetManager *mgr = AAssetManager_fromJava(env, assetManager);
        if (mgr == nullptr)
        {
            __android_log_print(ANDROID_LOG_ERROR, "PixiTag", "Failed to obtain AAssetManager");
            return JNI_FALSE;
        }

        const char *filename = "images/wallpaper.png";
        AAsset *asset = AAssetManager_open(mgr, filename, AASSET_MODE_BUFFER);
        if (asset == nullptr)
        {
            __android_log_print(ANDROID_LOG_ERROR, "PixiTag", "Failed to open asset");
            return JNI_FALSE;
        }

        off_t length = AAsset_getLength(asset);
        std::vector<unsigned char> buffer(length);

        AAsset_read(asset, buffer.data(), length);
        AAsset_close(asset);

        cv::Mat data(1, length, CV_8UC1, buffer.data());
        cv::Mat image = cv::imdecode(data, cv::IMREAD_UNCHANGED);

        int32_t output_width = 800;
        int32_t output_height = 600;

        cv::Size newSize(output_width, output_height);

        auto cv_start = std::chrono::high_resolution_clock::now();

        cv::Mat resizedImage;
        cv::resize(image, resizedImage, newSize);

        // Stop timing
        auto cv_end = std::chrono::high_resolution_clock::now();
        auto cv_duration = std::chrono::duration_cast<std::chrono::microseconds>(cv_end - cv_start);
        __android_log_print(ANDROID_LOG_INFO, "PixiTag", "Image resized with OpenCV took %d microseconds", cv_duration.count());

        Halide::Runtime::Buffer<uint8_t> halide_input_image = cvMatToHalideBuffer(image);

        Halide::Runtime::Buffer<uint8_t> halide_output_image(output_width, output_height, image.channels());

        auto halide_start = std::chrono::high_resolution_clock::now();
        int resize_status = image_resizer(halide_input_image, output_width, output_height, halide_output_image);
        auto halide_stop = std::chrono::high_resolution_clock::now();
        auto halide_duration = std::chrono::duration_cast<std::chrono::microseconds>(halide_stop - halide_start);
        __android_log_print(ANDROID_LOG_INFO, "PixiTag", "Image resize with Halide took %d microseconds", halide_duration.count());

        if (resize_status != 0)
        {
            std::cerr << "Image resizing failed." << std::endl;
            return 1;
        }

        return JNI_TRUE;
    }
    catch (...)
    {
        __android_log_print(ANDROID_LOG_ERROR, "PixiTag", "Unknown error occurred during installation");
        return JNI_FALSE;
    }
}
