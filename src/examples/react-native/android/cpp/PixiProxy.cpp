#include "PixiProxy.h"
#include "PixiHostObject.h"

namespace Pixi
{

  using namespace facebook::jni;

  jboolean PixiProxy::installJsiRuntime(jni::alias_ref<jni::JClass>, jlong runtimePtr)
  {
    auto runtime = reinterpret_cast<jsi::Runtime *>(runtimePtr);
    if (runtime == nullptr)
    {
      return false;
    }

    auto PixiProxyInstaller = jsi::Function::createFromHostFunction(*runtime,
                                                                    jsi::PropNameID::forAscii(*runtime, "PixiProxy"),
                                                                    1,
                                                                    [](jsi::Runtime &runtime,
                                                                       const jsi::Value &thisValue,
                                                                       const jsi::Value *arguments, size_t count) -> jsi::Value
                                                                    {
            auto pixiHostObj = std::make_shared<PixiHostObject>(runtime);
            return jsi::Object::createFromHostObject(runtime, pixiHostObj); });

    runtime->global().setProperty(*runtime, "__PixiProxy", PixiProxyInstaller);
    return true;
  }

  void PixiProxy::registerNatives()
  {
    javaClassStatic()->registerNatives({makeNativeMethod("installJsiRuntime", PixiProxy::installJsiRuntime)});
  }
}
