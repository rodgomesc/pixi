
#include <jsi/jsi.h>
#include <fbjni/fbjni.h>
#include <fbjni/detail/Hybrid.h>
#include "PixiProxy.h"


namespace Pixi {
    using namespace facebook;

    class PixiInstaller : public jni::JavaClass<PixiInstaller> {
    public:
        static auto constexpr kJavaDescriptor = "Lcom/example/pixi/PixiInstaller;";

        static void registerNatives();

        static void
        installHostObject(jni::alias_ref<jni::JClass> clazz,
                          jni::alias_ref<PixiProxy::javaobject> proxy);
    };
}
