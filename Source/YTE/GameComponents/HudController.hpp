/******************************************************************************/
/*!
* \author Isaac Dayton
* \date   2018/03/15
*
* \copyright All content 2018 DigiPen (USA) Corporation, all rights reserved.
*/
/******************************************************************************/

#pragma once

#include "YTE/Core/Composition.hpp"
#include "YTE/Core/ForwardDeclarations.hpp"
#include "YTE/Core/EventHandler.hpp"

#include "YTE/GameComponents/InputInterpreter.hpp"

#include "YTE/WWise/WWiseEmitter.hpp"

namespace YTE
{
  class Sprite;
  class Transform;

  class HudController : public Component
  {
  public:

    YTEDeclareType(HudController);
    HudController(Composition *aOwner, Space *aSpace, RSValue *aProperties);
    void Initialize() override;

    void OnElementToggled(HudElementToggled *aElement);
    void OnBoatRotation(BoatRotation *aRotation);

  private:

    Composition *mMap;
    Sprite *mMapSprite;

    Composition *mPostcard;
    Sprite *mPostcardSprite;
    
    Composition *mCompass;
    Sprite *mCompassSprite;

    Composition *mCompassCircle;
    Sprite *mCompassCircleSprite;

    Composition *mCompassNeedle;
    Sprite *mCompassNeedleSprite;
    Transform *mCompassNeedleTransform;
    WWiseEmitter *mSoundEmitter;

    u64 mCompassOpen;
    u64 mCompassClose;
    u64 mSailMapOpen;
    u64 mSailMapClose;
  };
}
