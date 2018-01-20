/******************************************************************************/
/*!
\file   InputInterpreter.cpp
\author Jonathan Ackerman
\par    email: jonathan.ackerman\@digipen.edu
\date   2018-01-19

All content (c) 2016 DigiPen  (USA) Corporation, all rights reserved.
*/
/******************************************************************************/

#include "YTE/GameComponents/InputInterpreter.hpp"

namespace YTE
{
    YTEDefineEvent(ToggleSailEvent);

    YTEDefineType(ToggleSailEvent)
    {
        YTERegisterType(ToggleSailEvent);
        //YTEBindField(&ToggleSailEvent::SailState, "SailState", PropertyBinding::Get);
    }

    YTEDefineType(InputInterpreter)
    {
        YTERegisterType(InputInterpreter);
    }

    InputInterpreter::InputInterpreter(Composition *aOwner, Space *aSpace)
        : Component(aOwner, aSpace)
    {
    }

    void InputInterpreter::Initialize()
    {
        //this doesnt seem right?
        mGamepadSystem = new GamepadSystem;
        mGamepad = mGamepadSystem->GetXboxController(YTE::Controller_Id::Xbox_P1);
    }

    void InputInterpreter::LogicUpdate()
    {
        ToggleSailEvent sailChanged;
        if (mGamepad->IsButtonDown(YTE::Xbox_Buttons::DPAD_Down))
        {
            sailChanged.SailState = ToggleSailEvent::States::sail_down;
        }
        else if (mGamepad->IsButtonDown(YTE::Xbox_Buttons::DPAD_Up))
        {
            sailChanged.SailState = ToggleSailEvent::States::sail_up;
        }
        mOwner->SendEvent(Events::ToggleSailEvent, &sailChanged);
    }
} // End yte namespace