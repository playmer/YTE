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
#include "YTE/Graphics/Sprite.hpp"
#include "YTE/Physics/Transform.hpp"
#include "YTE/WWise/WWiseSystem.hpp"

namespace YTE
{

  YTEDefineType(HudController)
  {
    YTERegisterType(HudController);
  }


  HudController::HudController(Composition *aOwner, Space *aSpace, RSValue *aProperties)
    : Component(aOwner, aSpace)
    , mMap(nullptr)
    , mMapSprite(nullptr)
    , mPostcard(nullptr)
    , mPostcardSprite(nullptr)
    , mCompass(nullptr)
    , mCompassSprite(nullptr)
  {
    DeserializeByType(aProperties, this, GetStaticType());
  }

  void HudController::Initialize()
  {
    // get map object and its sprite
    mMap = mSpace->FindFirstCompositionByName("Map");

    if (mMap)
    {
      mMapSprite = mMap->GetComponent<Sprite>();
      mMapSprite->SetVisibility(false);
    }

    // get postcard object and its sprite
    mPostcard = mSpace->FindFirstCompositionByName("Postcard");

    if (mPostcard)
    {
      mPostcardSprite = mPostcard->GetComponent<Sprite>();
      mPostcardSprite->SetVisibility(false);
    }

    // get compass object and its sprite
    mCompass = mSpace->FindFirstCompositionByName("Compass");

    if (mCompass)
    {
      mCompassSprite = mCompass->GetComponent<Sprite>();
      mCompassSprite->SetVisibility(false);
    }

    // get compass object and its sprite
    mCompassCircle = mSpace->FindFirstCompositionByName("CompassCircle");

    if (mCompassCircle)
    {
      mCompassCircleSprite = mCompassCircle->GetComponent<Sprite>();
      mCompassCircleSprite->SetVisibility(false);
    }

    // get compass needle and its sprite
    mCompassNeedle = mSpace->FindFirstCompositionByName("CompassNeedle");

    if (mCompassNeedle)
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
    mSpace->YTERegister(Events::HudElementToggled, this, &HudController::OnElementToggled);

    mSpace->YTERegister(Events::MenuStart, this, &HudController::OnMenuStart);
    mSpace->YTERegister(Events::MenuExit, this, &HudController::OnMenuExit);

    mSpace->GetOwner()->YTERegister(Events::BoatRotation, this, &HudController::OnBoatRotation);
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
    YTEUnusedArgument(aEvent);

    mMapSprite->SetVisibility(false);
    mPostcardSprite->SetVisibility(false);
    mCompassSprite->SetVisibility(false);
    mCompassCircleSprite->SetVisibility(false);
    mCompassNeedleSprite->SetVisibility(false);
    mMapSprite->SetVisibility(false);

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
  void HudController::OnMenuExit(MenuExit *aEvent)
  {
    YTEUnusedArgument(aEvent);

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
}
