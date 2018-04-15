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

#include "YTE/Core/Actions/ActionManager.hpp"
#include "YTE/Graphics/GraphicsView.hpp"

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
    , mFadeValue(0.0f)
    , mFadeFinished(true)
  {
    DeserializeByType(aProperties, this, GetStaticType());
  }

  void LaunchGame::Initialize()
  {
    mSpace->YTERegister(Events::LogicUpdate, this, &LaunchGame::OnLogicUpdate);
    mOwner->YTERegister(Events::MenuElementTrigger, this, &LaunchGame::OnElementTrigger);
  }

  void LaunchGame::OnLogicUpdate(LogicUpdate *)
  {
    if (!mFadeFinished)
    {
      auto graphicsView = mSpace->GetComponent<GraphicsView>();
      auto clearColor = graphicsView->GetClearColor();

      graphicsView->SetClearColor(glm::vec4(clearColor.x, clearColor.y, clearColor.z, mFadeValue));
    }
  }

  void LaunchGame::OnElementTrigger(MenuElementTrigger *)
  {
    MenuExit leaveMainMenu(true);
    leaveMainMenu.PlaySound = false;
    leaveMainMenu.Handled = false;
    mOwner->GetParent()->SendEvent(Events::MenuExit, &leaveMainMenu);

    auto actionManager = mSpace->GetComponent<ActionManager>();
    ActionSequence fadeOutSeq;
    mFadeFinished = false;

    fadeOutSeq.Add<Quad::easeInOut>(mFadeValue, 1.0f, 0.5f);

    StartGame startGame;
    Space *parentSpace = static_cast<Space*>(mSpace->GetOwner());
    Space *hudSpace = static_cast<Space*>(parentSpace->FindFirstCompositionByName("MSR_HUD"));

    fadeOutSeq.Call([&startGame, parentSpace, hudSpace]() {
      parentSpace->GetComponent<InputInterpreter>()->SetInputContext(InputInterpreter::InputContext::Sailing);
      parentSpace->SendEvent(Events::StartGame, &startGame);
      hudSpace->SendEvent(Events::StartGame, &startGame);
    });

    fadeOutSeq.Add<Quad::easeInOut>(mFadeValue, 0.0f, 0.5f);

    fadeOutSeq.Call([this]() {
      mFadeFinished = true;
    });

    actionManager->AddSequence(mOwner, fadeOutSeq);
  }
}
