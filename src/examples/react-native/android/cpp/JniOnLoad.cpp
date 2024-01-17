#include <fbjni/fbjni.h>
#include <jni.h>
#include "PixiInstaller.h"

JNIEXPORT jint JNICALL JNI_OnLoad(JavaVM *vm, void *) {
    return facebook::jni::initialize(vm, [] {
        Pixi::PixiProxy::registerNatives();
        Pixi::PixiInstaller::registerNatives();
    });
}
