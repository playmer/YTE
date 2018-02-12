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
    YTEUnusedArgument(aProperties);
  }

  void SampleAction::Initialize()
  {
    mSpace->YTERegister(Events::LogicUpdate, this, &SampleAction::Update);
    mValue = 10.0f;
  }

  void SampleAction::Update(LogicUpdate* aLogicUpdate)
  {
    YTEUnusedArgument(aLogicUpdate);
    if (mValue == 10.0f)
    {
      mValue = 0.0f;
      mSimultaneousVal1 = 30.0f;
      mSimultaneousVal2 = -7.0f;
      auto manager = mSpace->GetComponent<ActionManager>();
      ActionSequence s;

      ActionGroup g; // groups are actions that occur simultaneously
      g.Add<Cubic::easeOut>(mSimultaneousVal1, 3.0f, 0.5f);
      g.Add<Quad::easeInOut>(mSimultaneousVal2, -36.0f, 1.5f);
      s.AddGroup(g);

      int someValue = 3.0f;
      s.Delay(0.5f);
      s.Call([someValue, this]() {
        std::cout << "Hey I'm a function callback that has been called! Look at this value:" << someValue << std::endl;
        std::cout << "Look at this value from the component this pointer:" << mValue << std::endl;
      });

      s.Add<Sine::easeIn>(mValue, 10.0f, 0.5f);
      manager->AddSequence(mOwner, s);
    }
  }
}



