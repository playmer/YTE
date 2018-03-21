/******************************************************************************/
/*!
* \author Isaac Dayton
* \date   2018/02/05
*
* \copyright All content 2018 DigiPen (USA) Corporation, all rights reserved.
*/
/******************************************************************************/

#include "YTE/Core/Engine.hpp"
#include "YTE/Core/Space.hpp"

#include "YTE/Physics/Orientation.hpp"
#include "YTE/GameComponents/Menu/MenuController.hpp"
#include "YTE/WWise/WWiseSystem.hpp"

namespace YTE
{
  YTEDefineEvent(MenuElementHover);
  YTEDefineEvent(MenuElementTrigger);
  YTEDefineEvent(MenuElementDeHover);

  YTEDefineType(MenuElementHover) { YTERegisterType(MenuElementHover); }
  YTEDefineType(MenuElementTrigger) { YTERegisterType(MenuElementTrigger); }
  YTEDefineType(MenuElementDeHover) { YTERegisterType(MenuElementHover); }

  YTEDefineType(MenuController)
  {
    YTERegisterType(MenuController);

    YTEBindProperty(&GetDisplayed, &SetDisplayed, "IsDisplayed")
      .AddAttribute<Serializable>()
      .AddAttribute<EditorProperty>();
  }

  MenuController::MenuController(Composition* aOwner, Space* aSpace, RSValue* aProperties) : Component(aOwner, aSpace), mConstructing(true)
  {
    mCurrMenuElement = 0;
    mIsDisplayed = false;

    DeserializeByType(aProperties, this, GetStaticType());
    mConstructing = false;
  }

  void MenuController::Initialize()
  {
    mSoundEmitter = mOwner->GetComponent<WWiseEmitter>();
    mMenuElements = mOwner->GetCompositions();
    mNumElements = static_cast<int>(mMenuElements->size());

    mMySprite = mOwner->GetComponent<Sprite>();

      // Cache sound ids used by this component
    auto soundSystem = mOwner->GetEngine()->GetComponent<WWiseSystem>();

    if (soundSystem)
    {
      mSoundPause = soundSystem->GetSoundIDFromString("UI_Menu_Pause");
      mSoundUnpause = soundSystem->GetSoundIDFromString("UI_Menu_Unpause");
      mSoundElementNext = soundSystem->GetSoundIDFromString("UI_Menu_Up");
      mSoundElementPrev = soundSystem->GetSoundIDFromString("UI_Menu_Down");
      mSoundElementSelect = soundSystem->GetSoundIDFromString("UI_Menu_Select");
    }

    mSpace->YTERegister(Events::LogicUpdate, this, &MenuController::OnChildrenInitialized);

    mOwner->YTERegister(Events::MenuStart, this, &MenuController::OnMenuStart);
    mOwner->YTERegister(Events::MenuExit, this, &MenuController::OnDirectMenuExit);

    mSpace->YTERegister(Events::MenuExit, this, &MenuController::OnMenuExit);
    mSpace->YTERegister(Events::MenuConfirm, this, &MenuController::OnMenuConfirm);
    mSpace->YTERegister(Events::MenuElementChange, this, &MenuController::OnMenuElementChange);
  }

  void MenuController::OnChildrenInitialized(LogicUpdate *aEvent)
  {
    YTEUnusedArgument(aEvent);
    UpdateVisibility();
    mSpace->YTEDeregister(Events::LogicUpdate, this, &MenuController::OnChildrenInitialized);
  }

  void MenuController::OnMenuStart(MenuStart *aEvent)
  {
    if (aEvent->ParentMenu)
    {
      mParentMenu = aEvent->ParentMenu;
    }

    if (aEvent->ResetCursor)
    {
      mCurrMenuElement = 0;
    }

    mIsDisplayed = true;
    UpdateVisibility();

    if (aEvent->PlaySound)
    {
      mSoundEmitter->PlayEvent(mSoundPause);
    }

    //mCurrMenuElement = 0;

    if (mMenuElements->size() != 0)
    {
      MenuElementHover hoverEvent;

      auto currElement = mMenuElements->begin() + mCurrMenuElement;
      currElement->second->SendEvent(Events::MenuElementHover, &hoverEvent);
    }
  }

  void MenuController::OnDirectMenuExit(MenuExit *aEvent)
  {
    if (mIsDisplayed)
    {
      mIsDisplayed = false;
      UpdateVisibility();

      if (aEvent->PlaySound)
      {
        mSoundEmitter->PlayEvent(mSoundUnpause);
      }

      if (mMenuElements->size() != 0)
      {
        MenuElementDeHover deHoverEvent;

        auto currElement = mMenuElements->begin() + mCurrMenuElement;
        currElement->second->SendEvent(Events::MenuElementDeHover, &deHoverEvent);
        //mCurrMenuElement = 0;
      }
    }
  }

  void MenuController::OnMenuExit(MenuExit *aEvent)
  {
    if (mIsDisplayed && !aEvent->Handled)
    {
      mIsDisplayed = false;
      UpdateVisibility();

      if (aEvent->PlaySound)
      {
        mSoundEmitter->PlayEvent(mSoundUnpause);
      }

      if (mMenuElements->size() != 0)
      {
        MenuElementDeHover deHoverEvent;

        auto currElement = mMenuElements->begin() + mCurrMenuElement;
        currElement->second->SendEvent(Events::MenuElementDeHover, &deHoverEvent);
        //mCurrMenuElement = 0;
      }

        // Pop up to the owning menu
      if (!aEvent->ShouldExitAll)
      {
        aEvent->Handled = true;

          // Opens the parent menu
        if (mParentMenu)
        {
          MenuStart menuStart;
          mParentMenu->SendEvent(Events::MenuStart, &menuStart);
        }

          // Return context to the game if there aren't any parent menus to open
        else
        {
          aEvent->ContextSwitcher->SetInputContext(InputInterpreter::InputContext::Sailing);
        }
      }
      else
      {
        aEvent->ContextSwitcher->SetInputContext(InputInterpreter::InputContext::Sailing);
      }
    }
  }

  void MenuController::OnMenuConfirm(MenuConfirm *aEvent)
  {
    if (!aEvent->IsHandled && mIsDisplayed && mMenuElements->size() != 0)
    {
      if (!aEvent->IsReleased)
      {
        aEvent->IsHandled = true;
        
        mSoundEmitter->PlayEvent(mSoundElementSelect);

        MenuElementTrigger triggerEvent;

        auto currElement = mMenuElements->begin() + mCurrMenuElement;
        currElement->second->SendEvent(Events::MenuElementTrigger, &triggerEvent);
      }

      else
      {
        MenuElementHover hoverEvent;

        auto currElement = mMenuElements->begin() + mCurrMenuElement;
        currElement->second->SendEvent(Events::MenuElementHover, &hoverEvent);
      }
    }
  }

  void MenuController::OnMenuElementChange(MenuElementChange *aEvent)
  {
    if (mIsDisplayed && mMenuElements->size() != 0)
    {
      MenuElementDeHover deHoverEvent;

      auto currElement = mMenuElements->begin() + mCurrMenuElement;

      currElement->second->SendEvent(Events::MenuElementDeHover, &deHoverEvent);

      if (aEvent->ChangeDirection == MenuElementChange::Direction::Previous)
      {
        mCurrMenuElement = (mCurrMenuElement <= 0) ? (mNumElements - 1) : (mCurrMenuElement - 1);
        mSoundEmitter->PlayEvent(mSoundElementPrev);
      }
      else if (aEvent->ChangeDirection == MenuElementChange::Direction::Next)
      {
        mCurrMenuElement = (mCurrMenuElement + 1) % mNumElements;
        mSoundEmitter->PlayEvent(mSoundElementNext);
      }

      MenuElementHover hoverEvent;

      currElement = mMenuElements->begin() + mCurrMenuElement;
      currElement->second->SendEvent(Events::MenuElementHover, &hoverEvent);
    }
  }

  void MenuController::UpdateVisibility()
  {
    if (mMySprite != nullptr)
    {
      mMySprite->SetVisibility(mIsDisplayed);
    }

    auto children = mMenuElements->All();

    for (auto &child : children)
    {
      Sprite* childSprite = child.second->GetComponent<Sprite>();

      if (childSprite != nullptr)
      {
        childSprite->SetVisibility(mIsDisplayed);
      }
    }
  }
}
