#include "PixiHostObject.h"
#include "frameprocessor/FrameHostObject.h"
#include <opencv2/opencv.hpp>
#include <image_resizer.h>
#include <yuv_to_rgba888.h>
#include "HalideBuffer.h"
#include "pixi/parsers.h"
#include "JSITypedArray.h"
#include <fbjni/ByteBuffer.h>
#include <android/asset_manager_jni.h>
#include <jsi/jsi.h>

typedef struct {
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


namespace Pixi {
    using namespace facebook;


    PixiHostObject::PixiHostObject(const jni::alias_ref<PixiProxy::javaobject> &pixiProxy) {
        _pixiProxy = make_global(pixiProxy);
    }

    PixiHostObject::~PixiHostObject() {}

    jsi::Value PixiHostObject::get(jsi::Runtime &runtime, const jsi::PropNameID &propNameId) {
        auto propName = propNameId.utf8(runtime);

        if (propName == "convert") {

            return jsi::Function::createFromHostFunction(runtime,
                                                         propNameId,
                                                         2,
                                                         [=](jsi::Runtime &runtime,
                                                             const jsi::Value &thisValue,
                                                             const jsi::Value *arguments,
                                                             size_t count) -> jsi::Value {
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

                                                             if (output_format != "rgba-8888") {
                                                                 throw jsi::JSError(runtime,
                                                                                    "Unsupported output format: " +
                                                                                    output_format +
                                                                                    ". Supported formats are: rgba-8888");
                                                             }

                                                             auto frame = dynamic_cast<vision::FrameHostObject *>(hostObject.get());
                                                             if (frame != nullptr) {
                                                                 jni::global_ref<vision::JFrame> unwrappedFrame = frame->frame;

                                                                 unwrappedFrame->incrementRefCount();
                                                                 int frameWidth = unwrappedFrame->getWidth();
                                                                 int frameHeight = unwrappedFrame->getHeight();

                                                                 __android_log_print(
                                                                         ANDROID_LOG_INFO,
                                                                         "PixiTag",
                                                                         "[yuv420toArgbNative] Frame dimensions: %d x %d",
                                                                         frameWidth,
                                                                         frameHeight);

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
                                                                 halide_dimension_t y_dim[] = {{0, frameWidth,  1},
                                                                                               {0, frameHeight, layout.y_row_stride}};
                                                                 halide_dimension_t uv_dim[] = {{0, width_uv,  1},
                                                                                                {0, height_uv, layout.uv_row_stride}};

                                                                 // 3 - create the Halide::Runtime::Buffer objects
                                                                 Halide::Runtime::Buffer<uint8_t> y_buffer(
                                                                         layout.y_data, 2, y_dim);
                                                                 Halide::Runtime::Buffer<uint8_t> u_buffer(
                                                                         layout.u_data, 2, uv_dim);
                                                                 Halide::Runtime::Buffer<uint8_t> v_buffer(
                                                                         layout.v_data, 2, uv_dim);



                                                                 // 5 - Call Halide yuv_to_rgba888 generator
                                                                 Halide::Runtime::Buffer<uint32_t> output_converted(
                                                                         frameWidth, frameHeight);
                                                                 auto start = std::chrono::high_resolution_clock::now();
                                                                 int result = yuv_to_rgba888(
                                                                         y_buffer, u_buffer,
                                                                         v_buffer,
                                                                         output_converted);
                                                                 auto stop = std::chrono::high_resolution_clock::now();

                                                                 auto duration = std::chrono::duration<double, std::milli>(
                                                                         stop - start).count();

                                                                 __android_log_print(
                                                                         ANDROID_LOG_INFO, "Timing",
                                                                         "yuv_to_rgba888 took %.3f ms",
                                                                         duration);


                                                                 if (result != 0) {

                                                                     __android_log_print(
                                                                             ANDROID_LOG_ERROR,
                                                                             "PixiTag",
                                                                             "[yuv420toArgbNative] Halide generator execution failed with error code %d.",
                                                                             result);
                                                                 }


                                                                 static constexpr auto ARRAYBUFFER_CACHE_PROP_NAME = "__bufferCache";

                                                                 auto arrayBuffer = vision::TypedArray<vision::TypedArrayKind::Uint32Array>(
                                                                         runtime,
                                                                         output_converted.size_in_bytes());

                                                                 runtime.global().setProperty(
                                                                         runtime,
                                                                         ARRAYBUFFER_CACHE_PROP_NAME,
                                                                         arrayBuffer);

                                                                 auto destinationBuffer = arrayBuffer.data(
                                                                         runtime);
//
                                                                 memcpy(destinationBuffer,
                                                                        output_converted.data(),
                                                                        output_converted.size_in_bytes());

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


                                                                 return arrayBuffer;
                                                             }

                                                             return jsi::Value::undefined();
                                                         });
        }
        if (propName == "sayHello") {
            return jsi::Function::createFromHostFunction(runtime,
                                                         propNameId,
                                                         1,
                                                         [=](jsi::Runtime &runtime,
                                                             const jsi::Value &thisValue,
                                                             const jsi::Value *arguments,
                                                             size_t count) -> jsi::Value {
                                                             int number = arguments[0].getNumber();
                                                             _pixiProxy->cthis()->sayHello(number);
                                                             return jsi::Value::undefined();
                                                         });
        }

        return jsi::Value::undefined();
    }

    std::vector<jsi::PropNameID> PixiHostObject::getPropertyNames(jsi::Runtime &runtime) {
        std::vector<jsi::PropNameID> result;
        result.push_back(jsi::PropNameID::forAscii(runtime, "convert"));
        return result;
    }

}
