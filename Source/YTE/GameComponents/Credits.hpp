#pragma once

#include "YTE/Core/Component.hpp"

#include "YTE/Core/Component.hpp"

namespace YTE
{
  class Credits : public Component
  {
  public:
    YTEDeclareType(Credits);

    Credits(Composition *aOwner, Space *aSpace, RSValue *aProperties);

    void Start() override;

    void Update(LogicUpdate *aUpdate);

    void CreateLine(char const *aString);
    void CreateHeader(char const *aString);
    void AddSection();

    bool GetDone();

  private:
    void CreateLineInternal(char const *aString, float aFontSize);

    float mDelay;
    float mY;
    int mCurrentNumber;
    bool mDone;

    Composition *mLastLine;
  };
}