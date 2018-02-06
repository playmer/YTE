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

namespace YTE
{
  class MenuController : public Component
  {
	public:
    YTEDeclareType(MenuController);
    MenuController(Composition *aOwner, Space *aSpace, RSValue *aProperties);
    
		void Initialize() override;

		// PROPERTIES /////////////////////////////////////////
		u8 GetNumMenuElements()
		{
			return mNumMenuElements;
		}

		void SetNumMenuElements(u8& aNumElements)
		{
			mNumMenuElements = aNumElements;
		}
		////////////////////////////////////////////////////////

    //void CheckSticks(XboxStickEvent *aEvent);
    //void CheckButtons(XboxButtonEvent *aEvent);

	private:
		XboxController* mGamePad;

		u8 mNumMenuElements;

		bool mConstructing;
  };
} // End YTE namespace
