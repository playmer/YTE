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

#include "YTE/GameComponents/Menu/RootMenu.hpp"

namespace YTE
{
  YTEDefineType(RootMenu)
  {
    RegisterType<RootMenu>();
    TypeBuilder<RootMenu> builder;
  }

  RootMenu::RootMenu(Composition* aOwner, Space* aSpace, RSValue* aProperties) : Component(aOwner, aSpace)
  {
    DeserializeByType(aProperties, this, GetStaticType());
  }

  void RootMenu::Initialize()
  {
    mSpace->RegisterEvent<&RootMenu::OnMenuStart>(Events::MenuStart, this);
  }

  void RootMenu::OnMenuStart(MenuStart *aEvent)
  {
    aEvent->ResetCursor = true;
    mOwner->SendEvent(Events::MenuStart, aEvent);
  }
}

