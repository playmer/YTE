#include "YTE/Platform/Gamepad.hpp"

namespace YTE
{
  YTEDefineEvent(GamepadStickFlicked);
  YTEDefineEvent(GamepadStickEvent);
  YTEDefineEvent(GamepadButtonPress);
  YTEDefineEvent(GamepadButtonRelease);
  YTEDefineEvent(GamepadButtonPersist);

  YTEDefineType(GamepadFlickEvent)
  {
    RegisterType<GamepadFlickEvent>();
    TypeBuilder<GamepadFlickEvent> builder;
    builder.Field<&GamepadFlickEvent::FlickDirection>("FlickDirection", PropertyBinding::Get);
    builder.Field<&GamepadFlickEvent::FlickedStick>("FlickedStick", PropertyBinding::Get);
    builder.Field<&GamepadFlickEvent::Controller>("Controller", PropertyBinding::Get);
  }
  
  YTEDefineType(GamepadStickEvent)
  {
    RegisterType<GamepadStickEvent>();
    TypeBuilder<GamepadStickEvent> builder;
    builder.Field<&GamepadStickEvent::StickDirection>("StickDirection", PropertyBinding::Get);
    builder.Field<&GamepadStickEvent::Stick>("Stick", PropertyBinding::Get);
    builder.Field<&GamepadStickEvent::Controller>("Controller", PropertyBinding::Get);
  }

  YTEDefineType(GamepadButtonEvent)
  {
    RegisterType<GamepadButtonEvent>();
    TypeBuilder<GamepadButtonEvent> builder;
    builder.Field<&GamepadButtonEvent::Button>("Button", PropertyBinding::GetSet);
    builder.Field<&GamepadButtonEvent::Controller>("Controller", PropertyBinding::GetSet);
  }


  YTEDefineType(Gamepad)
  {
    RegisterType<Gamepad>();
    TypeBuilder<Gamepad> builder;
    builder.Property<&Gamepad::GetId, NoSetter>("Id");
    builder.Property<&Gamepad::GetLeftStick, NoSetter>("LeftStick");
    builder.Property<&Gamepad::GetRightStick, NoSetter>("RightStick");
    builder.Property<&Gamepad::GetLeftTrigger, NoSetter>("LeftTrigger");
    builder.Property<&Gamepad::GetRightTrigger, NoSetter>("RightTrigger");
    builder.Property<&Gamepad::Active, NoSetter>("Active");
    
    builder.Function<&Gamepad::IsButtonDown>("IsButtonDown")
      .SetParameterNames("aButton")
      .SetDocumentation("Finds if the given button is pressed right now.");
    builder.Function<&Gamepad::WasButtonDown>("WasButtonDown")
      .SetParameterNames("aButton")
      .SetDocumentation("Finds if the given button is pressed last frame.");
    builder.Function<&Gamepad::Vibrate>("Vibrate")
      .SetParameterNames("aVibration")
      .SetDocumentation("Sets the controllers motors to vibrate via the specified amounts.");
    builder.Function<&Gamepad::VibrateForTime>("VibrateForTime")
      .SetParameterNames("aVibration", "aTime")
      .SetDocumentation("The controller will vibrate for the given amount of time. If called again, will choose the higher vibration.");
  }

  ///////////////////////////////////////
  // Public Functions
  ///////////////////////////////////////
  //////////////////////////////
  // Constructors
  //////////////////////////////
  Gamepad::Gamepad()
  {
    mCurrentButtons.fill(false);
    mPreviousButtons.fill(false);

    mLeftStickFlicked = false;
    mRightStickFlicked = false;
    mVibration = 0.0f;
  }


  Gamepad::~Gamepad()
  {
    Vibrate(0.0f);
  }

  void Gamepad::PreUpdate()
  {
    mPreviousLeftStick = mLeftStick;
    mPreviousRightStick = mRightStick;
    mPreviousButtons = mCurrentButtons;
  }

  void Gamepad::Update(double aDt)
  {
    OPTICK_EVENT();

    GamepadButtonEvent event;
    constexpr auto gamepadButtons = EnumCast(GamepadButtons::Gamepad_Buttons_Number);

    for (size_t i = 0; i < gamepadButtons; ++i)
    {
      const auto currentState = mCurrentButtons[i];
      const auto previousState = mPreviousButtons[i];

      event.Button = static_cast<GamepadButtons>(i);
      event.Controller = this;

      /* Key has been persisted */
      if (currentState && previousState)
      {
        SendEvent(Events::GamepadButtonPersist, &event);
      }
      /* Key has been pressed */
      else if (currentState)
      {
        SendEvent(Events::GamepadButtonPress, &event);
      }
      /* Key has been released */
      else if (!currentState && previousState)
      {
        SendEvent(Events::GamepadButtonRelease, &event);
      }
    }

    // Evelyn, the genius gave the great point that comparing these to the prev stick value would 
    // function as a true stick change (and could save some event constructions in the case
    // of a stick being held in the same position that is not zero)
    {
      GamepadStickEvent stickEvent;
      stickEvent.Controller = this;

      auto sendStickEvent = [this, &stickEvent](glm::vec2 aStick, GamepadButtons aStickButton)
      {
        stickEvent.StickDirection = aStick;
        stickEvent.Stick = aStickButton;
        SendEvent(Events::GamepadStickEvent, &stickEvent);
      };

      // Left Stick not zero
      if (mLeftStick.x != 0.0f || mLeftStick.y != 0.0f)
      {
        sendStickEvent(mLeftStick, GamepadButtons::LeftStick);
      }
      // Right Stick not zero
      if (mRightStick.x != 0.0f || mRightStick.y != 0.0f)
      {
        sendStickEvent(mRightStick, GamepadButtons::RightStick);
      }
      // Left Stick recentered, this allows gameplay to depend on a stick returning to zero
      if (mLeftStick.x == 0.0f && mLeftStick.y == 0.0f && mPreviousLeftStick.x != 0.0f && mPreviousLeftStick.y != 0.0f)
      {
        sendStickEvent(mLeftStick, GamepadButtons::LeftStick);
      }
      // Right Stick recentered, this allows gameplay to depend on a stick returning to zero
      if (mRightStick.x == 0.0f && mRightStick.y == 0.0f && mPreviousRightStick.x != 0.0f && mPreviousRightStick.y != 0.0f)
      {
        sendStickEvent(mRightStick, GamepadButtons::RightStick);
      }
    }

    //Stick flicking
    {
      GamepadFlickEvent flickEvent;
      flickEvent.Controller = this;
      
      auto sendStickEvent = [this, &flickEvent](glm::vec2 aStick, GamepadButtons aStickButton)
      {
        flickEvent.FlickDirection = glm::normalize(aStick);
        flickEvent.FlickedStick = aStickButton;
        SendEvent(Events::GamepadStickFlicked, &flickEvent);
      };

      bool leftStickFlicked = (glm::length(mLeftStick) >= cFlickMagnitude);
      bool rightStickFlicked = (glm::length(mRightStick) >= cFlickMagnitude);

      if (leftStickFlicked && !mLeftStickFlicked)
      {
        sendStickEvent(mLeftStick, GamepadButtons::LeftStick);
      }

      if (rightStickFlicked && !mRightStickFlicked)
      {
        sendStickEvent(mRightStick, GamepadButtons::RightStick);
      }

      mLeftStickFlicked = leftStickFlicked;
      mRightStickFlicked = rightStickFlicked;
    }

    float vibration = 0.0f;

    for (auto &vibe : mVibrations)
    {
      if (vibe.mVibration > vibration)
      {
        vibration = vibe.mVibration;
      }

      vibe.mTime -= aDt;
    }

    if (mVibrations.size() > 0 && (vibration != mVibration))
    {
      mVibration = vibration;
      Vibrate(vibration);
    }
    else if ((mVibration != 0.0f) && (mVibrations.size() == 0))
    {
      mVibration = 0.0f;
      Vibrate(0.0f);
    }

    auto iterator = std::remove_if(mVibrations.begin(), mVibrations.end(),
                                   [](const Vibration &aVibe) { return aVibe.mTime < 0.0f; });

    mVibrations.erase(iterator, mVibrations.end());
  }

  //////////////////////////////
  // Getters/Setters
  //////////////////////////////
  bool Gamepad::IsButtonDown(GamepadButtons aButton)
  {
    return mCurrentButtons[static_cast<size_t>(aButton)];
  }
  bool Gamepad::WasButtonDown(GamepadButtons aButton)
  {
    return mPreviousButtons[static_cast<size_t>(aButton)];
  }
}
