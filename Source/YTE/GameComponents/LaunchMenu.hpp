/******************************************************************************/
/*!
* \author Isaac Dayton
* \date   2018/03/05
*
* \copyright All content 2018 DigiPen (USA) Corporation, all rights reserved.
*/
/******************************************************************************/

#pragma once

#include "YTE/Core/Composition.hpp"
#include "YTE/Core/ForwardDeclarations.hpp"
#include "YTE/Core/Engine.hpp"

#include "YTE/GameComponents/MenuController.hpp"

namespace YTE
{
	class LaunchMenu : public Component
	{
	public:
		YTEDeclareType(LaunchMenu);
		LaunchMenu(Composition *aOwner, Space *aSpace, RSValue *aProperties);

		void Initialize() override;

		// PROPERTIES /////////////////////////////////////////
		std::string GetMenuToLaunch() { return mMenuToLaunch; }
		void SetMenuToLaunch(std::string& aLaunchMenu) { mMenuToLaunch = aLaunchMenu; }
		////////////////////////////////////////////////////////

		void OnElementTrigger(MenuElementTrigger *aEvent);

	private:
		std::string mMenuToLaunch;
	};
}
