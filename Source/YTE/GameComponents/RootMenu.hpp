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

#include "YTE/GameComponents/InputInterpreter.hpp"

namespace YTE
{
	class RootMenu : public Component
	{
	public:
		YTEDeclareType(RootMenu);
		RootMenu(Composition *aOwner, Space *aSpace, RSValue *aProperties);

		void Initialize() override;

		void OnMenuStart(MenuStart *aEvent);
	};
}
