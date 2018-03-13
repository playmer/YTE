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

#include "YTE/GameComponents/LaunchMenu.hpp"

namespace YTE
{
  YTEDefineType(LaunchMenu)
  {
    YTERegisterType(LaunchMenu);

    YTEBindProperty(&GetMenuToLaunch, &SetMenuToLaunch, "Menu To Launch")
      .AddAttribute<Serializable>()
      .AddAttribute<EditorProperty>()
      .SetDocumentation("Menu that will be launched by triggering this menu element");
  }

  LaunchMenu::LaunchMenu(Composition* aOwner, Space* aSpace, RSValue* aProperties) : Component(aOwner, aSpace)
  {
    DeserializeByType(aProperties, this, GetStaticType());
  }

  void LaunchMenu::Initialize()
  {
    mOwner->YTERegister(Events::MenuElementTrigger, this, &LaunchMenu::OnElementTrigger);
  }

  void LaunchMenu::OnElementTrigger(MenuElementTrigger *aEvent)
  {
    YTEUnusedArgument(aEvent);

    MenuStart menuStart;
    menuStart.PlaySound = false;
    menuStart.ParentMenu = mOwner->GetParent();

    // @@@TODO: Relies on object hierarchy to work, menus that are related must share the same parent
    //          This is okay for now, as all menus must be parented to the game camera anyway, however, this
    //          will DEFINITELY cause problems with more complicated camera behavior...Change menus to multispace rendering anyway
    menuStart.ParentMenu->GetParent()->FindFirstCompositionByName(mMenuToLaunch)->SendEvent(Events::MenuStart, &menuStart);

    MenuExit menuExit(false);
    menuExit.PlaySound = false;
    menuStart.ParentMenu->SendEvent(Events::MenuExit, &menuExit);
  }
}

