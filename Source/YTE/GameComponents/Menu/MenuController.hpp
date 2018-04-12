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

#include "YTE/GameComponents/InputInterpreter.hpp"

#include "YTE/Graphics/Sprite.hpp"

#include "YTE/WWise/WWiseEmitter.hpp"

namespace YTE
{
  YTEDeclareEvent(MenuElementHover);
  YTEDeclareEvent(MenuElementTrigger);
  YTEDeclareEvent(MenuElementDeHover);

  class MenuElementHover : public Event
  {
  public:
    YTEDeclareType(MenuElementHover);
  };

  class MenuElementTrigger : public Event
  {
  public:
    YTEDeclareType(MenuElementTrigger);
  };

  class MenuElementDeHover : public Event
  {
  public:
    YTEDeclareType(MenuElementDeHover);
  };

  class MenuController : public Component
  {
  public:
    YTEDeclareType(MenuController);
    MenuController(Composition *aOwner, Space *aSpace, RSValue *aProperties);

    void Initialize() override;
    // void CloseMenu();

    // PROPERTIES /////////////////////////////////////////
    bool GetDisplayed() { return mIsDisplayed; }
    void SetDisplayed(bool aDisplay) { mIsDisplayed = aDisplay; }

    int GetNumElements() { return mNumElements; }
    void SetNumElements(int aNumElements) { mNumElements = aNumElements; }
    ///////////////////////////////////////////////////////
    void OnChildrenInitialized(LogicUpdate *aEvent);

    void OnMenuStart(MenuStart *aEvent);
    void OnDirectMenuExit(MenuExit *aEvent);
    void OnMenuExit(MenuExit *aEvent);
    void OnMenuConfirm(MenuConfirm *aEvent);
    void OnMenuElementChange(MenuElementChange *aEvent);

    InputInterpreter* GetContextSwitcher() const { return mContextSwitcher; }

  private:
    WWiseEmitter* mSoundEmitter;
    Composition* mParentMenu = nullptr;
    InputInterpreter* mContextSwitcher = nullptr;

    Sprite* mMySprite;

    int mCurrMenuElement;
    int mNumElements;
    YTE::CompositionMap* mMenuElements;

    u64 mSoundPause;
    u64 mSoundUnpause;
    u64 mSoundMenuBack;
    u64 mSoundElementNext;
    u64 mSoundElementPrev;
    u64 mSoundElementSelect;

    bool mIsDisplayed;

    bool mConstructing;

    void UpdateVisibility();
  };
}
