/******************************************************************************/
/*!
* \author Nicholas Ammann
* \date   2018/03/25
*
* \copyright All content 2018 DigiPen (USA) Corporation, all rights reserved.
*/
/******************************************************************************/

#include "YTE/GameComponents/HudController.hpp"
#include "YTE/Graphics/Sprite.hpp"
#include "YTE/Physics/Transform.hpp"

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

    // get compass needle and its sprite
    mCompassNeedle = mSpace->FindFirstCompositionByName("CompassNeedle");

    if (mCompassNeedle)
    {
      mCompassNeedleSprite = mCompassNeedle->GetComponent<Sprite>();
      mCompassNeedleSprite->SetVisibility(false);

      mCompassNeedleTransform = mCompassNeedle->GetComponent<Transform>();
    }

    // register for user toggling hud elements
    mSpace->YTERegister(Events::HudElementToggled, this, &HudController::OnElementToggled);

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
        }
        break;
      }

      case HudElementToggled::Element::Postcard:
      {
        if (mPostcardSprite)
        {
          bool postcardVisible = mPostcardSprite->GetVisibility();
          mPostcardSprite->SetVisibility(!postcardVisible);
        }
        break;
      }

      case HudElementToggled::Element::Compass:
      {
        if (mCompassSprite)
        {
          bool compassVisible = mCompassSprite->GetVisibility();
          mCompassSprite->SetVisibility(!compassVisible);
        }

        if (mCompassNeedleSprite)
        {
          bool compassNeedleVisible = mCompassNeedleSprite->GetVisibility();
          mCompassNeedleSprite->SetVisibility(!compassNeedleVisible);
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

    glm::vec3 needleRot(0.0f, 0.0f, rotY);

    if (mCompassNeedleTransform)
    {
      mCompassNeedleTransform->SetWorldRotation(needleRot);
    }
  }
}
