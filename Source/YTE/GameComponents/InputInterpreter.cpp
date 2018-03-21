/******************************************************************************/
/*!
\file   InputInterpreter.cpp
\author Isaac Dayton
        Jonathan Ackerman
\par    email: jonathan.ackerman\@digipen.edu
\date   2018-01-19

All content (c) 2016 DigiPen  (USA) Corporation, all rights reserved.
*/
/******************************************************************************/

#include "YTE/GameComponents/InputInterpreter.hpp"
#include "YTE/Graphics/GraphicsView.hpp"

namespace YTE
{
  // Dialogue Events
  YTEDefineEvent(RequestDialogueStart);
  YTEDefineEvent(DialogueSelect);
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
  YTEDefineEvent(DirectCameraEvent);

  // Debug Events
  YTEDefineEvent(DebugSwitch);

  YTEDefineType(RequestDialogueStart) { YTERegisterType(RequestDialogueStart); }
  YTEDefineType(DialogueSelect) { YTERegisterType(DialogueSelect); }
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
  YTEDefineType(DirectCameraEvent) { YTERegisterType(DirectCameraEvent); }
  YTEDefineType(DebugSwitch) { YTERegisterType(DebugSwitch); }

  YTEDefineType(InputInterpreter)
  {
    YTERegisterType(InputInterpreter);
  }

  InputInterpreter::InputInterpreter(Composition *aOwner, Space *aSpace, RSValue *aProperties)
    : Component(aOwner, aSpace), mGamepad(nullptr), mIsRightTriggerDown(false), mIsLeftTriggerDown(false), mConstructing(true)
  {
    DeserializeByType(aProperties, this, GetStaticType());
    mConstructing = false;
  }

  void InputInterpreter::Initialize()
  {
    mGamepad = mOwner->GetEngine()->GetGamepadSystem()->GetXboxController(YTE::ControllerId::Xbox_P1);
    mKeyboard = &mSpace->GetComponent<GraphicsView>()->GetWindow()->mKeyboard;
    mContext = InputContext::Sailing;

    mMenuSpace = mSpace->AddChildSpace("MSR_Menus");

    mSpace->YTERegister(Events::LogicUpdate, this, &InputInterpreter::OnLogicUpdate);

    mGamepad->YTERegister(Events::XboxStickEvent, this, &InputInterpreter::OnStickEvent);
    mGamepad->YTERegister(Events::XboxStickFlicked, this, &InputInterpreter::OnFlickEvent);
    mGamepad->YTERegister(Events::XboxButtonPress, this, &InputInterpreter::OnButtonPress);
    mGamepad->YTERegister(Events::XboxButtonRelease, this, &InputInterpreter::OnButtonRelease);

    mKeyboard->YTERegister(Events::KeyPersist, this, &InputInterpreter::OnKeyPersist);
    mKeyboard->YTERegister(Events::KeyPress, this, &InputInterpreter::OnKeyPress);
    mKeyboard->YTERegister(Events::KeyRelease, this, &InputInterpreter::OnKeyRelease);
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
    YTEUnusedArgument(aEvent);

    if (mContext == InputContext::Sailing)
    {
      if (mIsRightTriggerDown && mGamepad->GetRightTrigger() < 0.1f)
      {
          mIsRightTriggerDown = false;
      }
      else if (!mIsRightTriggerDown && mGamepad->GetRightTrigger() > 0.5f)
      {
        SailStateChanged setSailUp(true);
        mOwner->SendEvent(Events::SailStateChanged, &setSailUp);

        mIsRightTriggerDown = true;
      }

      if (mIsLeftTriggerDown && mGamepad->GetLeftTrigger() < 0.1f)
      {
        mIsLeftTriggerDown = false;
      }
      else if (!mIsLeftTriggerDown && mGamepad->GetLeftTrigger() > 0.5f)
      {
        SailStateChanged setSailUp(false);
        mOwner->SendEvent(Events::SailStateChanged, &setSailUp);

        mIsLeftTriggerDown = true;
      }
    }
  }

  void InputInterpreter::OnStickEvent(XboxStickEvent *aEvent)
  {
    if (mContext == InputContext::Sailing)
    {
      if (aEvent->Stick == XboxButtons::LeftStick)
      {
        BoatTurnEvent turnEvent;
        turnEvent.StickDirection = aEvent->StickDirection;
        mOwner->SendEvent(Events::BoatTurnEvent, &turnEvent);
      }
      else if (aEvent->Stick == XboxButtons::RightStick)
      {
        CameraRotateEvent camRot;
        camRot.StickDirection = aEvent->StickDirection;
        mOwner->SendEvent(Events::CameraRotateEvent, &camRot);
      }
    }

    else if (mContext == InputContext::Dialogue) 
    {
      if (aEvent->Stick == XboxButtons::LeftStick)
      {
        DialogueSelect selectEvent;
        selectEvent.StickDirection = aEvent->StickDirection;
        mOwner->SendEvent(Events::DialogueSelect, &selectEvent);
      }
    }
  }

  void InputInterpreter::OnFlickEvent(XboxFlickEvent *aEvent)
  {
    if (mContext == InputContext::Menu)
    {
      if (aEvent->FlickedStick == XboxButtons::LeftStick)
      {
        glm::vec2 flickDir = aEvent->FlickDirection;

          // Check dead-zone
        if (glm::length(flickDir) > 0.01f)
        {
            // Transform the flick direction by 45 degrees
            // Pressing something mostly-right or mostly-down should progess to the next
            //   menu element. Mostly-left or mostly-up goes back.
            // This transform makes the following checks easier by allowing us to simply check
            //   if the x component is greater than 0 (0 is okay here because we've already passed
            //   our dead-zone check).
          float cos45 = glm::cos(glm::quarter_pi<float>());
          float sin45 = glm::cos(glm::quarter_pi<float>());
          glm::vec2 rotatedDir(flickDir.x * cos45 - flickDir.y * sin45, flickDir.x * sin45 + flickDir.y * cos45);

            // Check forward region
          if (rotatedDir.x > 0.0f)
          {
            MenuElementChange menuNext(MenuElementChange::Direction::Next);

            mMenuSpace->SendEvent(Events::MenuElementChange, &menuNext);
          }

          else //(aEvent->FlickDirection.x < 0.01f)
          {
            MenuElementChange menuPrev(MenuElementChange::Direction::Previous);

            mMenuSpace->SendEvent(Events::MenuElementChange, &menuPrev);
          }
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
          case XboxButtons::A:
          {
            DialogueConfirm diagConfirm;
            mOwner->SendEvent(Events::DialogueConfirm, &diagConfirm);
            break;
          }
          case XboxButtons::B:
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
          case XboxButtons::DPAD_Down:
            break;
          case XboxButtons::DPAD_Up:
            break;
          case XboxButtons::DPAD_Left:
            break;
          case XboxButtons::DPAD_Right:
            break;
          case XboxButtons::A:
          {
            RequestDialogueStart dock;
            mOwner->SendEvent(Events::RequestDialogueStart, &dock);
            break;
          }
          case XboxButtons::B:
            break;
          case XboxButtons::X:
            break;
          case XboxButtons::Y:
            break;
          case XboxButtons::Start:
          {
            MenuStart menuStart;
            menuStart.PlaySound = true;
            mMenuSpace->SendEvent(Events::MenuStart, &menuStart);
            mSpace->SendEvent(Events::MenuStart, &menuStart);

            mContext = InputContext::Menu;
            break;
          }
          case XboxButtons::LeftShoulder:
            break;
          case XboxButtons::RightShoulder:
            break;
          case XboxButtons::LeftStick:
            break;
          case XboxButtons::RightStick:
            break;
        }
        break;
      }

      case InputContext::Menu:
      {
        switch (aEvent->Button)
        {
          case XboxButtons::Back:
          case XboxButtons::Start:
          {
            MenuExit menuExit(true);
            menuExit.PlaySound = true;
            menuExit.ContextSwitcher = this;
            mMenuSpace->SendEvent(Events::MenuExit, &menuExit);

            mContext = InputContext::Sailing;
            break;
          }

          case XboxButtons::A:
          {
            MenuConfirm menuConfirm(false);
            mMenuSpace->SendEvent(Events::MenuConfirm, &menuConfirm);
            break;
          }

          case XboxButtons::B:
          {
            MenuExit menuExit(false);
            menuExit.PlaySound = true;
            menuExit.ContextSwitcher = this;
            mMenuSpace->SendEvent(Events::MenuExit, &menuExit);

            break;
          }

          case XboxButtons::DPAD_Up:
          case XboxButtons::DPAD_Left:
          {
            MenuElementChange menuPrev(MenuElementChange::Direction::Previous);
            mMenuSpace->SendEvent(Events::MenuElementChange, &menuPrev);

            break;
          }

          case XboxButtons::DPAD_Down:
          case XboxButtons::DPAD_Right:
          {
            MenuElementChange menuNext(MenuElementChange::Direction::Next);
            mMenuSpace->SendEvent(Events::MenuElementChange, &menuNext);

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
        if (aEvent->Button == XboxButtons::A)
        {
          MenuConfirm menuConfirm(true);
          mMenuSpace->SendEvent(Events::MenuConfirm, &menuConfirm);
          break;
        }
      }
    }
  }

  void InputInterpreter::OnKeyPersist(KeyboardEvent *aEvent)
  {
    if (mContext == InputContext::Sailing)
    {
      switch (aEvent->Key)
      {
        case Keys::A:
        case Keys::Left:
        {
          BoatTurnEvent turnEvent;
          turnEvent.StickDirection = glm::vec2(-1.0f, 0.f);
          mOwner->SendEvent(Events::BoatTurnEvent, &turnEvent);

          break;
        }

        case Keys::D:
        case Keys::Right:
        {
          BoatTurnEvent turnEvent;
          turnEvent.StickDirection = glm::vec2(1.0f, 0.f);
          mOwner->SendEvent(Events::BoatTurnEvent, &turnEvent);

          break;
        }
      }
    }
  }

  void InputInterpreter::OnKeyPress(KeyboardEvent *aEvent)
  {
    switch (mContext)
    {
      case InputContext::Debug:
      {
        switch (aEvent->Key)
        {
          case Keys::F1:
          {
            DebugSwitch setDebug(false);
            mOwner->SendEvent(Events::DebugSwitch, &setDebug);

            mContext = InputContext::Sailing;

            break;
          }
        }
      }

      case InputContext::Dialogue:
      {
        break;
      }
      case InputContext::Sailing:
      {
        switch (aEvent->Key)
        {
          case Keys::Return:
          {
            RequestDialogueStart dock;
            mOwner->SendEvent(Events::RequestDialogueStart, &dock);
            break;
          }
          case Keys::Escape:
          {
            MenuStart menuStart;
            menuStart.PlaySound = true;
            mMenuSpace->SendEvent(Events::MenuStart, &menuStart);
            mSpace->SendEvent(Events::MenuStart, &menuStart);

            mContext = InputContext::Menu;
            break;
          }

          case Keys::W:
          case Keys::Up:
          {
            SailStateChanged setSailUp(true);
            mOwner->SendEvent(Events::SailStateChanged, &setSailUp);

            break;
          }

          case Keys::S:
          case Keys::Down:
          {
            SailStateChanged setSailUp(false);
            mOwner->SendEvent(Events::SailStateChanged, &setSailUp);

            break;
          }

          case Keys::F1:
          {
            DebugSwitch setDebug(true);
            mOwner->SendEvent(Events::DebugSwitch, &setDebug);

            mContext = InputContext::Debug;

            break;
          }
        }
        break;
      }

      case InputContext::Menu:
      {
        switch (aEvent->Key)
        {
          case Keys::Escape:
          {
            MenuExit menuExit(true);
            menuExit.PlaySound = true;
            menuExit.ContextSwitcher = this;
            mMenuSpace->SendEvent(Events::MenuExit, &menuExit);

            mContext = InputContext::Sailing;
            break;
          }

          case Keys::Return:
          {
            MenuConfirm menuConfirm(false);
            mMenuSpace->SendEvent(Events::MenuConfirm, &menuConfirm);

            break;
          }

          case Keys::Backspace:
          {
            MenuExit menuExit(false);
            menuExit.PlaySound = true;
            menuExit.ContextSwitcher = this;
            mMenuSpace->SendEvent(Events::MenuExit, &menuExit);

            break;
          }

          case Keys::W:
          case Keys::Up:
          case Keys::A:
          case Keys::Left:
          {
            MenuElementChange menuPrev(MenuElementChange::Direction::Previous);
            mMenuSpace->SendEvent(Events::MenuElementChange, &menuPrev);

            break;
          }

          case Keys::S:
          case Keys::Down:
          case Keys::D:
          case Keys::Right:
          {
            MenuElementChange menuNext(MenuElementChange::Direction::Next);
            mMenuSpace->SendEvent(Events::MenuElementChange, &menuNext);

            break;
          }
        }

        break;
      }

    }
  }

  void InputInterpreter::OnKeyRelease(KeyboardEvent *aEvent)
  {
    switch (mContext)
    {
      case InputContext::Sailing:
      {
        switch (aEvent->Key)
        {
          case Keys::A:
          case Keys::D:
          case Keys::Left:
          case Keys::Right:
          {
            BoatTurnEvent turnEvent;
            turnEvent.StickDirection = glm::vec2(0.0f, 0.f);
            mOwner->SendEvent(Events::BoatTurnEvent, &turnEvent);

            break;
          }
        }
      }

      case InputContext::Menu:
      {
        if (aEvent->Key == Keys::Return)
        {
          MenuConfirm menuConfirm(true);
          mMenuSpace->SendEvent(Events::MenuConfirm, &menuConfirm);
          break;
        }
      }
    }
  }
}
