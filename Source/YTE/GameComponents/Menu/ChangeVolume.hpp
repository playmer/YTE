/******************************************************************************/
/*!
* \author Isaac Dayton
* \date   2018/04/17
*
* \copyright All content 2018 DigiPen (USA) Corporation, all rights reserved.
*/
/******************************************************************************/

#pragma once

#include "YTE/Core/Composition.hpp"
#include "YTE/Core/ForwardDeclarations.hpp"
#include "YTE/Core/Engine.hpp"

#include "YTE/Physics/Transform.hpp"

#include "YTE/WWise/WWiseSystem.hpp"

#include "YTE/GameComponents/Menu/MenuController.hpp"

namespace YTE
{
  class ChangeVolume : public Component
  {
  public:
    YTEDeclareType(ChangeVolume);
    ChangeVolume(Composition *aOwner, Space *aSpace, RSValue *aProperties);

    void Initialize() override;
    void Start() override;

    // PROPERTIES /////////////////////////////////////////
    ////////////////////////////////////////////////////////

    void OnLogicUpdate(LogicUpdate *aEvent);
    void OnStickEvent(OptionsStickEvent *aEvent);
    void OnConfirm(OptionsConfirmEvent *aEvent);

    void OnElementTrigger(MenuElementTrigger *);

  private:
    WWiseSystem *mSoundSystem;

    Transform *mMyTransform;
    Transform *mMaskTransform;

    glm::vec2 mSliderRange;

    float mLastStickX;
    float mFillRatio;

    void UpdateSlider();
  };
}