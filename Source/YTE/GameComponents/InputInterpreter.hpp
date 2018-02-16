/******************************************************************************/
/*!
\file   InputInterpreter.hpp
\author Jonathan Ackerman
        Isaac Dayton
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

#ifndef YTE_Gameplay_InputInterpreter_hpp
#define YTE_Gameplay_InputInterpreter_hpp

#include "YTE/Core/Composition.hpp"
#include "YTE/Core/ForwardDeclarations.hpp"
#include "YTE/Core/Engine.hpp"

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
  YTEDeclareEvent(DialogueStart);
  YTEDeclareEvent(DialogueConfirm);
  YTEDeclareEvent(DialogueExit);
  YTEDeclareEvent(CameraRotateEvent);
  YTEDeclareEvent(MenuStart);
  YTEDeclareEvent(MenuConfirm);
  YTEDeclareEvent(MenuExit);

  class CameraRotateEvent : public Event
  {
  public:
    YTEDeclareType(CameraRotateEvent);
    glm::vec2 StickDirection;
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

  class DialogueStart : public Event
  {
  public:
    YTEDeclareType(DialogueStart);
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
    MenuStart(std::string aMenuName) { mMenuName = aMenuName; }
    std::string mMenuName;
  };

  class MenuConfirm : public Event
  {
  public:
    YTEDeclareType(MenuConfirm);
  };

  class MenuExit : public Event
  {
  public:
    YTEDeclareType(MenuExit);
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
    void CheckSticks(XboxStickEvent *aEvent);
    void CheckButtons(XboxButtonEvent *aEvent);

    void SetInputContext(InputContext aContext);
    InputContext GetInputContext();

    // PROPERTIES //////////////////////////////////////////////////////
    std::string GetRootMenuName() { return mRootPauseMenuName; }
    void SetRootMenuName(std::string& aRootMenuName) { mRootPauseMenuName = aRootMenuName; }
    ////////////////////////////////////////////////////////////////////

  private:
    XboxController *mGamepad;
    std::string mRootPauseMenuName;
    InputContext mContext;

    bool mConstructing;
  };
} // End YTE namespace

#endif
