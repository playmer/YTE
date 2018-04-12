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

#include "YTE/GameComponents/Menu/LaunchGame.hpp"

namespace YTE
{
  YTEDefineEvent(StartGame);
  YTEDefineType(StartGame) { YTERegisterType(StartGame); }

  YTEDefineType(LaunchGame)
  {
    YTERegisterType(LaunchGame);
  }

  LaunchGame::LaunchGame(Composition* aOwner, Space* aSpace, RSValue* aProperties)
    : Component(aOwner, aSpace)
  {
    DeserializeByType(aProperties, this, GetStaticType());
  }

  void LaunchGame::Initialize()
  {
    mOwner->YTERegister(Events::MenuElementTrigger, this, &LaunchGame::OnElementTrigger);
  }

  void LaunchGame::OnElementTrigger(MenuElementTrigger *)
  {
    MenuExit leaveMainMenu(true);
    leaveMainMenu.PlaySound = false;
    leaveMainMenu.Handled = false;
    mOwner->GetParent()->SendEvent(Events::MenuExit, &leaveMainMenu);

    StartGame startGame;
    Space *parentSpace = static_cast<Space*>(mSpace->GetOwner());

    parentSpace->GetComponent<InputInterpreter>()->SetInputContext(InputInterpreter::InputContext::Sailing);
    parentSpace->SendEvent(Events::StartGame, &startGame);
  }
}
