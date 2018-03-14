/******************************************************************************/
/*!
\file   TestingComponent.hpp
\author Nicholas Ammann
\brief
This component is just for calling functinos to test various gameplay functionality (such as playing animations).

All content (c) 2016 DigiPen  (USA) Corporation, all rights reserved.
*/
/******************************************************************************/

#pragma once

#include "YTE/Core/Component.hpp"
#include "YTE/Graphics/Animation.hpp"
#include "YTE/Physics/Transform.hpp"
#include "YTE/Physics/Orientation.hpp"
#include "YTE/GameComponents/InputInterpreter.hpp"

namespace YTE
{
  class TestingComponent : public Component
  {
  public:
    YTEDeclareType(TestingComponent);
    TestingComponent(Composition *aOwner, Space *aSpace, RSValue *aProperties);
    void Initialize() override;
    void Update(LogicUpdate *aEvent);
  
    void CheckButtons(XboxButtonEvent *aEvent);

    std::string GetCurrentAnimation() const;
    void SetCurrentAnimation(std::string aAnimation);

  private:
    XboxController *mGamepad;

    Animator *mAnimator;

    double mAnimLoopTimer;

    std::string mCurrentAnimation;

    bool mAnimForward;

  };
}//end yte namespace