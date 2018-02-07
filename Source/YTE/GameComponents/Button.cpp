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

#include "YTE/GameComponents/Button.hpp"

namespace YTE
{
	YTEDefineType(Button)
	{
		YTERegisterType(Button);
	}

	Button::Button(Composition* aOwner, Space* aSpace, RSValue* aProperties) : Component(aOwner, aSpace), mConstructing(true)
	{
		DeserializeByType(aProperties, this, GetStaticType());

		mConstructing = false;
	}

	void Button::Initialize()
	{
		mOwner->YTERegister("MenuElementHover", this, &Button::OnButtonHover);
		mOwner->YTERegister("MenuElementTrigger", this, &Button::OnButtonTrigger);
	}

	void Button::OnButtonHover(MenuElementHover* aEvent)
	{
		std::cout << "Hovering over button: " << mOwner->GetName() << std::endl;
	}

	void Button::OnButtonTrigger(MenuElementTrigger* aEvent)
	{
		std::cout << "Activating button: " << mOwner->GetName() << std::endl;
	}
}
