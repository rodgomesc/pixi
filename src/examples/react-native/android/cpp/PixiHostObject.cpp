#include "PixiHostObject.h"
#include "frameprocessor/FrameHostObject.h"
#include <opencv2/opencv.hpp>
#include <image_resizer.h>
#include <yuv_to_rgba888.h>
#include "HalideBuffer.h"
#include "pixi/parsers.h"

#include <android/asset_manager_jni.h>

typedef struct
{
        uint8_t *y_data;
        uint8_t *u_data;
        uint8_t *v_data;
        int y_row_stride;
        int y_pixel_stride;
        int uv_row_stride;
        int uv_pixel_stride;
        int width;
        int height;
} YUVLayout;

Runtime::Buffer<uint8_t> convertBufferToUint8(const Runtime::Buffer<uint32_t> &input)
{
        // Get the dimensions of the input buffer
        int width = input.width();
        int height = input.height();

        // Create a new uint8_t buffer with the same dimensions
        std::vector<uint8_t> temp_output_uint8(width * height);
        Runtime::Buffer<uint8_t> output_uint8(temp_output_uint8.data(), width, height);

        // Copy and cast each element from the uint32_t buffer to the uint8_t buffer
        for (int y = 0; y < height; ++y)
        {
                for (int x = 0; x < width; ++x)
                {
                        // Cast the uint32 value to uint8 and store it in the new buffer
                        output_uint8(x, y) = static_cast<uint8_t>(input(x, y));
                }
        }

        return output_uint8;
}

namespace Pixi
{
        using namespace facebook;

        PixiHostObject::PixiHostObject(const jni::alias_ref<PixiProxy::javaobject> &pixiProxy)
        {
                _pixiProxy = make_global(pixiProxy);
        }

        PixiHostObject::~PixiHostObject() {}

        jsi::Value PixiHostObject::get(jsi::Runtime &runtime, const jsi::PropNameID &propNameId)
        {
                auto propName = propNameId.utf8(runtime);

                if (propName == "convert")
                {

                        return jsi::Function::createFromHostFunction(runtime,
                                                                     propNameId,
                                                                     2,
                                                                     [=](jsi::Runtime &runtime,
                                                                         const jsi::Value &thisValue,
                                                                         const jsi::Value *arguments,
                                                                         size_t count) -> jsi::Value
                                                                     {
                                                                             std::shared_ptr hostObject = arguments[0].getObject(
                                                                                                                          runtime)
                                                                                                              .asHostObject(runtime);
                                                                             jsi::Object optionsObject = arguments[1].getObject(
                                                                                 runtime);
                                                                             int output_width = optionsObject.getProperty(
                                                                                                                 runtime,
                                                                                                                 "output_width")
                                                                                                    .asNumber();
                                                                             int output_height = optionsObject.getProperty(
                                                                                                                  runtime,
                                                                                                                  "output_height")
                                                                                                     .asNumber();
                                                                             std::string output_format = optionsObject.getProperty(
                                                                                                                          runtime,
                                                                                                                          "output_format")
                                                                                                             .getString(
                                                                                                                 runtime)
                                                                                                             .utf8(runtime);

                                                                             if (output_format != "rgba-8888")
                                                                             {
                                                                                     throw jsi::JSError(runtime,
                                                                                                        "Unsupported output format: " +
                                                                                                            output_format +
                                                                                                            ". Supported formats are: rgba-8888");
                                                                             }

                                                                             auto frame = dynamic_cast<vision::FrameHostObject *>(hostObject.get());
                                                                             if (frame != nullptr)
                                                                             {
                                                                                     jni::global_ref<vision::JFrame> unwrappedFrame = frame->frame;

                                                                                     unwrappedFrame->incrementRefCount();
                                                                                     int frameWidth = unwrappedFrame->getWidth();
                                                                                     int frameHeight = unwrappedFrame->getHeight();

                                                                                     AHardwareBuffer *ahb = unwrappedFrame->getHardwareBuffer();
                                                                                     AHardwareBuffer_Desc bufferDescription;
                                                                                     AHardwareBuffer_describe(ahb,
                                                                                                              &bufferDescription);

                                                                                     YUVLayout layout = {0};
                                                                                     AHardwareBuffer_Planes planes;
                                                                                     AHardwareBuffer_lockPlanes(ahb,
                                                                                                                AHARDWAREBUFFER_USAGE_CPU_READ_MASK,
                                                                                                                -1,
                                                                                                                nullptr,
                                                                                                                &planes);

                                                                                     // 1 - get the YUV layout from the AHardwareBuffer
                                                                                     layout.y_data = static_cast<uint8_t *>(planes.planes[0].data);
                                                                                     layout.y_row_stride = planes.planes[0].rowStride;
                                                                                     layout.y_pixel_stride = planes.planes[0].pixelStride;
                                                                                     layout.u_data = static_cast<uint8_t *>(planes.planes[1].data);
                                                                                     layout.uv_row_stride = planes.planes[1].rowStride;
                                                                                     layout.uv_pixel_stride = planes.planes[1].pixelStride;
                                                                                     layout.v_data = static_cast<uint8_t *>(planes.planes[2].data);
                                                                                     layout.width = bufferDescription.width;
                                                                                     layout.height = bufferDescription.height;

                                                                                     // 2 - create y, uv dimensions
                                                                                     int width_uv = frameWidth / 2;
                                                                                     int height_uv = frameHeight / 2;
                                                                                     halide_dimension_t y_dim[] = {{0, frameWidth, 1},
                                                                                                                   {0, frameHeight, layout.y_row_stride}};
                                                                                     halide_dimension_t uv_dim[] = {{0, width_uv, 1},
                                                                                                                    {0, height_uv, layout.uv_row_stride}};

                                                                                     // 3 - create the Halide::Runtime::Buffer objects
                                                                                     Halide::Runtime::Buffer<uint8_t> y_buffer(
                                                                                         layout.y_data, 2, y_dim);
                                                                                     Halide::Runtime::Buffer<uint8_t> u_buffer(
                                                                                         layout.u_data, 2, uv_dim);
                                                                                     Halide::Runtime::Buffer<uint8_t> v_buffer(
                                                                                         layout.v_data, 2, uv_dim);

                                                                                     // 4 - create an output buffer with the same dimensions as the Y plane
                                                                                     std::vector<uint32_t> temp_output(
                                                                                         frameWidth * frameHeight);
                                                                                     Halide::Runtime::Buffer<uint32_t> output_converted(
                                                                                         temp_output.data(),
                                                                                         frameWidth, frameHeight);

                                                                                     // temporary test for resize input generator
                                                                                     //                                                       Runtime::Buffer<uint8_t> output_uint8 = convertBufferToUint8(output_converted);

                                                                                     // 5 - call Halide yuv_to_rgba888 generator
                                                                                     auto halide_start = std::chrono::high_resolution_clock::now();
                                                                                     int result = yuv_to_rgba888(
                                                                                         y_buffer, u_buffer,
                                                                                         v_buffer,
                                                                                         output_converted);
                                                                                     auto halide_stop = std::chrono::high_resolution_clock::now();
                                                                                     auto halide_duration_micro = std::chrono::duration_cast<std::chrono::microseconds>(
                                                                                         halide_stop -
                                                                                         halide_start);
                                                                                     double halide_duration_milli =
                                                                                         halide_duration_micro.count() /
                                                                                         1000.0;

                                                                                     __android_log_print(
                                                                                         ANDROID_LOG_INFO,
                                                                                         "PixiTag",
                                                                                         "[yuv420toArgbNative] Execution time: %.3f ms",
                                                                                         halide_duration_milli);
                                                                                     if (result != 0)
                                                                                     {
                                                                                             __android_log_print(
                                                                                                 ANDROID_LOG_ERROR,
                                                                                                 "PixiTag",
                                                                                                 "[yuv420toArgbNative] Halide generator execution failed with error code %d.",
                                                                                                 result);
                                                                                     }

                                                                                     AHardwareBuffer_unlock(ahb,
                                                                                                            nullptr);
                                                                                     unwrappedFrame->decrementRefCount();

                                                                                     jsi::Object frameDataObj(runtime);
                                                                                     frameDataObj.setProperty(runtime,
                                                                                                              "output_width",
                                                                                                              frameWidth);
                                                                                     frameDataObj.setProperty(runtime,
                                                                                                              "output_height",
                                                                                                              frameHeight);

                                                                                     return frameDataObj;
                                                                             }

                                                                             return jsi::Value::undefined();
                                                                     });
                }
                if (propName == "sayHello")
                {
                        return jsi::Function::createFromHostFunction(runtime,
                                                                     propNameId,
                                                                     1,
                                                                     [=](jsi::Runtime &runtime,
                                                                         const jsi::Value &thisValue,
                                                                         const jsi::Value *arguments,
                                                                         size_t count) -> jsi::Value
                                                                     {
                                                                             int number = arguments[0].getNumber();
                                                                             _pixiProxy->cthis()->sayHello(number);
                                                                             return jsi::Value::undefined();
                                                                     });
                }

                return jsi::Value::undefined();
        }

        std::vector<jsi::PropNameID> PixiHostObject::getPropertyNames(jsi::Runtime &runtime)
        {
                std::vector<jsi::PropNameID> result;
                result.push_back(jsi::PropNameID::forAscii(runtime, "convert"));
                return result;
        }

}
