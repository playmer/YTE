/******************************************************************************/
/*!
\file   InputInterpreter.hpp
\author Jonathan Ackerman
\par    email: jonathan.ackerman\@digipen.edu
\date   2018-01-19
\brief
    An interface for gameplay logic components. This component will listen
    to input events and translate them to a format gameplay components can use 
    and then the interface will dispatch that info in gameplay events
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

#include "YTE/Platform/Gamepad.hpp"
#include "YTE/Platform/GamepadSystem.hpp"
#include "YTE/Platform/DeviceEnums.hpp"

namespace YTE
{
    YTEDeclareEvent(ToggleSailEvent);

    class ToggleSailEvent : public Event
    {
    public:
        YTEDeclareType(ToggleSailEvent);
        enum class States { sail_down, sail_up, num_states };
        States SailState;
    };

    class InputInterpreter : public Component
    {
    public:
        YTEDeclareType(InputInterpreter);
        InputInterpreter(Composition *aOwner, Space *aSpace);
        void Initialize() override;

        //definitely wrong
        void LogicUpdate();

        //maybe the input focus is here (ie what state are we in, gameplay, ui, etc)
    private:
        XboxController *mGamepad;
        GamepadSystem *mGamepadSystem;
        //
    };
} // End YTE namespace

#endif
