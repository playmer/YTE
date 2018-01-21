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
    YTEDefineEvent(SailStateChanged);

    YTEDefineType(SailStateChanged)
    {
        YTERegisterType(SailStateChanged);
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
        mOwner->YTERegister(Events::LogicUpdate, this, &InputInterpreter::CheckSticks);
        mOwner->YTERegister(Events::XboxButtonPress, this, &InputInterpreter::CheckButtons);

        mGamepad = mOwner->GetEngine()->GetGamepadSystem()->GetXboxController(YTE::Controller_Id::Xbox_P1);
    }
/******************************************************************************/
/*
    Event Callbacks
*/
/******************************************************************************/
    void InputInterpreter::CheckSticks(LogicUpdate *aEvent)
    {
        YTEUnusedArgument(aEvent);

        glm::vec2 LS = mGamepad->GetLeftStick();
        glm::vec2 RS = mGamepad->GetRightStick();
        // do some math here
    }
    
    void InputInterpreter::CheckButtons(XboxButtonEvent *aEvent)
    {
        switch (mContext) {
        case InputContext::Sailing:
            switch (aEvent->Button) {
            case Xbox_Buttons::DPAD_Down: 
            {
                SailStateChanged sailEvent(true);
                mOwner->SendEvent(Events::SailStateChanged, &sailEvent);
                break;
            }
            case Xbox_Buttons::DPAD_Up: 
            {
                SailStateChanged sailEvent(false);
                mOwner->SendEvent(Events::SailStateChanged, &sailEvent);
                break;
            }
            case Xbox_Buttons::DPAD_Left:
                break;
            case Xbox_Buttons::DPAD_Right:
                break;
            case Xbox_Buttons::A:
                break;
            case Xbox_Buttons::B:
                break;
            case Xbox_Buttons::X:
                break;
            case Xbox_Buttons::Y:
                break;
            case Xbox_Buttons::Start:
                break;
            case Xbox_Buttons::LeftShoulder:
                break;
            case Xbox_Buttons::RightShoulder:
                break;
            case Xbox_Buttons::LeftStick:
                break;
            case Xbox_Buttons::RightStick:
                break;
            }
        }
    }
    
} // End yte namespace