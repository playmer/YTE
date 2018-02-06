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

#include "YTE/GameComponents/MenuController.hpp"

namespace YTE
{
	YTEDefineEvent(MenuElementHover);
	YTEDefineEvent(MenuElementTrigger);

	YTEDefineType(MenuElementHover) { YTERegisterType(MenuElementHover); }
	YTEDefineType(MenuElementTrigger) { YTERegisterType(MenuElementTrigger); }

	YTEDefineType(MenuController)
	{
		YTERegisterType(MenuController);

		YTEBindProperty(&GetNumMenuElements, &SetNumMenuElements, "NumMenuElements")
			.AddAttribute<EditorProperty>()
			.AddAttribute<Serializable>();

		YTEBindProperty(&GetFirstMenuElement, &SetFirstMenuElement, "FirstMenuElement")
			.AddAttribute<EditorProperty>()
			.AddAttribute<Serializable>();

		YTEBindProperty(&GetSecondMenuElement, &SetSecondMenuElement, "SecondMenuElement")
			.AddAttribute<EditorProperty>()
			.AddAttribute<Serializable>();
	}

	MenuController::MenuController(Composition* aOwner, Space* aSpace, RSValue* aProperties) : Component(aOwner, aSpace), mConstructing(true)
	{
		DeserializeByType(aProperties, this, GetStaticType());

		mMenuElements[0] = nullptr;
		mMenuElements[1] = nullptr;

		mNumMenuElements = 0;
		mCurrMenuElement = 0;
		mIsDisplayed = false;

		mConstructing = false;
	}

	void MenuController::Initialize()
	{
		mGamePad = mOwner->GetEngine()->GetGamepadSystem()->GetXboxController(YTE::Controller_Id::Xbox_P1);

		mGamePad->YTERegister(Events::XboxStickEvent, this, &MenuController::OnXboxStickEvent);
		mGamePad->YTERegister(Events::XboxButtonPress, this, &MenuController::OnXboxButtonPress);
	}

	void MenuController::OnXboxStickEvent(XboxStickEvent* aEvent)
	{
		if (mIsDisplayed && (aEvent->StickDirection.x < 0.01f || aEvent->StickDirection.x > 0.01f))
			mCurrMenuElement = (mCurrMenuElement + 1) % 2;
	}

	void MenuController::OnXboxButtonPress(XboxButtonEvent* aEvent)
	{
		switch (aEvent->Button)
		{
		case (Xbox_Buttons::Start):
		{
			mIsDisplayed = !mIsDisplayed;

			// Actually bring up the sprites here

			break;
		}

		case (Xbox_Buttons::A):
		{
			if (mIsDisplayed)
			{
				MenuElementTrigger triggerEvent;
				mMenuElements[mCurrMenuElement]->SendEvent("MenuElementTrigger", &triggerEvent);
			}
		}
		}
	}
}
