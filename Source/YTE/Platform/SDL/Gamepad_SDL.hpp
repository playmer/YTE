#pragma once

#include "SDL.h"

#include "YTE/Platform/Mouse.hpp"
#include "YTE/Platform/ForwardDeclarations.hpp"
#include "YTE/Platform/PlatformManager.hpp"

namespace YTE
{
  struct GamepadData
  {
    static void ControllerEvent(SDL_JoyDeviceEvent aEvent, GamepadSystem* aSystem);

    GamepadData(Gamepad* mGamepad, SDL_GameController* aController, char const* aName, int aInstanceId);
    ~GamepadData();
    
    void ControllerMotionEvent(SDL_ControllerAxisEvent& aEvent);
    void ControllerButtonEvent(SDL_ControllerButtonEvent& aEvent);

    Gamepad* mGamepad = nullptr;
    SDL_GameController* mController = nullptr;
    SDL_Joystick* mJoystick = nullptr;
    SDL_Haptic* mHapticDevice = nullptr;
    char const* mName = nullptr;
    int mInstanceId = -1;
    bool mRemovedPreviously = false;
  };
}