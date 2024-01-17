#pragma once

#include <ReactCommon/CallInvokerHolder.h>
#include <fbjni/fbjni.h>
#include <jsi/jsi.h>

#include <memory>
#include <string>


namespace Pixi
{

  using namespace facebook;

  class PixiProxy : public jni::HybridClass<PixiProxy>
  {
  public:
    ~PixiProxy();
    static void registerNatives();
    jsi::Runtime *getJSRuntime();
    void sayHello(int number);

  private:
    friend HybridBase;
    jni::global_ref<PixiProxy::javaobject> _javaPart;
    jsi::Runtime *_runtime;

    static auto constexpr TAG = "PixiProxy";
    static auto constexpr kJavaDescriptor = "Lcom/example/pixi/PixiProxy;";

    explicit PixiProxy(const jni::alias_ref<PixiProxy::jhybridobject> &javaThis, jsi::Runtime *jsRuntime);
    static jni::local_ref<jhybriddata> initHybrid(jni::alias_ref<jhybridobject> javaThis, jlong jsRuntimePointer);
  };

}