#pragma once

#include "YTE/Core/Component.hpp"

#include "YTE/Core/Component.hpp"

#include "YTE/GameComponents/Menu/LaunchCredits.hpp"

namespace YTE
{
  class MakeLoadingScreen : public Component
  {
  public:
    YTEDeclareType(MakeLoadingScreen);

    MakeLoadingScreen(Composition *aOwner, Space *aSpace, RSValue *aProperties);
    void NativeInitialize() override;
    void Update(LogicUpdate *aUpdate);

  private:
    Composition *mChild;
  };
}