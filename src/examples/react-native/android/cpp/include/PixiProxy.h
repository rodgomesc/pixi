#pragma once

#include <fbjni/fbjni.h>
#include <jsi/jsi.h>

namespace Pixi
{
    using namespace facebook;

    class PixiProxy : public jni::JavaClass<PixiProxy>
    {
    public:
        static auto constexpr kJavaDescriptor = "Lcom/example/pixi/PixiModule;";
        static void registerNatives();
        static jboolean installJsiRuntime(jni::alias_ref<jni::JClass>, jlong runtimePtr);
    };
}
