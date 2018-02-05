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
    YTEDefineEvent(BoatTurnEvent);
    YTEDefineEvent(BoatDockEvent);

    YTEDefineType(SailStateChanged) { YTERegisterType(SailStateChanged); }
    YTEDefineType(BoatTurnEvent) { YTERegisterType(BoatTurnEvent); }
    YTEDefineType(BoatDockEvent) { YTERegisterType(BoatDockEvent); }


    YTEDefineType(InputInterpreter)
    {
        YTERegisterType(InputInterpreter);
    }

    InputInterpreter::InputInterpreter(Composition *aOwner, Space *aSpace, RSValue *aProperties)
        : Component(aOwner, aSpace)
    {
        YTEUnusedArgument(aProperties);
    }

    void InputInterpreter::Initialize()
    {
        mGamepad = mOwner->GetEngine()->GetGamepadSystem()->GetXboxController(YTE::Controller_Id::Xbox_P1);
        mContext = InputContext::Sailing;
        //mSpace->YTERegister(Events::LogicUpdate, this, &InputInterpreter::CheckSticks);
        //auto space = mOwner->GetEngine()->GetSpace();
        mGamepad->YTERegister(Events::XboxStickEvent, this, &InputInterpreter::CheckSticks);
        mGamepad->YTERegister(Events::XboxButtonPress, this, &InputInterpreter::CheckButtons);
    }

    void InputInterpreter::SetInputContext(InputInterpreter::InputContext aContext)
    {
      mContext = aContext;
    }
    InputInterpreter::InputContext InputInterpreter::GetInputContext()
    {
      return mContext;
    }
/******************************************************************************/
/*
    Event Callbacks
*/
/******************************************************************************/
    void InputInterpreter::CheckSticks(XboxStickEvent *aEvent)
    {
      //std::cout << "Stick (" << aEvent->StickDirection.x << ", " << aEvent->StickDirection.y << ")\n";
      BoatTurnEvent turnEvent;
      turnEvent.Stick = aEvent->Stick;
      turnEvent.StickDirection = aEvent->StickDirection;
      mOwner->SendEvent(Events::BoatTurnEvent, &turnEvent);
    }
    
    void InputInterpreter::CheckButtons(XboxButtonEvent *aEvent)
    {
        switch (mContext) 
        {
        case InputContext::Dialogue:
          switch (aEvent->Button) 
          {
          case Xbox_Buttons::A:
            // dialogue confirm event
            break;
          case Xbox_Buttons::B:
            // dialogue quit event
            break;
          }
        case InputContext::Sailing:
          switch (aEvent->Button) {
          case Xbox_Buttons::DPAD_Down: 
          {
            SailStateChanged setSailUp(false);
            mOwner->SendEvent(Events::SailStateChanged, &setSailUp);
            break;
          }
          case Xbox_Buttons::DPAD_Up: 
          {
            SailStateChanged setSailUp(true);
            mOwner->SendEvent(Events::SailStateChanged, &setSailUp);
            break;
          }
          case Xbox_Buttons::DPAD_Left:
            break;
          case Xbox_Buttons::DPAD_Right:
            break;
          case Xbox_Buttons::A:
          {
            BoatDockEvent dock;
            mOwner->SendEvent(Events::BoatDockEvent, &dock);
            break;
          }
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