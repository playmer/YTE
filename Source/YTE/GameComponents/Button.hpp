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

#include "YTE/GameComponents/MenuController.hpp"

namespace YTE
{
  class Button : public Component
  {
	public:
    YTEDeclareType(Button);
    Button(Composition *aOwner, Space *aSpace, RSValue *aProperties);
    
		void Initialize() override;

    void OnButtonHover(MenuElementHover* aEvent);
    void OnButtonTrigger(MenuElementTrigger* aEvent);

	private:
		// Store whatever button properties make sense
		// Probably an audio queue and some state sprites or something

		bool mConstructing;
  };
}
