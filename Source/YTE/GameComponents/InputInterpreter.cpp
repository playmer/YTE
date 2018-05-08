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

#include "YTE/Physics/PhysicsSystem.hpp"
#include "YTE/GameComponents/InputInterpreter.hpp"
#include "YTE/Graphics/GraphicsView.hpp"
#include "YTE/WWise/WWiseSystem.hpp"
#include "YTE/GameComponents/NoticeBoard.hpp"

namespace YTE
{
  YTEDefineEvent(RequestNoticeBoardStart);
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

  // Option Events
  YTEDefineEvent(OptionsStickEvent);
  YTEDefineEvent(OptionsFlickEvent);
  YTEDefineEvent(OptionsConfirmEvent);
  YTEDefineEvent(MuteBypass);

  // Boat Events
  YTEDefineEvent(SailStateChanged);
  YTEDefineEvent(BoatTurnEvent);
  YTEDefineEvent(BoatDockEvent);
  YTEDefineEvent(BoatRotation);

  // Camera Events
  YTEDefineEvent(CameraRotateEvent);
  YTEDefineEvent(DirectCameraEvent);

  // Debug Events
  YTEDefineEvent(DebugSwitch);

  // Hud Events
  YTEDefineEvent(HudElementToggled);

  YTEDefineType(RequestDialogueStart)    { RegisterType<RequestDialogueStart>(); }
  YTEDefineType(RequestNoticeBoardStart) { RegisterType<RequestNoticeBoardStart>(); }
  YTEDefineType(DialogueSelect)          { RegisterType<DialogueSelect>(); }
  YTEDefineType(DialogueConfirm)         { RegisterType<DialogueConfirm>(); }
  YTEDefineType(DialogueExit)            { RegisterType<DialogueExit>(); }
  YTEDefineType(MenuStart)               { RegisterType<MenuStart>(); }
  YTEDefineType(MenuConfirm)             { RegisterType<MenuConfirm>(); }
  YTEDefineType(MenuExit)                { RegisterType<MenuExit>(); }
  YTEDefineType(MenuElementChange)       { RegisterType<MenuElementChange>(); }
  YTEDefineType(OptionsStickEvent)       { RegisterType<OptionsStickEvent>(); }
  YTEDefineType(OptionsFlickEvent)       { RegisterType<OptionsFlickEvent>(); }
  YTEDefineType(OptionsConfirmEvent)     { RegisterType<OptionsConfirmEvent>(); }
  YTEDefineType(MuteBypass)              { RegisterType<MuteBypass>(); }
  YTEDefineType(SailStateChanged)        { RegisterType<SailStateChanged>(); }
  YTEDefineType(BoatTurnEvent)           { RegisterType<BoatTurnEvent>(); }
  YTEDefineType(BoatDockEvent)           { RegisterType<BoatDockEvent>(); }
  YTEDefineType(BoatRotation)            { RegisterType<BoatRotation>(); }
  YTEDefineType(CameraRotateEvent)       { RegisterType<CameraRotateEvent>(); }
  YTEDefineType(DirectCameraEvent)       { RegisterType<DirectCameraEvent>(); }
  YTEDefineType(DebugSwitch)             { RegisterType<DebugSwitch>(); }
  YTEDefineType(HudElementToggled)       { RegisterType<HudElementToggled>(); }

  YTEDefineType(InputInterpreter)
  {
    RegisterType<InputInterpreter>();
  }

  InputInterpreter::InputInterpreter(Composition *aOwner, Space *aSpace, RSValue *aProperties)
    : Component(aOwner, aSpace)
    , mView(nullptr)
    , mGamepad(nullptr)
    , mKeyboard(nullptr)
    , mMenuSpace(nullptr)
    , mHudSpace(nullptr)
    , mCreditsSpace(nullptr)
    , mDoneOnce(false)
    , mIsRightTriggerDown(false)
    , mIsLeftTriggerDown(false)
    , mConstructing(true)
  {
    DeserializeByType(aProperties, this, GetStaticType());
    mConstructing = false;
  }

  void InputInterpreter::Initialize()
  {
    mView = mSpace->GetComponent<GraphicsView>();
    mGamepad = mOwner->GetEngine()->GetGamepadSystem()->GetXboxController(YTE::ControllerId::Xbox_P1);
    mKeyboard = &mView->GetWindow()->mKeyboard;
    mContext = InputContext::Menu;

    mMenuSpace = mSpace->AddChildSpace("MSR_Menus");
    mHudSpace = mSpace->AddChildSpace("MSR_HUD");
    mCreditsSpace = mSpace->AddChildSpace("MSR_Credits");

    mSpace->RegisterEvent<&InputInterpreter::OnFrameUpdate>(Events::FrameUpdate, this);
    mSpace->RegisterEvent<&InputInterpreter::OnLogicUpdate>(Events::LogicUpdate, this);

    mGamepad->RegisterEvent<&InputInterpreter::OnStickEvent>(Events::XboxStickEvent, this);
    mGamepad->RegisterEvent<&InputInterpreter::OnFlickEvent>(Events::XboxStickFlicked, this);
    mGamepad->RegisterEvent<&InputInterpreter::OnButtonPress>(Events::XboxButtonPress, this);
    mGamepad->RegisterEvent<&InputInterpreter::OnButtonRelease>(Events::XboxButtonRelease, this);

    mKeyboard->RegisterEvent<&InputInterpreter::OnKeyPersist>(Events::KeyPersist, this);
    mKeyboard->RegisterEvent<&InputInterpreter::OnKeyPress>(Events::KeyPress, this);
    mKeyboard->RegisterEvent<&InputInterpreter::OnKeyRelease>(Events::KeyRelease, this);
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
  void InputInterpreter::OnPostStart(LogicUpdate*)
  {
    mDoneOnce = true;

    DirectCameraEvent setupMainMenuCam;

    if (auto camAnchor = mSpace->FindFirstCompositionByName("MainMenuCamAnchor"))
    {
      camAnchor->SendEvent(Events::DirectCameraEvent, &setupMainMenuCam);
    }

    MenuElementChange hoverFirstButton(MenuElementChange::Direction::Init);

    mMenuSpace->SendEvent(Events::MenuElementChange, &hoverFirstButton);

    mSpace->DeregisterEvent<&InputInterpreter::OnPostStart>(Events::LogicUpdate,  this);
  }

  void InputInterpreter::OnFrameUpdate(LogicUpdate *)
  {
    auto window = mView->GetWindow();
    if (window)
    {
      if (window->IsFocused())
      {
        window->SetCursorVisibility(false);
      }
      else
      {
        window->SetCursorVisibility(true);
      }
    }
  }

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

    else if (mContext == InputContext::Dialogue)
    {
      if (mIsRightTriggerDown && mGamepad->GetRightTrigger() < 0.1f)
      {
        mIsRightTriggerDown = false;
      }
      else if (!mIsRightTriggerDown && mGamepad->GetRightTrigger() > 0.5f)
      {
        DialogueSelect select(DialogueSelect::Direction::Next);
        mSpace->SendEvent(Events::DialogueSelect, &select);

        mIsRightTriggerDown = true;
      }

      if (mIsLeftTriggerDown && mGamepad->GetLeftTrigger() < 0.1f)
      {
        mIsLeftTriggerDown = false;
      }
      else if (!mIsLeftTriggerDown && mGamepad->GetLeftTrigger() > 0.5f)
      {
        DialogueSelect select(DialogueSelect::Direction::Prev);
        mSpace->SendEvent(Events::DialogueSelect, &select);

        mIsLeftTriggerDown = true;
      }
    }

    if (!mDoneOnce)
    {
      mSpace->RegisterEvent<&InputInterpreter::OnPostStart>(Events::LogicUpdate, this);
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

    else if (mContext == InputContext::Options)
    {
      if (aEvent->Stick == XboxButtons::LeftStick)
      {
        OptionsStickEvent stickEvent(aEvent->StickDirection);
        mMenuSpace->SendEvent(Events::OptionsStickEvent, &stickEvent);
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

    else if (mContext == InputContext::Options)
    {
      if (aEvent->FlickedStick == XboxButtons::LeftStick)
      {
        glm::vec2 flickDir = aEvent->FlickDirection;

        // Check dead-zone
        if (glm::length(flickDir) > 0.1f)
        {
            // Check forward region
          if (flickDir.x > 0.0f)
          {
            OptionsFlickEvent stickEvent(glm::vec2(1.0f, 0.0f));
            mMenuSpace->SendEvent(Events::OptionsFlickEvent, &stickEvent);
          }

          else
          {
            OptionsFlickEvent stickEvent(glm::vec2(-1.0f, 0.0f));
            mMenuSpace->SendEvent(Events::OptionsFlickEvent, &stickEvent);
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
          {
            HudElementToggled postcard(HudElementToggled::Element::Postcard);
            mHudSpace->SendEvent(Events::HudElementToggled, &postcard);
            break;
          }
          case XboxButtons::DPAD_Left:
          {
            HudElementToggled map(HudElementToggled::Element::Map);
            mHudSpace->SendEvent(Events::HudElementToggled, &map);
            break;
          }
          case XboxButtons::DPAD_Right:
          {
            HudElementToggled compass(HudElementToggled::Element::Compass);
            mHudSpace->SendEvent(Events::HudElementToggled, &compass);
            break;
          }
          case XboxButtons::A:
          {
            RequestDialogueStart dock;
            mOwner->SendEvent(Events::RequestDialogueStart, &dock);
            RequestNoticeBoardStart notice;
            mOwner->SendEvent(Events::RequestNoticeBoardStart, &notice);
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
            SailStateChanged sailUp(false);
            mSpace->SendEvent(Events::SailStateChanged, &sailUp);

            MenuStart menuStart;
            menuStart.PlaySound = true;
            menuStart.ContextSwitcher = this;
            mMenuSpace->SendEvent(Events::MenuStart, &menuStart);
            mHudSpace->SendEvent(Events::MenuStart, &menuStart);

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
            mMenuSpace->SendEvent(Events::MenuExit, &menuExit);
            mHudSpace->SendEvent(Events::MenuExit, &menuExit);

            //mContext = InputContext::Sailing;
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
            mMenuSpace->SendEvent(Events::MenuExit, &menuExit);
            mHudSpace->SendEvent(Events::MenuExit, &menuExit);

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

      case InputContext::Options:
      {
        switch (aEvent->Button)
        {
          case XboxButtons::A:
          {
            OptionsConfirmEvent confirm;
            mMenuSpace->SendEvent(Events::OptionsConfirmEvent, &confirm);

            break;
          }
        }
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
        switch (aEvent->Key)
        {
          case Keys::Return:
          {
            DialogueConfirm diagConfirm;
            mOwner->SendEvent(Events::DialogueConfirm, &diagConfirm);
            break;
          }

          case Keys::D:
          case Keys::Right:
          {
            DialogueSelect select(DialogueSelect::Direction::Next);
            mSpace->SendEvent(Events::DialogueSelect, &select);

            mIsRightTriggerDown = true;

            break;
          }

          case Keys::A:
          case Keys::Left:
          {
            DialogueSelect select(DialogueSelect::Direction::Prev);
            mSpace->SendEvent(Events::DialogueSelect, &select);

            mIsRightTriggerDown = true;

            break;
          }
        }
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
            menuStart.ContextSwitcher = this;
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

      case InputContext::Options:
      {
        switch (aEvent->Key)
        {
          case Keys::Return:
          {
            OptionsConfirmEvent confirm;
            mMenuSpace->SendEvent(Events::OptionsConfirmEvent, &confirm);

            break;
          }

          case Keys::A:
          case Keys::Left:
          {
            OptionsStickEvent stickEvent(glm::vec2(-1.0f, 0.0f));
            mMenuSpace->SendEvent(Events::OptionsStickEvent, &stickEvent);

            OptionsFlickEvent flickEvent(glm::vec2(-1.0f, 0.0f));
            mMenuSpace->SendEvent(Events::OptionsFlickEvent, &flickEvent);

            break;
          }

          case Keys::D:
          case Keys::Right:
          {
            OptionsStickEvent stickEvent(glm::vec2(1.0f, 0.0f));
            mMenuSpace->SendEvent(Events::OptionsStickEvent, &stickEvent);

            OptionsFlickEvent flickEvent(glm::vec2(1.0f, 0.0f));
            mMenuSpace->SendEvent(Events::OptionsFlickEvent, &flickEvent);

            break;
          }
        }

        break;
      }
    }

    switch (aEvent->Key)
    {
      case Keys::M:
      {
        MuteBypass mute;
        mMenuSpace->SendEvent(Events::MuteBypass, &mute);

        break;
      }

      case Keys::F3:
      {
        auto physicsSys = mSpace->GetComponent<PhysicsSystem>();

        if (physicsSys)
        {
          physicsSys->ToggleDebugDraw();
        }

        break;
      }
      case Keys::F4:
      {
        mSpace->GetComponent<GraphicsView>()->GetWindow()->SetResolution(2160,1440);
        break;
      }
      case Keys::F5:
      {
        mSpace->GetComponent<GraphicsView>()->GetWindow()->SetResolution(1920, 1080);
        break;
      }
      case Keys::F6:
      {
        mSpace->GetComponent<GraphicsView>()->GetWindow()->SetResolution(1280, 720);
        break;
      }
      case Keys::F7:
      {
        mSpace->GetComponent<GraphicsView>()->GetWindow()->SetResolution(1152, 648);
        break;
      }
      case Keys::F11:
      {
        auto fullscreen = mSpace->GetComponent<GraphicsView>()->GetWindow()->mFullscreen;
        mSpace->GetComponent<GraphicsView>()->GetWindow()->SetFullscreen(!fullscreen, false);
        break;
      }
      // hotkey for triggering ending
      case Keys::F9:
      {
        DebugSwitch setDebug(true);
        mOwner->SendEvent(Events::DebugSwitch, &setDebug);

        if (auto noticeBoard = mOwner->FindFirstCompositionByName("noticeboard"); noticeBoard)
        {
          if (NoticeBoard *noticeCmp = noticeBoard->GetComponent<NoticeBoard>(); noticeCmp)
          {
            noticeCmp->TriggerEndingSequence();
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

      case InputContext::Options:
      {
        switch (aEvent->Key)
        {
          case Keys::A:
          case Keys::Left:
          case Keys::D:
          case Keys::Right:
          {
            OptionsStickEvent stickEvent(glm::vec2(0.0f, 0.0f));
            mMenuSpace->SendEvent(Events::OptionsStickEvent, &stickEvent);

            break;
          }
        }
      }
    }
  }
}
