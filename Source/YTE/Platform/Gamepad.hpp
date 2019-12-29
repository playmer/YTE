#pragma once

#ifndef YTE_Platform_Gamepad_h
#define YTE_Platform_Gamepad_h

#include "YTE/Core/EventHandler.hpp"

#include "YTE/Platform/DeviceEnums.hpp"
#include "YTE/Platform/ForwardDeclarations.hpp"

#include "YTE/StandardLibrary/PrivateImplementation.hpp"

namespace YTE
{
  YTEDeclareEvent(GamepadStickFlicked);
  YTEDeclareEvent(GamepadStickEvent);
  YTEDeclareEvent(GamepadButtonPress);
  YTEDeclareEvent(GamepadButtonRelease);
  YTEDeclareEvent(GamepadButtonPersist);

  class Gamepad;
  class GamepadFlickEvent : public Event
  {
  public:
    YTEDeclareType(GamepadFlickEvent);

    glm::vec2 FlickDirection;
    GamepadButtons FlickedStick;
    Gamepad* Controller;
  };

  class GamepadStickEvent : public Event
  {
  public:
    YTEDeclareType(GamepadStickEvent);

    glm::vec2 StickDirection;
    GamepadButtons Stick;
    Gamepad* Controller;
  };

  class GamepadButtonEvent : public Event
  {
  public:
    YTEDeclareType(GamepadButtonEvent);

    GamepadButtons Button;
    Gamepad* Controller;
  };

  struct Vibration
  {
    float mVibration;
    double mTime;
  };

  
  uint64_t TranslateFromOurToOSGamepadButton(GamepadButtons aOurButton);
  GamepadButtons TranslateFromOsGamepadButtonToOurs(uint64_t aOsButton);


  class Gamepad : public EventHandler
  {
  public:
    YTEDeclareType(Gamepad);

    YTE_Shared Gamepad();
    YTE_Shared ~Gamepad();

    // aVibration: The amount to vibrate, a float between 0 and 1.
    YTE_Shared void Vibrate(float aVibration);

    // aVibration: The amount to vibrate, a float between 0 and 1.
    // aTime: A number of seconds.
    YTE_Shared void VibrateForTime(float aVibration, double aTime);

    YTE_Shared bool IsButtonDown(GamepadButtons aButton);
    YTE_Shared bool WasButtonDown(GamepadButtons aButton);
    inline bool Active() { return mActive; };
    inline glm::vec2 GetLeftStick() { return mLeftStick; };
    inline glm::vec2 GetRightStick() { return mRightStick; };
    inline float GetLeftTrigger() { return mLeftTrigger; };
    inline float GetRightTrigger() { return mRightTrigger; };
    inline float GetId() { return mGamepadIndex; };

    uint8_t mGamepadIndex;
  private:
    friend GamepadSystem;
    friend GamepadSystemData;
    friend GamepadData;
    friend PlatformManager;

    void PlatformUpdate();

    // Call PreUpdate before pumping the event queue.
    void PreUpdate();
    
    // Call Update after pumping the event queue.
    void Update(double aDt);
    
    
    Gamepad& operator=(Gamepad&);

    // true means the button is/was down.
    std::array<bool, EnumCast(GamepadButtons::Gamepad_Buttons_Number)> mPreviousButtons;
    std::array<bool, EnumCast(GamepadButtons::Gamepad_Buttons_Number)> mCurrentButtons;

    PrivateImplementationLocal<56> mData;

    std::vector<Vibration> mVibrations;

    glm::vec2 mLeftStick;
    glm::vec2 mRightStick;
    glm::vec2 mPreviousLeftStick;
    glm::vec2 mPreviousRightStick;
    
    float mVibration;

    float mLeftTrigger;
    float mRightTrigger;

    bool mLeftStickFlicked;
    bool mRightStickFlicked;

    bool mActive;

    const float cFlickMagnitude = 0.8f;
    const float cMaxRumble = 65535.0f;
  };
}

#endif