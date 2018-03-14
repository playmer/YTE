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

    YTEBindProperty(&TestingComponent::GetCurrentAnimation, &TestingComponent::SetCurrentAnimation, "Current Animation")
      .AddAttribute<EditorProperty>()
      .AddAttribute<Serializable>();
  }

  TestingComponent::TestingComponent(Composition *aOwner, Space *aSpace, RSValue *aProperties)
    : Component(aOwner, aSpace)
    , mAnimLoopTimer(0.0)
    , mCurrentAnimation("")
    , mAnimForward(true)
  {
    YTEUnusedArgument(aProperties);
  }

  void TestingComponent::Initialize()
  {
    mOwner->GetSpace()->YTERegister(Events::LogicUpdate, this, &TestingComponent::Update);

    mGamepad = mOwner->GetEngine()->GetGamepadSystem()->GetXboxController(YTE::Controller_Id::Xbox_P1);
    mGamepad->YTERegister(Events::XboxButtonPress, this, &TestingComponent::CheckButtons);

    mAnimator = mOwner->GetComponent<Animator>();
  }

  void TestingComponent::Update(LogicUpdate *aEvent)
  {
    YTEUnusedArgument(aEvent);

    if (mCurrentAnimation.empty())
    {
      auto anims = mAnimator->GetAnimations();

      mCurrentAnimation = anims.begin()->first;
    }

    mAnimator->SetCurrentAnimation(mCurrentAnimation);
    mAnimator->SetCurrentPlayOverTime(false);

    double maxTime = mAnimator->GetMaxTime();

    if (mAnimForward)
    {
      mAnimLoopTimer += aEvent->Dt;
    }
    else
    {
      mAnimLoopTimer -= aEvent->Dt;
    }

    if (mAnimLoopTimer > maxTime)
    {
      mAnimLoopTimer = maxTime;
      mAnimForward = false;
    }
    else if (mAnimLoopTimer < 0.0)
    {
      mAnimLoopTimer = 0.0;
      mAnimForward = true;
    }

    mAnimator->SetCurrentAnimTime(mAnimLoopTimer);
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

  std::string TestingComponent::GetCurrentAnimation() const
  {
    return mCurrentAnimation;
  }

  void TestingComponent::SetCurrentAnimation(std::string aAnimation)
  {
    mCurrentAnimation = aAnimation;
  }

}// end yte namespace