#include "YTE/Platform/TargetDefinitions.hpp"

#include "YTE/Platform/Gamepad.hpp"
#include "YTE/Platform/SDL/Gamepad_SDL.hpp"
#include "YTE/Platform/DeviceEnums.hpp"

namespace YTE
{
  GamepadButtons TranslateFromOsGamepadButtonToOurs(uint64_t aOsButton)
  {
    switch (aOsButton)
    {
      #define ProcessButtonMacro(aOsButton, aOurButton) case (aOsButton) : return aOurButton;
        #include "YTE/Platform/SDL/OsGamepadButtons_SDL.hpp"
      #undef ProcessButtonMacro

      default: return GamepadButtons::Unknown;
    }
  }

  uint64_t TranslateFromOurToOSGamepadButton(GamepadButtons aOurButton)
  {
    switch (aOurButton)
    {
      #define ProcessButtonMacro(aOsButton, aOurButton) case (aOurButton) : return aOsButton;
        #include "YTE/Platform/SDL/OsGamepadButtons_SDL.hpp"
      #undef ProcessButtonMacro

      default: return static_cast<uint64_t>(SDL_GameControllerButton::SDL_CONTROLLER_BUTTON_INVALID);
    }
  }

  void Gamepad::Vibrate(float aVibration)
  {
    auto self = mData.Get<GamepadData>();

    if (self)
    {
      SDL_HapticRumblePlay(self->mHapticDevice, aVibration, 500);
    }
  }

  void Gamepad::PlatformUpdate()
  {
    auto self = mData.Get<GamepadData>();

    if (nullptr == self)
    {
      return;
    }

    auto controller = self->mController;
    auto processButton = [&](GamepadButtons aOurButton, SDL_GameControllerButton aSdlButton) { 
      auto index = EnumCast(aOurButton);
      mPreviousButtons[index] = mCurrentButtons[index];
      mCurrentButtons[index] = SDL_GameControllerGetButton(controller, aSdlButton); 
    };
    
    processButton(GamepadButtons::DPAD_Up, SDL_CONTROLLER_BUTTON_DPAD_UP);
    processButton(GamepadButtons::DPAD_Down, SDL_CONTROLLER_BUTTON_DPAD_DOWN);
    processButton(GamepadButtons::DPAD_Left, SDL_CONTROLLER_BUTTON_DPAD_LEFT);
    processButton(GamepadButtons::DPAD_Right, SDL_CONTROLLER_BUTTON_DPAD_RIGHT);
    processButton(GamepadButtons::Guide, SDL_CONTROLLER_BUTTON_GUIDE);
    processButton(GamepadButtons::Start, SDL_CONTROLLER_BUTTON_START);
    processButton(GamepadButtons::Back, SDL_CONTROLLER_BUTTON_BACK);
    processButton(GamepadButtons::LeftStick, SDL_CONTROLLER_BUTTON_LEFTSTICK);
    processButton(GamepadButtons::RightStick, SDL_CONTROLLER_BUTTON_RIGHTSTICK);
    processButton(GamepadButtons::LeftShoulder, SDL_CONTROLLER_BUTTON_LEFTSHOULDER);
    processButton(GamepadButtons::RightShoulder, SDL_CONTROLLER_BUTTON_RIGHTSHOULDER);
    processButton(GamepadButtons::A, SDL_CONTROLLER_BUTTON_A);
    processButton(GamepadButtons::B, SDL_CONTROLLER_BUTTON_B);
    processButton(GamepadButtons::X, SDL_CONTROLLER_BUTTON_X);
    processButton(GamepadButtons::Y, SDL_CONTROLLER_BUTTON_Y);

    glm::ivec2 leftStick;
    leftStick.x = SDL_GameControllerGetAxis(controller, SDL_CONTROLLER_AXIS_LEFTX);
    leftStick.y = SDL_GameControllerGetAxis(controller, SDL_CONTROLLER_AXIS_LEFTY);

    glm::ivec2 rightStick;
    rightStick.x = SDL_GameControllerGetAxis(controller, SDL_CONTROLLER_AXIS_RIGHTX);
    rightStick.y = SDL_GameControllerGetAxis(controller, SDL_CONTROLLER_AXIS_RIGHTY);
  }

  void Gamepad::VibrateForTime(float aVibration, double aTime)
  {
    auto self = mData.Get<GamepadData>();
    auto milliseconds = static_cast<u32>(aTime * 1000);

    SDL_HapticRumblePlay(self->mHapticDevice, aVibration, milliseconds);
  }

  GamepadData::GamepadData(Gamepad* aGamepad, 
                           SDL_GameController* aController,
                           char const* aName,
                           int aInstanceId)
    : mGamepad{ aGamepad }
    , mController{ aController }
    , mJoystick{ SDL_GameControllerGetJoystick(mController) }
    , mName{ aName }
    , mInstanceId{ aInstanceId }
  {
    mHapticDevice = SDL_HapticOpenFromJoystick(mJoystick);
  }

  GamepadData::~GamepadData()
  {
    SDL_GameControllerClose(mController);
    SDL_HapticClose(mHapticDevice);
  }

  static float ToFloat(Sint16 aValue)
  {
    return (static_cast<float>(aValue + 32768.f) / 65535.f);
  }

  void GamepadData::ControllerMotionEvent(SDL_ControllerAxisEvent& aEvent)
  {
    // Note that on the Y axis we flip the result. We want moving the stick "up" to be positive Y, SDL reports it as negative,
    // so this resolves that.
    switch (aEvent.axis)
    {
      case SDL_CONTROLLER_AXIS_LEFTX: mGamepad->mLeftStick.x = 2.f * (ToFloat(aEvent.value) - .5f); return;
      case SDL_CONTROLLER_AXIS_LEFTY: mGamepad->mLeftStick.y = -2.f * (ToFloat(aEvent.value) - .5f); return;
      case SDL_CONTROLLER_AXIS_RIGHTX: mGamepad->mRightStick.x = 2.f * (ToFloat(aEvent.value) - .5f); return;
      case SDL_CONTROLLER_AXIS_RIGHTY: mGamepad->mRightStick.y = -2.f * (ToFloat(aEvent.value) - .5f); return;
      case SDL_CONTROLLER_AXIS_TRIGGERLEFT: mGamepad->mLeftTrigger = 2.f * (ToFloat(aEvent.value) - .5f); return;
      case SDL_CONTROLLER_AXIS_TRIGGERRIGHT: mGamepad->mRightTrigger = 2.f * (ToFloat(aEvent.value) - .5f); return;
    }
  }

  void GamepadData::ControllerButtonEvent(SDL_ControllerButtonEvent& aEvent)
  {
    mGamepad->mCurrentButtons[EnumCast(TranslateFromOsGamepadButtonToOurs(aEvent.button))] = aEvent.state == SDL_PRESSED;
  }
}