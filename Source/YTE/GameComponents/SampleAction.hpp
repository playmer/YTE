#pragma once

#include "YTE/Core/Component.hpp"
#include "YTE/Core/Engine.hpp"

namespace YTE
{
  class SampleAction : public Component
  {
  public:
    YTEDeclareType(SampleAction);
    SampleAction(Composition *aOwner, Space * aSpace, RSValue *aProperties);
    void Initialize() override;
    void Update(LogicUpdate* aLogicUpdate);
  private:
    float mSimultaneousVal1;
    float mSimultaneousVal2;
    float mValue;
  };
}
