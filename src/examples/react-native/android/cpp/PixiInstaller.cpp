//
// Created by rodrigo gomes on 16/01/24.
//

#include "PixiInstaller.h"
#include "PixiHostObject.h"


namespace Pixi {
    using namespace facebook::jni;

    void PixiInstaller::installHostObject(jni::alias_ref<jni::JClass>,
                                          jni::alias_ref<PixiProxy::javaobject> proxy) {

        auto pixiProxy = std::make_shared<PixiHostObject>(proxy);
        jsi::Runtime &runtime = *proxy->cthis()->getJSRuntime();
        runtime.global().setProperty(runtime, "__PixiHostFn",
                                     jsi::Object::createFromHostObject(runtime, pixiProxy));

    }


    void PixiInstaller::registerNatives() {
        javaClassStatic()->registerNatives(
                {makeNativeMethod("installHostObject", PixiInstaller::installHostObject)});
    }
}
