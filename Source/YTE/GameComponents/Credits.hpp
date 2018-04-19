#pragma once

#include "YTE/Core/Component.hpp"

#include "YTE/Core/Component.hpp"

#include "YTE/GameComponents/Menu/LaunchCredits.hpp"

namespace YTE
{
  class Credits : public Component
  {
  public:
    YTEDeclareType(Credits);

    Credits(Composition *aOwner, Space *aSpace, RSValue *aProperties);

    void Start() override;

    void OnStartCredits(StartCredits *);
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

    bool mFadeFinished;
    float mFadeValue;

    GraphicsView *mBlackoutView;
    GraphicsView *mMyView;
    Composition *mFirstLine;
    Composition *mLastLine;
  };
}