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
  /////////////////////////////////////////////////////////////////////////////////////
  // Events
  /////////////////////////////////////////////////////////////////////////////////////
  YTEDeclareEvent(SailStateChanged);
  YTEDeclareEvent(BoatTurnEvent);
  YTEDeclareEvent(BoatDockEvent);
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
    DirectCameraEvent(const glm::vec3& aAnchorPos, const glm::vec3& aLookAtPoint) { CameraAnchor = aAnchorPos; LookAtPoint = aLookAtPoint; }
    glm::vec3 CameraAnchor;
    glm::vec3 LookAtPoint;
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
    BoatDockEvent(const glm::vec3& aAnchorPos, const glm::vec3& aDockDir) { DockAnchorPos = aAnchorPos; DockDirection = aDockDir; }
    glm::vec3 DockAnchorPos;
    glm::vec3 DockDirection;
  };

  class RequestDialogueStart : public Event
  {
  public:
    YTEDeclareType(RequestDialogueStart);
    bool EventHandled = false;
  };

  class DialogueSelect : public Event
  {
  public:
    YTEDeclareType(DialogueSelect);
    glm::vec2 StickDirection;
    bool EventHandled = false;
  };

  class DialogueConfirm : public Event
  {
  public:
    YTEDeclareType(DialogueConfirm);
    bool EventHandled = false;
  };

  class DialogueExit : public Event
  {
  public:
    YTEDeclareType(DialogueExit);
    bool EventHandled = false;
  };

  class MenuStart : public Event
  {
  public:
    YTEDeclareType(MenuStart);

    Composition* ParentMenu = nullptr;
    bool PlaySound = false;
  };

  class MenuConfirm : public Event
  {
  public:
    YTEDeclareType(MenuConfirm);
    MenuConfirm(bool aIsReleased) { IsReleased = aIsReleased; }

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
    enum class Direction {Previous, Next, COUNT};

    YTEDeclareType(MenuElementChange);
    MenuElementChange(Direction aChangeDirection) { ChangeDirection = aChangeDirection; }

    Direction ChangeDirection;
  };


  /////////////////////////////////////////////////////////////////////////////////////
  // Class
  /////////////////////////////////////////////////////////////////////////////////////

  class InputInterpreter : public Component
  {
  public:
    enum class InputContext { Sailing, Dialogue, UI, Menu, num_contexts };

    YTEDeclareType(InputInterpreter);
    InputInterpreter(Composition *aOwner, Space *aSpace, RSValue *aProperties);
    void Initialize() override;

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

    InputContext mContext;

    bool mIsRightTriggerDown;
    bool mIsLeftTriggerDown;

    bool mConstructing;
  };
}
