/******************************************************************************/
/*!
* \author Nicholas Ammann
* \date   2018/03/25
*
* \copyright All content 2018 DigiPen (USA) Corporation, all rights reserved.
*/
/******************************************************************************/

#include "YTE/GameComponents/InputInterpreter.hpp"
#include "YTE/GameComponents/HudController.hpp"
#include "YTE/GameComponents/Menu/LaunchGame.hpp"
#include "YTE/GameComponents/PostcardIconPulse.hpp"
#include "YTE/GameComponents/DialogueDirector.hpp"
#include "YTE/Graphics/Sprite.hpp"
#include "YTE/Physics/Transform.hpp"
#include "YTE/WWise/WWiseSystem.hpp"

#include "YTE/Core/Actions/Action.hpp"
#include "YTE/Core/Actions/ActionManager.hpp"

namespace YTE
{
  YTEDefineEvent(PostcardUpdate);

  YTEDefineType(PostcardUpdate)
  {
    RegisterType<PostcardUpdate>();
    TypeBuilder<PostcardUpdate> builder;
    builder.Field<&PostcardUpdate::Number>( "Number", PropertyBinding::GetSet);
  }

  YTEDefineEvent(HideHudEvent);

  YTEDefineType(HideHudEvent)
  {
    RegisterType<HideHudEvent>();
    TypeBuilder<HideHudEvent> builder;
  }

  YTEDefineEvent(DialoguePossible);

  YTEDefineType(DialoguePossible)
  {
    RegisterType<DialoguePossible>();
    TypeBuilder<DialoguePossible> builder;
    builder.Field<&DialoguePossible::isPossible>( "isPossible", PropertyBinding::GetSet);
  }

  YTEDefineType(HudController)
  {
    RegisterType<HudController>();
    TypeBuilder<HudController> builder;
  }

  HudController::HudController(Composition *aOwner, Space *aSpace, RSValue *aProperties)
    : Component(aOwner, aSpace)
    , mMap(nullptr)
    , mMapSprite(nullptr)
    , mPostcard(nullptr)
    , mPostcardSprite(nullptr)
    , mCompass(nullptr)
    , mCompassSprite(nullptr)
    , mPostcardTextures{ "" }
    , mOpenPostcardTimer(0.0)
    , mRightTrigger(nullptr)
    , mRightTriggerSprite(nullptr)
    , mLeftTrigger(nullptr)
    , mLeftTriggerSprite(nullptr)
    , mAButton(nullptr)
    , mAButtonSprite(nullptr)
    , mRightTriggerTutorialOver(false)
    , mLeftTriggerTutorialOver(false)
  {
    DeserializeByType(aProperties, this, GetStaticType());

    mPostcardTextures[0] = "Postcard_Tutorial.jpg";
    mPostcardTextures[1] = "Postcard_1.jpg";
    mPostcardTextures[2] = "Postcard_2.jpg";
    mPostcardTextures[3] = "Postcard_3.jpg";
    mPostcardTextures[4] = "Postcard_4.jpg";
    mPostcardTextures[5] = "Postcard_5.jpg";
    mPostcardTextures[6] = "Postcard_6.jpg";
    mPostcardTextures[7] = "Postcard_7.jpg";
    mPostcardTextures[8] = "Postcard_8.jpg";
    mPostcardTextures[9] = "Postcard_9.jpg";
  }

  void HudController::Initialize()
  {
    // get map object and its sprite
    if (mMap = mSpace->FindFirstCompositionByName("Map"); mMap)
    {
      mMapSprite = mMap->GetComponent<Sprite>();
      mMapSprite->SetVisibility(false);
    }

    // get postcard object and its sprite
    if (mPostcard = mSpace->FindFirstCompositionByName("Postcard"); mPostcard)
    {
      mPostcardSprite = mPostcard->GetComponent<Sprite>();
      mPostcardSprite->SetVisibility(false);
    }

    // get compass object and its sprite
    if (mCompass = mSpace->FindFirstCompositionByName("Compass");  mCompass)
    {
      mCompassSprite = mCompass->GetComponent<Sprite>();
      mCompassSprite->SetVisibility(false);
    }

    // get compass object and its sprite
    if (mCompassCircle = mSpace->FindFirstCompositionByName("CompassCircle"); mCompassCircle)
    {
      mCompassCircleSprite = mCompassCircle->GetComponent<Sprite>();
      mCompassCircleSprite->SetVisibility(false);
    }

    // get compass needle and its sprite
    if (mCompassNeedle = mSpace->FindFirstCompositionByName("CompassNeedle"); mCompassNeedle)
    {
      mCompassNeedleSprite = mCompassNeedle->GetComponent<Sprite>();
      mCompassNeedleSprite->SetVisibility(false);

      mCompassNeedleTransform = mCompassNeedle->GetComponent<Transform>();
    }


    // dpad and icons

    if (mDPad = mSpace->FindFirstCompositionByName("DPad"); mDPad)
    {
      if (mDPadSprite = mDPad->GetComponent<Sprite>(); mDPadSprite)
      {
        mDPadSprite->SetVisibility(false);
      }
    }

    if (mMapIcon = mSpace->FindFirstCompositionByName("Icon_Map"); mMapIcon)
    {
      if (mMapIconSprite = mMapIcon->GetComponent<Sprite>(); mMapIconSprite)
      {
        mMapIconSprite->SetVisibility(false);
      }
    }

    if (mPostcardIcon = mSpace->FindFirstCompositionByName("Icon_Postcard"); mPostcardIcon)
    {
      if (mPostcardIconSprite = mPostcardIcon->GetComponent<Sprite>(); mPostcardIconSprite)
      {
        mPostcardIconSprite->SetVisibility(false);
      }
    }

    if (mCompassIcon = mSpace->FindFirstCompositionByName("Icon_Compass"); mCompassIcon)
    {
      if (mCompassIconSprite = mCompassIcon->GetComponent<Sprite>(); mCompassIconSprite)
      {
        mCompassIconSprite->SetVisibility(false);
      }
    }

    // control prompts
    if (mRightTrigger = mSpace->FindFirstCompositionByName("RightTrigger"); mRightTrigger)
    {
      mRightTriggerSprite = mRightTrigger->GetComponent<Sprite>();
      mRightTriggerSprite->SetVisibility(false);
    }

    if (mLeftTrigger = mSpace->FindFirstCompositionByName("LeftTrigger"); mLeftTrigger)
    {
      mLeftTriggerSprite = mLeftTrigger->GetComponent<Sprite>();
      mLeftTriggerSprite->SetVisibility(false);
    }

    if (mAButton = mSpace->FindFirstCompositionByName("AButton"); mAButton)
    {
      mAButtonSprite = mAButton->GetComponent<Sprite>();
      mAButtonSprite->SetVisibility(false);
    }

    auto soundSystem = mSpace->GetEngine()->GetComponent<WWiseSystem>();

    if (soundSystem)
    {
      mCompassClose = soundSystem->GetSoundIDFromString("UI_SailCompass_Close");
      mCompassOpen =  soundSystem->GetSoundIDFromString("UI_SailCompass_Open");
      mSailMapClose = soundSystem->GetSoundIDFromString("UI_SailMap_Close");
      mSailMapOpen =  soundSystem->GetSoundIDFromString("UI_SailMap_Open");
    }
    mSoundEmitter = mOwner->GetComponent<WWiseEmitter>();

    // register for user toggling hud elements
    mSpace->RegisterEvent<&HudController::OnElementToggled>(Events::HudElementToggled, this);

    mSpace->RegisterEvent<&HudController::OnMenuStart>(Events::MenuStart, this);
    mSpace->RegisterEvent<&HudController::OnMenuExit>(Events::MenuExit, this);

    mSpace->GetOwner()->RegisterEvent<&HudController::OnBoatRotation>(Events::BoatRotation, this);
    mSpace->GetOwner()->RegisterEvent<&HudController::OnPostcardUpdate>(Events::PostcardUpdate, this);
    mSpace->GetOwner()->RegisterEvent<&HudController::OnStartGame>(Events::StartGame, this);
    mSpace->GetOwner()->RegisterEvent<&HudController::OnSailChanged>(Events::SailStateChanged, this);
    mSpace->GetOwner()->RegisterEvent<&HudController::OnDialoguePossible>(Events::DialoguePossible, this);
    mSpace->GetOwner()->RegisterEvent<&HudController::OnHideHud>(Events::HideHudEvent, this);
    mSpace->GetOwner()->RegisterEvent<&HudController::OnDialogueStart>(Events::DialogueStart, this);
    mSpace->GetOwner()->RegisterEvent<&HudController::OnDialogueExit>(Events::DialogueExit, this);

    // force the game to load all textures so there's no hiccup when we switch in future
    for (int i = 9; i >= 0; i--)
    {
      std::string textureName = mPostcardTextures[i];
      mPostcardSprite->SetTexture(textureName);
    }
  }

  void HudController::OnElementToggled(HudElementToggled *aElement)
  {
    switch (aElement->mElement)
    {
      case HudElementToggled::Element::Map:
      {
        if (mMapSprite)
        {
          bool mapVisible = mMapSprite->GetVisibility();
          mMapSprite->SetVisibility(!mapVisible);
          if (mSoundEmitter)
          {
            (mapVisible)
              ? mSoundEmitter->PlayEvent(mSailMapClose)
              : mSoundEmitter->PlayEvent(mSailMapOpen);
          }
        }
        break;
      }

      case HudElementToggled::Element::Postcard:
      {
        if (mPostcardSprite)
        {
          bool postcardVisible = mPostcardSprite->GetVisibility();
          mPostcardSprite->SetVisibility(!postcardVisible);
          if (mSoundEmitter)
          {
            (postcardVisible)
              ? mSoundEmitter->PlayEvent("UI_SailPostcard_Close")
              : mSoundEmitter->PlayEvent("UI_SailPostcard_Open");
          }

          if (!postcardVisible)
          {
            if (auto iconPulse = mPostcardIcon->GetComponent<PostcardIconPulse>(); iconPulse)
            {
              iconPulse->SetPulsing(true);
            }

            mMapIconSprite->SetVisibility(false);
            mCompassIconSprite->SetVisibility(false);
          }
          else
          {
            if (auto iconPulse = mPostcardIcon->GetComponent<PostcardIconPulse>(); iconPulse)
            {
              iconPulse->SetPulsing(false);
            }

            mMapIconSprite->SetVisibility(true);
            mCompassIconSprite->SetVisibility(true);
          }
        }
        break;
      }

      case HudElementToggled::Element::Compass:
      {
        if (mCompassSprite)
        {
          bool compassVisible = mCompassSprite->GetVisibility();
          mCompassSprite->SetVisibility(!compassVisible);

          if (mSoundEmitter)
          {
            (compassVisible)
              ? mSoundEmitter->PlayEvent(mCompassClose)
              : mSoundEmitter->PlayEvent(mCompassOpen);
          }
        }

        if (mCompassNeedleSprite)
        {
          bool compassNeedleVisible = mCompassNeedleSprite->GetVisibility();
          mCompassNeedleSprite->SetVisibility(!compassNeedleVisible);
        }

        if (mCompassCircleSprite)
        {
          bool compassCircleVisible = mCompassCircleSprite->GetVisibility();
          mCompassCircleSprite->SetVisibility(!compassCircleVisible);
        }
        break;
      }
    }
  }

  void HudController::OnBoatRotation(BoatRotation *aRotation)
  {
    glm::vec3 forward = aRotation->BoatForward;

    forward = glm::normalize(forward);

    glm::vec3 xAxis(1, 0, 0);

    float rotY = glm::degrees(acos(glm::dot(xAxis, forward)));

    if (forward.z < 0)
    {
      rotY = 180 + (180 - rotY);
    }

    glm::vec3 needleRot(0.0f, 0.0f, (rotY - 90));

    if (mCompassNeedleTransform)
    {
      mCompassNeedleTransform->SetWorldRotation(needleRot);
    }
  }

  void HudController::OnMenuStart(MenuStart *aEvent)
  {
    UnusedArguments(aEvent);
    HideHud();
  }

  void HudController::OnMenuExit(MenuExit *aEvent)
  {
    UnusedArguments(aEvent);
    ShowHud();
  }

  void HudController::OnPostcardUpdate(PostcardUpdate *aEvent)
  {
    if (aEvent->Number > 9)
    {
      return;
    }

    std::string textureName = mPostcardTextures[aEvent->Number];

    mPostcardSprite->SetTexture(textureName);

    OpenPostcard();
  }

  void HudController::OnStartGame(StartGame *)
  {
    ShowHud();
    
    // show right trigger to prompt user to move
    if (mRightTriggerSprite)
    {
      mRightTriggerSprite->SetVisibility(true);
    }
  }

  void HudController::OnSailChanged(SailStateChanged *aEvent)
  {
    if (aEvent->SailUp && !mRightTriggerTutorialOver)
    {
      mRightTriggerTutorialOver = true;
      mRightTriggerSprite->SetVisibility(false);
      mLeftTriggerSprite->SetVisibility(true);
    }
    else if (!aEvent->SailUp && !mLeftTriggerTutorialOver)
    {
      mLeftTriggerSprite->SetVisibility(false);
      mLeftTriggerTutorialOver = true;
    }
  }

  void HudController::OnDialoguePossible(DialoguePossible *aEvent)
  {
    if (mAButtonSprite)
    {
      if (aEvent->isPossible)
      {
        mAButtonSprite->SetVisibility(true);
      }
      else
      {
        mAButtonSprite->SetVisibility(false);
      }
    }
  }

  void HudController::OnDialogueStart(DialogueStart *)
  {
    HideHud();
  }

  void HudController::OnDialogueExit(DialogueExit *)
  {
    ShowHud();
  }

  void HudController::OnHideHud(HideHudEvent *)
  {
    HideHud();
  }

  void HudController::OpenPostcard()
  {
    if (mPostcardSprite)
    {
      auto actionManager = mSpace->GetComponent<ActionManager>();

      ActionSequence openPostcard;
      openPostcard.Add<Linear::easeNone>(mOpenPostcardTimer, 1.0f, 1.0f);
      openPostcard.Call([this]() {
        mPostcardSprite->SetVisibility(true);
        mSoundEmitter->PlayEvent("UI_SailPostcard_Open");
        
        SailStateChanged setSailUp(false);
        mSpace->GetOwner()->SendEvent(Events::SailStateChanged, &setSailUp);
      });
      actionManager->AddSequence(mOwner, openPostcard);

      mSoundEmitter->PlayEvent("UI_SailPostcard_Open");

      if (auto iconPulse = mPostcardIcon->GetComponent<PostcardIconPulse>(); iconPulse)
      {
        iconPulse->SetPulsing(true);
      }

      mMapIconSprite->SetVisibility(false);
      mCompassIconSprite->SetVisibility(false);
    }
  }
  void HudController::ShowHud()
  {
    if (mDPadSprite)
    {
      mDPadSprite->SetVisibility(true);
    }

    if (mMapIconSprite)
    {
      mMapIconSprite->SetVisibility(true);
    }

    if (mPostcardIconSprite)
    {
      mPostcardIconSprite->SetVisibility(true);
    }

    if (mCompassIconSprite)
    {
      mCompassIconSprite->SetVisibility(true);
    }
  }

  void HudController::HideHud()
  {
    mMapSprite->SetVisibility(false);
    mPostcardSprite->SetVisibility(false);
    mCompassSprite->SetVisibility(false);
    mCompassCircleSprite->SetVisibility(false);
    mCompassNeedleSprite->SetVisibility(false);
    mMapSprite->SetVisibility(false);
    mLeftTriggerSprite->SetVisibility(false);
    mRightTriggerSprite->SetVisibility(false);
    mAButtonSprite->SetVisibility(false);

    if (mDPadSprite)
    {
      mDPadSprite->SetVisibility(false);
    }

    if (mMapIconSprite)
    {
      mMapIconSprite->SetVisibility(false);
    }

    if (mPostcardIconSprite)
    {
      mPostcardIconSprite->SetVisibility(false);
    }

    if (mCompassIconSprite)
    {
      mCompassIconSprite->SetVisibility(false);
    }
  }
}
