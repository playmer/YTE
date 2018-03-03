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
  YTEDefineEvent(MenuElementChange);

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
  YTEDefineType(MenuElementChange) { YTERegisterType(MenuElementChange); }
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
    : Component(aOwner, aSpace), mGamepad(nullptr), mIsRightTriggerDown(false), mIsLeftTriggerDown(false), mConstructing(true)
  {
    DeserializeByType(aProperties, this, GetStaticType());
    mConstructing = false;
  }

  void InputInterpreter::Initialize()
  {
    mGamepad = mOwner->GetEngine()->GetGamepadSystem()->GetXboxController(YTE::Controller_Id::Xbox_P1);
    mContext = InputContext::Sailing;
    
		mSpace->YTERegister(Events::LogicUpdate, this, &InputInterpreter::OnLogicUpdate);
    mGamepad->YTERegister(Events::XboxStickEvent, this, &InputInterpreter::OnStickEvent);
    mGamepad->YTERegister(Events::XboxStickFlicked, this, &InputInterpreter::OnFlickEvent);
    mGamepad->YTERegister(Events::XboxButtonPress, this, &InputInterpreter::OnButtonPress);
    mGamepad->YTERegister(Events::XboxButtonRelease, this, &InputInterpreter::OnButtonRelease);
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
	void InputInterpreter::OnLogicUpdate(LogicUpdate *aEvent)
	{
		if (mContext == InputContext::Sailing)
		{
			if (mIsRightTriggerDown)
			{
				if (mGamepad->GetRightTrigger() < 0.1f)
				{
					mIsRightTriggerDown = false;
				}
			}
			else
			{
				if (mGamepad->GetRightTrigger() > 0.5f)
				{
					SailStateChanged setSailUp(true);
					mOwner->SendEvent(Events::SailStateChanged, &setSailUp);

					mIsRightTriggerDown = true;
				}
			}

			if (mIsLeftTriggerDown)
			{
				if (mGamepad->GetLeftTrigger() < 0.1f)
				{
					mIsLeftTriggerDown = false;
				}
			}
			else
			{
				if (mGamepad->GetLeftTrigger() > 0.5f)
				{
					SailStateChanged setSailUp(false);
					mOwner->SendEvent(Events::SailStateChanged, &setSailUp);

					mIsLeftTriggerDown = true;
				}
			}
		}
	}
	
	void InputInterpreter::OnStickEvent(XboxStickEvent *aEvent)
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

  void InputInterpreter::OnFlickEvent(XboxFlickEvent *aEvent)
  {
    if (mContext == InputContext::Menu)
    {
      if (aEvent->FlickedStick == Xbox_Buttons::LeftStick)
      {
        if (aEvent->FlickDirection.x < 0.01f)
        {
          MenuElementChange menuPrev(MenuElementChange::Direction::Previous);

          mOwner->SendEvent(Events::MenuElementChange, &menuPrev);
        }

        else if (aEvent->FlickDirection.x > 0.01f)
        {
          MenuElementChange menuNext(MenuElementChange::Direction::Next);

          mOwner->SendEvent(Events::MenuElementChange, &menuNext);
        }
      }
    }
  }

  void InputInterpreter::OnButtonPress(XboxButtonEvent *aEvent)
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
        break;
      case Xbox_Buttons::DPAD_Up:
        break;
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
        MenuStart menuStart(mRootPauseMenuName);
        mOwner->SendEvent(Events::MenuStart, &menuStart);

        mContext = InputContext::Menu;
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
      case Xbox_Buttons::Back:
      case Xbox_Buttons::Start:
      {
        MenuExit menuExit(true);
        mOwner->SendEvent(Events::MenuExit, &menuExit);

        mContext = InputContext::Sailing;
        break;
      }

      case Xbox_Buttons::A:
      {
        MenuConfirm menuConfirm(false);
        mOwner->SendEvent(Events::MenuConfirm, &menuConfirm);
        break;
      }

      case Xbox_Buttons::B:
      {
        MenuExit menuExit(false);
        mOwner->SendEvent(Events::MenuExit, &menuExit);

        break;
      }

      case Xbox_Buttons::DPAD_Left:
      {
        MenuElementChange menuPrev(MenuElementChange::Direction::Previous);
        mOwner->SendEvent(Events::MenuElementChange, &menuPrev);

        break;
      }

      case Xbox_Buttons::DPAD_Right:
      {
        MenuElementChange menuNext(MenuElementChange::Direction::Next);
        mOwner->SendEvent(Events::MenuElementChange, &menuNext);

        break;
      }
      }

      break;
    }

    }
  }

  void InputInterpreter::OnButtonRelease(XboxButtonEvent *aEvent)
  {
    switch (mContext)
    {
    case InputContext::Menu:
    {
      if (aEvent->Button == Xbox_Buttons::A)
      {
        MenuConfirm menuConfirm(true);
        mOwner->SendEvent(Events::MenuConfirm, &menuConfirm);
        break;
      }
    }
    }
  }
}
