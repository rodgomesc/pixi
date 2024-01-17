#include "PixiProxy.h"
#include <fbjni/ByteBuffer.h>
#include <memory>
#include <string>
#include <utility>
#include <jsi/jsi.h>

namespace Pixi {
    using namespace facebook::jni;

    using TSelf = local_ref<HybridClass<PixiProxy>::jhybriddata>;

    PixiProxy::PixiProxy(const jni::alias_ref<PixiProxy::jhybridobject> &javaThis,
                         jsi::Runtime *runtime) {
        _javaPart = make_global(javaThis);
        _runtime = runtime;
    }

    PixiProxy::~PixiProxy() {
    }


    void PixiProxy::sayHello(int number) {
        auto sayHelloMethod = javaClassLocal()->getMethod<void(int)>("sayHello");
        sayHelloMethod(_javaPart, number);
    }

    void PixiProxy::receiveBuffer(alias_ref<JByteBuffer> buffer, int width, int height) {
        auto receiveBufferMethod = javaClassLocal()->getMethod<void(alias_ref<JByteBuffer>, int,
                                                                    int)>("receiveBuffer");
        receiveBufferMethod(_javaPart, buffer, width, height);
    }


    void PixiProxy::registerNatives() {
        registerHybrid({makeNativeMethod("initHybrid", PixiProxy::initHybrid)});
    }

    jsi::Runtime *PixiProxy::getJSRuntime() {
        return _runtime;
    }

    TSelf PixiProxy::initHybrid(alias_ref<jhybridobject> jThis, jlong jsRuntimePointer) {
        auto jsRuntime = reinterpret_cast<jsi::Runtime *>(jsRuntimePointer);
        return makeCxxInstance(jThis, jsRuntime);
    }

}