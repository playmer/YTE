#include "YTE/Core/Actions/ActionManager.hpp"
#include "YTE/GameComponents/SampleAction.hpp"

namespace YTE
{
  YTEDefineType(SampleAction)
  {
    YTERegisterType(SampleAction);
  }

  SampleAction::SampleAction(Composition *aOwner, Space * aSpace, RSValue *aProperties)
    : Component(aOwner, aSpace)
  {

  }

  void SampleAction::Initialize()
  {
    mSpace->YTERegister(Events::LogicUpdate, this, &SampleAction::Update);
    mValue = 0.0f;
    mEndValue = 10.0f;
    mDuration = 1.0f;
  }

  void SampleAction::Update(LogicUpdate* aLogicUpdate)
  {
    if (mValue != mEndValue)
    {
      auto manager = mSpace->GetComponent<ActionManager>();
      ActionSequence s;
      s.Add<Sine::easeIn>(mValue, mEndValue, mDuration);
      manager->AddSequence(mOwner, s);
    }
  }
}



