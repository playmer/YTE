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
#include "YTE/GameComponents/MenuController.hpp"
#include "YTE/WWise/WWiseEmitter.hpp"

namespace YTE
{
	YTEDefineEvent(MenuElementHover);
	YTEDefineEvent(MenuElementTrigger);
	YTEDefineEvent(MenuElementDeHover)

	YTEDefineType(MenuElementHover) { YTERegisterType(MenuElementHover); }
	YTEDefineType(MenuElementTrigger) { YTERegisterType(MenuElementTrigger); }
	YTEDefineType(MenuElementDeHover) { YTERegisterType(MenuElementHover); }

	YTEDefineType(MenuController)
	{
		YTERegisterType(MenuController);
	}

	MenuController::MenuController(Composition* aOwner, Space* aSpace, RSValue* aProperties) : Component(aOwner, aSpace), mConstructing(true)
	{
		mCurrMenuElement = 0;

		DeserializeByType(aProperties, this, GetStaticType());
		mConstructing = false;
	}

	void MenuController::Initialize()
	{ 
    mSpace->YTERegister(Events::MenuStart, this, &MenuController::OnMenuStart);
    mSpace->YTERegister(Events::MenuExit, this, &MenuController::OnMenuExit);
    mSpace->YTERegister(Events::MenuConfirm, this, &MenuController::OnMenuConfirm);
    mSpace->YTERegister(Events::MenuElementChange, this, &MenuController::OnMenuElementChange);
	
		mMenuElements = mOwner->GetCompositions();
		mNumElements = static_cast<int>(mMenuElements->size());

		mMyTransform = mOwner->GetComponent<Transform>();
		//mViewScale = mMyTransform->GetScale();
		mMyTransform->SetScale(0.f, 0.f, 0.f);
	}

  void MenuController::OnMenuStart(MenuStart *aEvent)
  {
      // Check that we are the correct menu to be opened
    if (mOwner->GetName().c_str() == aEvent->MenuName)
    {
      auto emitter = mOwner->GetComponent<WWiseEmitter>();

      mMyTransform->SetScale(-16.5, 9.5f, 1.f);
      emitter->PlayEvent("UI_Menu_Pause");

      mCurrMenuElement = 0;

      MenuElementHover hoverEvent;

      auto currElement = mMenuElements->begin() + mCurrMenuElement;
      currElement->second->SendEvent("MenuElementHover", &hoverEvent);
    }
  }

  void MenuController::OnMenuExit(MenuExit *aEvent)
  {
    auto emitter = mOwner->GetComponent<WWiseEmitter>();

    mMyTransform->SetScale(0.f, 0.f, 0.f);
    emitter->PlayEvent("UI_Menu_Unpause");

    MenuElementDeHover deHoverEvent;

    auto currElement = mMenuElements->begin() + mCurrMenuElement;
    currElement->second->SendEvent("MenuElementDeHover", &deHoverEvent);
    mCurrMenuElement = 0;

      // Pop up to the owning menu
    if (!aEvent->ShouldExitAll)
    {
        // Opens the parent menu (aka parent of parent button)
      MenuStart menuStart(mOwner->GetParent()->GetParent()->GetName().c_str());
      mOwner->GetEngine()->SendEvent(Events::MenuStart, &menuStart);
    }

    if (!mParentMenu)
    {
      mSpace->GetComponent<InputInterpreter>()->SetInputContext(InputInterpreter::InputContext::Sailing);
    }
  }

  void MenuController::OnMenuConfirm(MenuConfirm *aEvent)
  {
    if (!aEvent->IsReleased)
    {
      mOwner->GetComponent<WWiseEmitter>()->PlayEvent("UI_Menu_Select");

      MenuElementTrigger triggerEvent;

      auto currElement = mMenuElements->begin() + mCurrMenuElement;
      currElement->second->SendEvent("MenuElementTrigger", &triggerEvent);
    }

    else
    {
      MenuElementHover hoverEvent;

      auto currElement = mMenuElements->begin() + mCurrMenuElement;
      currElement->second->SendEvent("MenuElementHover", &hoverEvent);
    }
  }

  void MenuController::OnMenuElementChange(MenuElementChange *aEvent)
  {
    MenuElementDeHover deHoverEvent;

    auto currElement = mMenuElements->begin() + mCurrMenuElement;
    currElement->second->SendEvent("MenuElementDeHover", &deHoverEvent);

    if (aEvent->ChangeDirection == MenuElementChange::Direction::Previous)
      mCurrMenuElement = (mCurrMenuElement <= 0) ? (mNumElements - 1) : (mCurrMenuElement - 1);

    else if (aEvent->ChangeDirection == MenuElementChange::Direction::Next)
      mCurrMenuElement = (mCurrMenuElement + 1) % mNumElements;

    auto emitter = mOwner->GetComponent<WWiseEmitter>();
    emitter->PlayEvent("UI_Menu_Hover");

    MenuElementHover hoverEvent;

    currElement = mMenuElements->begin() + mCurrMenuElement;
    currElement->second->SendEvent("MenuElementHover", &hoverEvent);
  }
}
