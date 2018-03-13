/******************************************************************************/
/*!
* \author Isaac Dayton
* \date   2018/03/05
*
* \copyright All content 2018 DigiPen (USA) Corporation, all rights reserved.
*/
/******************************************************************************/

#include "YTE/Core/Engine.hpp"
#include "YTE/Core/Space.hpp"

#include "YTE/GameComponents/RootMenu.hpp"

namespace YTE
{
  YTEDefineType(RootMenu)
  {
    YTERegisterType(RootMenu);
  }

  RootMenu::RootMenu(Composition* aOwner, Space* aSpace, RSValue* aProperties) : Component(aOwner, aSpace)
  {
    DeserializeByType(aProperties, this, GetStaticType());
  }

  void RootMenu::Initialize()
  {
    mSpace->YTERegister(Events::MenuStart, this, &RootMenu::OnMenuStart);
  }

  void RootMenu::OnMenuStart(MenuStart *aEvent)
  {
    mOwner->SendEvent(Events::MenuStart, aEvent);
  }
}

