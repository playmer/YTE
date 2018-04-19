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
  class StartGame;
  
  class DialogueStart;
  class DialogueExit;

  YTEDeclareEvent(PostcardUpdate);
  class PostcardUpdate : public Event
  {
  public:
    YTEDeclareType(PostcardUpdate);

    int Number;
  };

  YTEDeclareEvent(HideHudEvent);
  class HideHudEvent : public Event
  {
  public:
    YTEDeclareType(HideHudEvent);
  };

  YTEDeclareEvent(DialoguePossible);
  class DialoguePossible : public Event
  {
  public:
    YTEDeclareType(DialoguePossible);

    bool isPossible;
  };

  class HudController : public Component
  {
  public:

    YTEDeclareType(HudController);
    HudController(Composition *aOwner, Space *aSpace, RSValue *aProperties);
    void Initialize() override;

    void OnElementToggled(HudElementToggled *aElement);
    void OnBoatRotation(BoatRotation *aRotation);

    void OnMenuStart(MenuStart *aEvent);
    void OnMenuExit(MenuExit *aEvent);

    void OnHideHud(HideHudEvent *aEvent);
    void OnPostcardUpdate(PostcardUpdate *aEvent);
    void OnStartGame(StartGame *aEvent);
    void OnSailChanged(SailStateChanged *aEvent);
    void OnDialoguePossible(DialoguePossible *aEvent);
    void OnDialogueStart(DialogueStart *aEvent);
    void OnDialogueExit(DialogueExit *aEvent);

    void OpenPostcard();

    void ShowHud();
    void HideHud();

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

    // dpad and icons
    Composition *mDPad;
    Sprite *mDPadSprite;

    Composition *mMapIcon;
    Sprite *mMapIconSprite;

    Composition *mPostcardIcon;
    Sprite *mPostcardIconSprite;

    Composition *mCompassIcon;
    Sprite *mCompassIconSprite;

    Composition *mRightTrigger;
    Sprite *mRightTriggerSprite;

    Composition *mLeftTrigger;
    Sprite *mLeftTriggerSprite;

    Composition *mAButton;
    Sprite *mAButtonSprite;

    std::string mPostcardTextures[10];

    u64 mCompassOpen;
    u64 mCompassClose;
    u64 mSailMapOpen;
    u64 mSailMapClose;

    float mOpenPostcardTimer;

    bool mRightTriggerTutorialOver;
    bool mLeftTriggerTutorialOver;
  };
}
