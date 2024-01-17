#pragma once

#include <jsi/jsi.h>
#include <fbjni/fbjni.h>
#include "PixiProxy.h"

namespace Pixi
{
    using namespace facebook;

    class PixiHostObject : public facebook::jsi::HostObject
    {
    public:
        explicit PixiHostObject(const jni::alias_ref<PixiProxy::javaobject> &pixiProxy);

        ~PixiHostObject();

        facebook::jsi::Value
        get(facebook::jsi::Runtime &runtime, const facebook::jsi::PropNameID &propNameId) override;

        std::vector<facebook::jsi::PropNameID>
        getPropertyNames(facebook::jsi::Runtime &runtime) override;

    private:
        jni::global_ref<PixiProxy::javaobject> _pixiProxy;
    };

} // namespace Pixi
