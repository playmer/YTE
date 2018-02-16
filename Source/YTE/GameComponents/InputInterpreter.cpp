/******************************************************************************/
/*!
\file   InputInterpreter.cpp
\author Jonathan Ackerman
        Isaac Dayton
\par    email: jonathan.ackerman\@digipen.edu
\date   2018-01-19

All content (c) 2016 DigiPen  (USA) Corporation, all rights reserved.
*/
/******************************************************************************/

#include "YTE/GameComponents/InputInterpreter.hpp"

namespace YTE
{
    // Dialogue Events
  YTEDefineEvent(DialogueStart);
  YTEDefineEvent(DialogueConfirm);
  YTEDefineEvent(DialogueExit);

    // Menu Events
  YTEDefineEvent(MenuStart);
  YTEDefineEvent(MenuConfirm);
  YTEDefineEvent(MenuExit);

    // Boat Events
  YTEDefineEvent(SailStateChanged);
  YTEDefineEvent(BoatTurnEvent);
  YTEDefineEvent(BoatDockEvent);

    // Camera Events
  YTEDefineEvent(CameraRotateEvent);

  YTEDefineType(DialogueStart) { YTERegisterType(DialogueStart); }
  YTEDefineType(DialogueConfirm) { YTERegisterType(DialogueConfirm); }
  YTEDefineType(DialogueExit) { YTERegisterType(DialogueExit); }
  YTEDefineType(MenuStart) { YTERegisterType(MenuStart); }
  YTEDefineType(MenuConfirm) { YTERegisterType(MenuConfirm); }
  YTEDefineType(MenuExit) { YTERegisterType(MenuExit); }
  YTEDefineType(SailStateChanged) { YTERegisterType(SailStateChanged); }
  YTEDefineType(BoatTurnEvent) { YTERegisterType(BoatTurnEvent); }
  YTEDefineType(BoatDockEvent) { YTERegisterType(BoatDockEvent); }
  YTEDefineType(CameraRotateEvent) { YTERegisterType(CameraRotateEvent); }


  YTEDefineType(InputInterpreter)
  {
    YTERegisterType(InputInterpreter);

    YTEBindProperty(&GetRootMenuName, &SetRootMenuName, "PauseMenuName")
      .AddAttribute<Serializable>()
      .AddAttribute<EditorProperty>()
      .SetDocumentation("The root pause menu that should open when \"start\" is pressed -- Sure don't love string properties -- ID");
  }

  InputInterpreter::InputInterpreter(Composition *aOwner, Space *aSpace, RSValue *aProperties)
    : Component(aOwner, aSpace), mGamepad(nullptr), mConstructing(true)
  {
    DeserializeByType(aProperties, this, GetStaticType());
    mConstructing = false;
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
    if (mContext == InputContext::Sailing)
    {
      if (aEvent->Stick == Xbox_Buttons::LeftStick)
      {
        BoatTurnEvent turnEvent;
        turnEvent.StickDirection = aEvent->StickDirection;
        mOwner->SendEvent(Events::BoatTurnEvent, &turnEvent);
      }
      else if (aEvent->Stick == Xbox_Buttons::RightStick)
      {
        CameraRotateEvent camRot;
        camRot.StickDirection = aEvent->StickDirection;
        mOwner->SendEvent(Events::CameraRotateEvent, &camRot);
      }
    }
  }

  void InputInterpreter::CheckButtons(XboxButtonEvent *aEvent)
  {
    switch (mContext)
    {
    case InputContext::Dialogue:
    {
      switch (aEvent->Button)
      {
      case Xbox_Buttons::Y:
      {
        DialogueStart diagStart;
        mOwner->SendEvent(Events::DialogueStart, &diagStart);
        break;
      }

      case Xbox_Buttons::A:
      {
        DialogueConfirm diagConfirm;
        mOwner->SendEvent(Events::DialogueConfirm, &diagConfirm);
        break;
      }
      case Xbox_Buttons::B:
      {
        DialogueExit diagExit;
        mOwner->SendEvent(Events::DialogueExit, &diagExit);
        break;
      }
      }
      break;
    }
    case InputContext::Sailing:
    {
      switch (aEvent->Button)
      {
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
      {
        mContext = InputContext::Menu;

        MenuStart menuStart(mRootPauseMenuName);
        mOwner->SendEvent(Events::MenuStart, &menuStart);
        break;
      }
      case Xbox_Buttons::LeftShoulder:
        break;
      case Xbox_Buttons::RightShoulder:
        break;
      case Xbox_Buttons::LeftStick:
        break;
      case Xbox_Buttons::RightStick:
        break;
      }
      break;
    }

    case InputContext::Menu:
    {
      switch (aEvent->Button)
      {
      case Xbox_Buttons::Start:
        break;

      case Xbox_Buttons::Back:
        break;

      case Xbox_Buttons::A:
        break;

      case Xbox_Buttons::B:
        break;
      }

      break;
    }

    }
  }
}
