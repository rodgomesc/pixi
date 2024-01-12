#pragma once

#include <jsi/jsi.h>

namespace Pixi
{

  class PixiHostObject : public facebook::jsi::HostObject
  {
  public:
    PixiHostObject(facebook::jsi::Runtime &runtime);
    virtual ~PixiHostObject();

    facebook::jsi::Value get(facebook::jsi::Runtime &runtime, const facebook::jsi::PropNameID &propNameId) override;
    std::vector<facebook::jsi::PropNameID> getPropertyNames(facebook::jsi::Runtime &runtime) override;
  };

} // namespace Pixi
