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
#include "YTE/Graphics/Sprite.hpp"

#include "YTE/GameComponents/SplashScreen.hpp"

#pragma optimize("",off)

namespace YTE
{
  YTEDefineType(SplashScreen)
  {
    YTERegisterType(SplashScreen);
  }

  SplashScreen::SplashScreen(Composition* aOwner, Space* aSpace, RSValue* aProperties)
    : Component(aOwner, aSpace)
    , mFadeValue(1.0f)
    , mReadyToExecute(false)
    , mDigiPenLogo(nullptr)
    , mTeamLogo(nullptr)
    , mControllerWarning(nullptr)
    , mBlackoutLevel(nullptr)
    , mView(nullptr)
  {
    DeserializeByType(aProperties, this, GetStaticType());
  }

  void SplashScreen::Initialize()
  {
    mView = mSpace->GetComponent<GraphicsView>();
    mBlackoutLevel = mSpace->AddChildSpace("MSR_Blackout");

    if (Composition *dp = mSpace->FindFirstCompositionByName("DigipenSplash"))
    {
      mDigiPenLogo = dp->GetComponent<Sprite>();
    }
    if (Composition *team = mSpace->FindFirstCompositionByName("TeamSplash"))
    {
      mTeamLogo = team->GetComponent<Sprite>();
    }
    if (Composition *controller = mSpace->FindFirstCompositionByName("ControllerSplash"))
    {
      mControllerWarning = controller->GetComponent<Sprite>();
    }

    mSpace->YTERegister(Events::FrameUpdate, this, &SplashScreen::OnFrameUpdate);
    mSpace->YTERegister(Events::LogicUpdate, this, &SplashScreen::OnLogicUpdate);
  }

  void SplashScreen::Start()
  {
    if (mDigiPenLogo)
    {
      mDigiPenLogo->SetVisibility(true);
    }
    if (mTeamLogo)
    {
      mTeamLogo->SetVisibility(false);
    }
    if (mControllerWarning)
    {
      mControllerWarning->SetVisibility(false);
    }

    auto manager = mSpace->GetComponent<ActionManager>();
    ActionSequence splashSeq;

    splashSeq.Add<Quad::easeInOut>(mFadeValue, 0.0f, 0.5f);
    splashSeq.Delay(2.0f);
    splashSeq.Add<Quad::easeInOut>(mFadeValue, 1.0f, 0.5f);
    
    splashSeq.Call([this]() {
      mDigiPenLogo->SetVisibility(false);
      mTeamLogo->SetVisibility(true);
    });

    splashSeq.Add<Quad::easeInOut>(mFadeValue, 0.0f, 0.5f);
    splashSeq.Delay(2.0f);
    splashSeq.Add<Quad::easeInOut>(mFadeValue, 1.0f, 0.5f);

    splashSeq.Call([this]() {
      mTeamLogo->SetVisibility(false);
      mControllerWarning->SetVisibility(true);
    });

    splashSeq.Add<Quad::easeInOut>(mFadeValue, 0.0f, 0.5f);
    splashSeq.Delay(2.0f);
    splashSeq.Add<Quad::easeInOut>(mFadeValue, 1.0f, 0.5f);

    splashSeq.Call([this]() {
      if (!mSpace->GetEngine()->IsEditor())
      {
        String level{ "presentationLevel" };
        mSpace->LoadLevel(level);
      }
    });

    manager->AddSequence(mOwner, splashSeq);

    mReadyToExecute = true;
  }

  void SplashScreen::OnFrameUpdate(LogicUpdate *)
  {
    auto window = mView->GetWindow();
    if (window)
    {
      if (window->IsFocused())
      {
        window->SetCursorVisibility(false);
      }
      else
      {
        window->SetCursorVisibility(true);
      }
    }
  }

  void SplashScreen::OnLogicUpdate(LogicUpdate *)
  {
    if (mReadyToExecute && mBlackoutLevel)
    {
      auto view = mBlackoutLevel->GetComponent<GraphicsView>();
      auto clearColor = view->GetClearColor();

      view->SetClearColor({ clearColor.r, clearColor.g, clearColor.b, mFadeValue });
    }
  }
}
