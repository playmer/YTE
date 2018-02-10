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
		mGamePad = mOwner->GetEngine()->GetGamepadSystem()->GetXboxController(YTE::Controller_Id::Xbox_P1);

		mGamePad->YTERegister(Events::XboxStickFlicked, this, &MenuController::OnXboxFlickEvent);
		mGamePad->YTERegister(Events::XboxButtonPress, this, &MenuController::OnXboxButtonPress);
		mGamePad->YTERegister(Events::XboxButtonRelease, this, &MenuController::OnXboxButtonRelease);
	
		mMenuElements = mOwner->GetCompositions();
		mNumElements = mMenuElements->size();

		mMyTransform = mOwner->GetComponent<Transform>();
		//mViewScale = mMyTransform->GetScale();
		mMyTransform->SetScale(0.f, 0.f, 0.f);
	}

	void MenuController::OnXboxFlickEvent(XboxFlickEvent* aEvent)
	{
		if (mIsDisplayed)
		{
			MenuElementDeHover deHoverEvent;

			auto currElement = mMenuElements->begin() + mCurrMenuElement;
			currElement->second->SendEvent("MenuElementDeHover", &deHoverEvent);

			if (aEvent->FlickDirection.x < 0.01f)
				mCurrMenuElement = (mCurrMenuElement <= 0) ? (mNumElements - 1) : (mCurrMenuElement - 1);

			else if (aEvent->FlickDirection.x > 0.01f)
				mCurrMenuElement = (mCurrMenuElement + 1) % mNumElements;

			auto emitter = mOwner->GetComponent<WWiseEmitter>();
			emitter->PlayEvent("UI_Menu_Hover");

			MenuElementHover hoverEvent;

			currElement = mMenuElements->begin() + mCurrMenuElement;
			currElement->second->SendEvent("MenuElementHover", &hoverEvent);
		}
	}

	void MenuController::OnXboxButtonPress(XboxButtonEvent* aEvent)
	{
		auto emitter = mOwner->GetComponent<WWiseEmitter>();

		switch (aEvent->Button)
		{
		case (Xbox_Buttons::Start):
		{
			mIsDisplayed = !mIsDisplayed;

      if (mIsDisplayed)
      {
				mMyTransform->SetScale(-16.5, 9.5f, 1.f);
        emitter->PlayEvent("UI_Menu_Pause");

				mCurrMenuElement = 0;

				MenuElementHover hoverEvent;

				auto currElement = mMenuElements->begin() + mCurrMenuElement;
				currElement->second->SendEvent("MenuElementHover", &hoverEvent);
      }
      else
      {
				mMyTransform->SetScale(0.f, 0.f, 0.f);
        emitter->PlayEvent("UI_Menu_Unpause");

				MenuElementDeHover deHoverEvent;

				auto currElement = mMenuElements->begin() + mCurrMenuElement;
				currElement->second->SendEvent("MenuElementDeHover", &deHoverEvent);
				mCurrMenuElement = 0;
      }

			break;
		}

		case (Xbox_Buttons::A):
		{
			if (mIsDisplayed)
			{
				emitter->PlayEvent("UI_Menu_Select");

				MenuElementTrigger triggerEvent;
        
				auto currElement = mMenuElements->begin() + mCurrMenuElement;
				currElement->second->SendEvent("MenuElementTrigger", &triggerEvent);
			}
      break;
		}
		}
	}

	void MenuController::OnXboxButtonRelease(XboxButtonEvent* aEvent)
	{
		if (mIsDisplayed)
		{
			switch (aEvent->Button)
			{
			case (Xbox_Buttons::A):
			{
				MenuElementHover hoverEvent;

				auto currElement = mMenuElements->begin() + mCurrMenuElement;
				currElement->second->SendEvent("MenuElementHover", &hoverEvent);

				break;
			}
			}
		}
	}
}
