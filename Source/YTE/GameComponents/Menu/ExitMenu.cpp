/******************************************************************************/
/*!
* \author Isaac Dayton
* \date   2018/04/11
*
* \copyright All content 2018 DigiPen (USA) Corporation, all rights reserved.
*/
/******************************************************************************/

#include "YTE/Core/Engine.hpp"
#include "YTE/Core/Space.hpp"

#include "YTE/GameComponents/Menu/ExitMenu.hpp"

namespace YTE
{
  YTEDefineType(ExitMenu)
  {
    YTERegisterType(ExitMenu);

    YTEBindProperty(&GetShouldCloseAll, &SetShouldCloseAll, "ShouldCloseAll")
      .AddAttribute<Serializable>()
      .AddAttribute<EditorProperty>()
      .SetDocumentation("Whether or not this element should close all menus or only this one");
  }

  ExitMenu::ExitMenu(Composition* aOwner, Space* aSpace, RSValue* aProperties) 
    : Component(aOwner, aSpace)
    , mShouldCloseAll(true)
  {
    DeserializeByType(aProperties, this, GetStaticType());
  }

  void ExitMenu::Initialize()
  {
    mOwner->YTERegister(Events::MenuElementTrigger, this, &ExitMenu::OnElementTrigger);
  }

  void ExitMenu::OnElementTrigger(MenuElementTrigger *)
  {
    MenuExit menuExit(mShouldCloseAll);
    menuExit.PlaySound = true;
    mOwner->GetSpace()->SendEvent(Events::MenuExit, &menuExit);
  }
}
