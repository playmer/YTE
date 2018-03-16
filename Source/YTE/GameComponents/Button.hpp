/******************************************************************************/
/*!
* \author Isaac Dayton
* \date   2018/02/05
*
* \copyright All content 2018 DigiPen (USA) Corporation, all rights reserved.
*/
/******************************************************************************/

#pragma once

#include "YTE/Core/Composition.hpp"
#include "YTE/Core/ForwardDeclarations.hpp"
#include "YTE/Core/Engine.hpp"

#include "YTE/Graphics/Sprite.hpp"

#include "YTE/Platform/Gamepad.hpp"
#include "YTE/Platform/GamepadSystem.hpp"
#include "YTE/Platform/DeviceEnums.hpp"

#include "YTE/GameComponents/UI/MenuController.hpp"

namespace YTE
{
  class Button : public Component
  {
  public:
    YTEDeclareType(Button);
    Button(Composition *aOwner, Space *aSpace, RSValue *aProperties);

    void Initialize() override;

    // PROPERTIES /////////////////////////////////////////
    std::string GetHover() { return mHoverSpriteName; }
    void SetHover(std::string& aHoverName) { mHoverSpriteName = aHoverName; }

    std::string GetActivated() { return mActivatedSpriteName; }
    void SetActivated(std::string& aActivatedName) { mActivatedSpriteName = aActivatedName; }

    std::string GetNeutral() { return mNeutralSpriteName; }
    void SetNeutral(std::string& aNeutralName) { mNeutralSpriteName = aNeutralName; }
    ////////////////////////////////////////////////////////

    void OnButtonHover(MenuElementHover* aEvent);
    void OnButtonTrigger(MenuElementTrigger* aEvent);
    void OnButtonDeHover(MenuElementDeHover* aEvent);

  private:
    // Store whatever button properties make sense
    // Probably an audio queue and some state sprites or something
    std::string mHoverSpriteName;
    std::string mActivatedSpriteName;
    std::string mNeutralSpriteName;

    Sprite* mCurrentSprite = nullptr;

    bool mConstructing;
  };
}
