#pragma once

#include "YTE/Core/Component.hpp"

#include "YTE/Core/Component.hpp"

#include "YTE/GameComponents/Menu/LaunchCredits.hpp"

namespace YTE
{
  class LoadingScreen : public Component
  {
  public:
    YTEDeclareType(LoadingScreen);

    LoadingScreen(Composition *aOwner, Space *aSpace, RSValue *aProperties);

    void Start() override;

    void Update(LogicUpdate *aUpdate);

  private:
    void CreateLineInternal(char const *aString, float aFontSize);

    int mCurrentNumber;

    Composition *mLastLine;
  };
}