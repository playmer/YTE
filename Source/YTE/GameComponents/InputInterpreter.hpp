/******************************************************************************/
/*!
\file   InputInterpreter.hpp
\author Isaac Dayton
        Jonathan Ackerman
\par    email: jonathan.ackerman\@digipen.edu
\date   2018-01-19
\brief
    An interface on the engine for gameplay logic components. This component
    will listen to input events and translate them to a format gameplay components
    can use and then the interface will dispatch that info in gameplay events
    (controllers that care about input listen to these events). The benefit
    of this interface is a control scheme agnostic controller.

All content (c) 2016 DigiPen  (USA) Corporation, all rights reserved.
*/
/******************************************************************************/

#pragma once

#include "YTE/Core/Composition.hpp"
#include "YTE/Core/ForwardDeclarations.hpp"
#include "YTE/Core/Engine.hpp"

#include "YTE/Platform/Keyboard.hpp"
#include "YTE/Platform/Gamepad.hpp"
#include "YTE/Platform/GamepadSystem.hpp"
#include "YTE/Platform/DeviceEnums.hpp"

namespace YTE
{
  class InputInterpreter;

  /////////////////////////////////////////////////////////////////////////////////////
  // Events
  /////////////////////////////////////////////////////////////////////////////////////
  YTEDeclareEvent(SailStateChanged);
  YTEDeclareEvent(BoatTurnEvent);
  YTEDeclareEvent(BoatDockEvent);
  YTEDeclareEvent(BoatRotation);
  YTEDeclareEvent(RequestDialogueStart);
  YTEDeclareEvent(DialogueSelect);
  YTEDeclareEvent(DialogueConfirm);
  YTEDeclareEvent(DialogueExit);
  YTEDeclareEvent(CameraRotateEvent);
  YTEDeclareEvent(DirectCameraEvent);
  YTEDeclareEvent(MenuStart);
  YTEDeclareEvent(MenuConfirm);
  YTEDeclareEvent(MenuExit);
  YTEDeclareEvent(MenuElementChange);
  YTEDeclareEvent(DebugSwitch);
  YTEDeclareEvent(RequestNoticeBoardStart);
  YTEDeclareEvent(HudElementToggled);

  class CameraRotateEvent : public Event
  {
  public:
    YTEDeclareType(CameraRotateEvent);
    glm::vec2 StickDirection;
  };

  class DirectCameraEvent : public Event
  {
  public:
    YTEDeclareType(DirectCameraEvent);
  };

  class SailStateChanged : public Event
  {
  public:
    YTEDeclareType(SailStateChanged);
    SailStateChanged(bool state) { SailUp = state; };
    bool SailUp;
  };

  class BoatTurnEvent : public Event
  {
  public:
    YTEDeclareType(BoatTurnEvent);
    glm::vec2 StickDirection;
  };

  class BoatDockEvent : public Event
  {
  public:
    YTEDeclareType(BoatDockEvent);
  };

  class BoatRotation : public Event
  {
  public:
    YTEDeclareType(BoatRotation);
    glm::vec3 BoatForward;
  };

  class RequestDialogueStart : public Event
  {
  public:
    YTEDeclareType(RequestDialogueStart);
  };

  class DialogueSelect : public Event
  {
  public:
    YTEDeclareType(DialogueSelect);
    glm::vec2 StickDirection;
  };

  class DialogueConfirm : public Event
  {
  public:
    YTEDeclareType(DialogueConfirm);
  };

  class DialogueExit : public Event
  {
  public:
    YTEDeclareType(DialogueExit);
  };

  class MenuStart : public Event
  {
  public:
    YTEDeclareType(MenuStart);

    Composition* ParentMenu = nullptr;
    bool PlaySound = false;
    bool ResetCursor = false;
    InputInterpreter* ContextSwitcher = nullptr;
  };

  class MenuConfirm : public Event
  {
  public:
    YTEDeclareType(MenuConfirm);
    MenuConfirm(bool aIsReleased) { IsReleased = aIsReleased; }

    bool IsHandled = false;
    bool IsReleased;
  };

  class MenuExit : public Event
  {
  public:
    YTEDeclareType(MenuExit);
    MenuExit(bool aExitAll) { ShouldExitAll = aExitAll; }

    bool ShouldExitAll;
    bool PlaySound = false;
    bool Handled = false;
  };

  class MenuElementChange : public Event
  {
  public:
    enum class Direction {Init, Previous, Next, COUNT};

    YTEDeclareType(MenuElementChange);
    MenuElementChange(Direction aChangeDirection) { ChangeDirection = aChangeDirection; }

    Direction ChangeDirection;
  };

  class DebugSwitch : public Event
  {
  public:
    YTEDeclareType(DebugSwitch);
    DebugSwitch(bool aEnableDebug) { EnableDebug = aEnableDebug; }

    bool EnableDebug;
  };

  class RequestNoticeBoardStart : public Event
  {
  public:
    YTEDeclareType(RequestNoticeBoardStart);
  };
  class HudElementToggled : public Event
  {
  public:
    enum class Element {Compass, Map, Postcard};


    YTEDeclareType(HudElementToggled);
    HudElementToggled(Element aElement) { mElement = aElement; }

    Element mElement;
  };


  /////////////////////////////////////////////////////////////////////////////////////
  // Class
  /////////////////////////////////////////////////////////////////////////////////////

  class InputInterpreter : public Component
  {
  public:
    enum class InputContext { Sailing, Dialogue, UI, Menu, Debug, num_contexts };

    YTEDeclareType(InputInterpreter);
    InputInterpreter(Composition *aOwner, Space *aSpace, RSValue *aProperties);
    void Initialize() override;

    void OnPostStart(LogicUpdate*);
    void OnLogicUpdate(LogicUpdate *aEvent);

    void OnStickEvent(XboxStickEvent *aEvent);
    void OnFlickEvent(XboxFlickEvent *aEvent);
    void OnButtonPress(XboxButtonEvent *aEvent);
    void OnButtonRelease(XboxButtonEvent *aEvent);

    void OnKeyPersist(KeyboardEvent *aEvent);
    void OnKeyPress(KeyboardEvent *aEvent);
    void OnKeyRelease(KeyboardEvent *aEvent);

    void SetInputContext(InputContext aContext);
    InputContext GetInputContext();

    // PROPERTIES //////////////////////////////////////////////////////
    ////////////////////////////////////////////////////////////////////

  private:
    XboxController *mGamepad;
    Keyboard *mKeyboard;

    Space *mMenuSpace;
    Space *mHudSpace;

    InputContext mContext;

    bool mDoneOnce;
    bool mIsRightTriggerDown;
    bool mIsLeftTriggerDown;

    bool mConstructing;
  };
}
