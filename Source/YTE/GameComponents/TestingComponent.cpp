/******************************************************************************/
/*!
\file   TestingComponent.cpp
\author NicholasAmmann

All content (c) 2016 DigiPen  (USA) Corporation, all rights reserved.
*/
/******************************************************************************/

#include "YTE/GameComponents/TestingComponent.hpp"

#include "YTE/Graphics/Animation.hpp"

namespace YTE
{
  YTEDefineType(TestingComponent)
  {
    YTERegisterType(TestingComponent);
  }

  TestingComponent::TestingComponent(Composition *aOwner, Space *aSpace, RSValue *aProperties)
    : Component(aOwner, aSpace)
  {
    YTEUnusedArgument(aProperties);
  }

  void TestingComponent::Initialize()
  {
    mOwner->GetSpace()->YTERegister(Events::LogicUpdate, this, &TestingComponent::Update);

    mGamepad = mOwner->GetEngine()->GetGamepadSystem()->GetXboxController(YTE::Controller_Id::Xbox_P1);
    mGamepad->YTERegister(Events::XboxButtonPress, this, &TestingComponent::CheckButtons);
  }

  void TestingComponent::Update(LogicUpdate *aEvent)
  {

  }

  void TestingComponent::CheckButtons(XboxButtonEvent * aEvent)
  {
    switch (aEvent->Button)
    {
    case Xbox_Buttons::A:
    {
      break;
    }
    }
  }

}// end yte namespace