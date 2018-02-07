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

	YTEDefineType(MenuElementHover) { YTERegisterType(MenuElementHover); }
	YTEDefineType(MenuElementTrigger) { YTERegisterType(MenuElementTrigger); }

	YTEDefineType(MenuController)
	{
		YTERegisterType(MenuController);

		YTEBindProperty(&GetNumMenuElements, &SetNumMenuElements, "NumMenuElements")
			.AddAttribute<EditorProperty>()
			.AddAttribute<Serializable>();

		YTEBindProperty(&GetFirstMenuElementName, &SetFirstMenuElementName, "FirstMenuElement")
			.AddAttribute<EditorProperty>()
			.AddAttribute<Serializable>();

		YTEBindProperty(&GetSecondMenuElementName, &SetSecondMenuElementName, "SecondMenuElement")
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

      mMenuElements[0] = mSpace->FindFirstCompositionByName(mElementOneName);
      mMenuElements[1] = mSpace->FindFirstCompositionByName(mElementTwoName);

      auto orientation = mOwner->GetOwner()->GetComponent<Orientation>();

      glm::vec3 forward = orientation->GetForwardVector();

      auto ownerTransform = mOwner->GetOwner()->GetComponent<Transform>();

      glm::vec3 pos = ownerTransform->GetWorldTranslation();

      auto button1Tr = mMenuElements[0]->GetComponent<Transform>();
      auto button2Tr = mMenuElements[1]->GetComponent<Transform>();

      auto emitter = mOwner->GetOwner()->GetComponent<WWiseEmitter>();

      if (mIsDisplayed)
      {
        emitter->PlayEvent("UI_Menu_Pause");
        glm::vec3 spritePos = pos + 10.0f * forward;
        button1Tr->SetWorldTranslation(spritePos);
        button1Tr->SetScale(glm::vec3(10.0f, 10.0f, 10.0f));
        button1Tr->SetWorldRotation(ownerTransform->GetWorldRotation());

        spritePos.y += 5.0f;
        button2Tr->SetWorldTranslation(spritePos);
        button2Tr->SetScale(glm::vec3(10.0f, 10.0f, 10.0f));
        button2Tr->SetWorldRotation(ownerTransform->GetWorldRotation());
      }
      else
      {
        emitter->PlayEvent("UI_Menu_Unpause");
        glm::vec3 spritePos = pos + 10.0f * -forward;
        button1Tr->SetWorldTranslation(spritePos);
        button1Tr->SetScale(glm::vec3());
        button2Tr->SetWorldTranslation(spritePos);
        button2Tr->SetScale(glm::vec3());
      }

			break;
		}

		case (Xbox_Buttons::A):
		{
			if (mIsDisplayed)
			{
				MenuElementTrigger triggerEvent;

        mMenuElements[0] = mSpace->FindFirstCompositionByName(mElementOneName);
        mMenuElements[1] = mSpace->FindFirstCompositionByName(mElementTwoName);

        auto element = mMenuElements[mCurrMenuElement];
        
        if (!element) return;
        
        element->SendEvent("MenuElementTrigger", &triggerEvent);
			}
      break;
		}
		}
	}
}
