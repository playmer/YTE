/******************************************************************************/
/*!
\file   InputInterpreter.hpp
\author Jonathan Ackerman
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
      Xbox_Buttons Stick;
    };

    class BoatDockEvent : public Event
    {
    public:
      YTEDeclareType(BoatDockEvent);
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
    private:
        XboxController *mGamepad;
        InputContext mContext;

    };
} // End YTE namespace

#endif
