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
		mIsDisplayed = false;

		DeserializeByType(aProperties, this, GetStaticType());
		mConstructing = false;
	}

	void MenuController::Initialize()
	{ 
    mOwner->YTERegister(Events::MenuStart, this, &MenuController::OnMenuStart);
		mOwner->YTERegister(Events::MenuExit, this, &MenuController::OnDirectMenuExit);

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
		if (aEvent->ParentMenu)
			mParentMenu = aEvent->ParentMenu;

    auto emitter = mOwner->GetComponent<WWiseEmitter>();

    mMyTransform->SetScale(-16.5, 9.5f, 1.f);
		mIsDisplayed = true;
    emitter->PlayEvent("UI_Menu_Pause");

    mCurrMenuElement = 0;

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
			auto emitter = mOwner->GetComponent<WWiseEmitter>();

			mMyTransform->SetScale(0.f, 0.f, 0.f);
			mIsDisplayed = false;
			emitter->PlayEvent("UI_Menu_Unpause");

			if (mMenuElements->size() != 0)
			{
				MenuElementDeHover deHoverEvent;

				auto currElement = mMenuElements->begin() + mCurrMenuElement;
				currElement->second->SendEvent(Events::MenuElementDeHover, &deHoverEvent);
				mCurrMenuElement = 0;
			}
		}
	}

  void MenuController::OnMenuExit(MenuExit *aEvent)
  {
		if (mIsDisplayed)
		{
			auto emitter = mOwner->GetComponent<WWiseEmitter>();

			mMyTransform->SetScale(0.f, 0.f, 0.f);
			mIsDisplayed = false;
			emitter->PlayEvent("UI_Menu_Unpause");

			if (mMenuElements->size() != 0)
			{
				MenuElementDeHover deHoverEvent;

				auto currElement = mMenuElements->begin() + mCurrMenuElement;
				currElement->second->SendEvent(Events::MenuElementDeHover, &deHoverEvent);
				mCurrMenuElement = 0;
			}

				// Pop up to the owning menu
			if (!aEvent->ShouldExitAll)
			{
					// Opens the parent menu (aka parent of parent button)
				if (mParentMenu)
				{
					MenuStart menuStart;
					mParentMenu->SendEvent(Events::MenuStart, &menuStart);
				}

					// Return context to the game if there aren't any parent menus to open
				else
				{
					mSpace->GetComponent<InputInterpreter>()->SetInputContext(InputInterpreter::InputContext::Sailing);
				}
			}
			else
			{
				mSpace->GetComponent<InputInterpreter>()->SetInputContext(InputInterpreter::InputContext::Sailing);
			}
		}
  }

  void MenuController::OnMenuConfirm(MenuConfirm *aEvent)
  {
		if (mIsDisplayed && mMenuElements->size() != 0)
		{
			if (!aEvent->IsReleased)
			{
				mOwner->GetComponent<WWiseEmitter>()->PlayEvent("UI_Menu_Select");

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
				mCurrMenuElement = (mCurrMenuElement <= 0) ? (mNumElements - 1) : (mCurrMenuElement - 1);

			else if (aEvent->ChangeDirection == MenuElementChange::Direction::Next)
				mCurrMenuElement = (mCurrMenuElement + 1) % mNumElements;

			auto emitter = mOwner->GetComponent<WWiseEmitter>();
			emitter->PlayEvent("UI_Menu_Hover");

			MenuElementHover hoverEvent;

			currElement = mMenuElements->begin() + mCurrMenuElement;
			currElement->second->SendEvent(Events::MenuElementHover, &hoverEvent);
		}
  }
}
