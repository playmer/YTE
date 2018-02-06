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

#include "YTE/Platform/Gamepad.hpp"
#include "YTE/Platform/GamepadSystem.hpp"
#include "YTE/Platform/DeviceEnums.hpp"

#include "fmt/format.h"

namespace YTE
{
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

  class MenuController : public Component
  {
	public:
    YTEDeclareType(MenuController);
    MenuController(Composition *aOwner, Space *aSpace, RSValue *aProperties);
    
		void Initialize() override;

		// PROPERTIES /////////////////////////////////////////
		int GetNumMenuElements() { return mNumMenuElements; }

		void SetNumMenuElements(int& aNumElements)
		{
			mNumMenuElements = aNumElements;
		}

		std::string GetFirstMenuElementName() 
		{ 
      return mElementOneName;
		}

		std::string GetSecondMenuElementName() 
		{ 
      return mElementTwoName;
		}

		void SetFirstMenuElementName(std::string& aElementName)
		{
      mElementOneName = aElementName;
			mMenuElements[0] = mSpace->FindFirstCompositionByName(aElementName);
		}

		void SetSecondMenuElementName(std::string& aElementName)
		{
      mElementTwoName = aElementName;
			mMenuElements[1] = mSpace->FindFirstCompositionByName(aElementName);
		}

		////////////////////////////////////////////////////////

    void OnXboxStickEvent(XboxStickEvent* aEvent);
    void OnXboxButtonPress(XboxButtonEvent* aEvent);

	private:
		XboxController* mGamePad;

		int mNumMenuElements;
		Composition* mMenuElements[2];
		int mCurrMenuElement;

    std::string mElementOneName;
    std::string mElementTwoName;

		bool mIsDisplayed;

		bool mConstructing;
  };
} // End YTE namespace
