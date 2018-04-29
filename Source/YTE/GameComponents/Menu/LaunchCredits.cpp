/******************************************************************************/
/*!
* \author Isaac Dayton
* \date   2018/04/18
*
* \copyright All content 2018 DigiPen (USA) Corporation, all rights reserved.
*/
/******************************************************************************/

#include "YTE/Core/Engine.hpp"
#include "YTE/Core/Space.hpp"

#include "YTE/Core/Actions/ActionManager.hpp"
#include "YTE/Graphics/GraphicsView.hpp"

#include "YTE/GameComponents/Menu/LaunchCredits.hpp"

namespace YTE
{
  YTEDefineEvent(StartCredits);
  YTEDefineType(StartCredits)
  {
    RegisterType<StartCredits>();
  }

  YTEDefineType(LaunchCredits)
  {
    RegisterType<LaunchCredits>();
  }

  LaunchCredits::LaunchCredits(Composition* aOwner, Space* aSpace, RSValue* aProperties)
    : Component(aOwner, aSpace)
  {
    DeserializeByType(aProperties, this, GetStaticType());
  }

  void LaunchCredits::Initialize()
  {
    auto emitter = mOwner->GetComponent<WWiseEmitter>();
    if (emitter)
    {
      emitter->PlayEvent("Menu_Start");
    }

    mOwner->YTERegister(Events::MenuElementTrigger, this, &LaunchCredits::OnElementTrigger);
  }

  void LaunchCredits::OnElementTrigger(MenuElementTrigger *)
  {
    StartCredits startCredits;

    auto creditSpace = mSpace->GetParent()->FindFirstCompositionByName("MSR_Credits");

    if (creditSpace)
    {
      creditSpace->SendEvent(Events::StartCredits, &startCredits);
    }

    auto emitter = mOwner->GetComponent<WWiseEmitter>();
    if (emitter)
    {
      emitter->PlayEvent("Menu_Stop");
    }

    /*auto actionManager = mSpace->GetComponent<ActionManager>();
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

    actionManager->AddSequence(mOwner, fadeOutSeq);*/
  }
}
