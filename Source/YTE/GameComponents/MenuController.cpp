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
	YTEDefineType(MenuController)
	{
		YTERegisterType(MenuController);

		YTEBindProperty(&MenuController::GetNumMenuElements, &MenuController::SetNumMenuElements, "Number of Menu Elements")
			.AddAttribute<EditorProperty>()
			.AddAttribute<Serializable>();
	}

	MenuController::MenuController(Composition* aOwner, Space* aSpace, RSValue* aProperties) : Component(aOwner, aSpace), mConstructing(true)
	{
		DeserializeByType(aProperties, this, GetStaticType());

		mConstructing = false;
	}

	void MenuController::Initialize()
	{

	}
}
