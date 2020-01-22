#pragma once

#include "SDL.h"

#include "YTE/Platform/Mouse.hpp"
#include "YTE/Platform/ForwardDeclarations.hpp"
#include "YTE/Platform/PlatformManager.hpp"

namespace YTE
{
  struct GamepadSystemData
  {
    void ControllerDeviceEvent(SDL_ControllerDeviceEvent aEvent, GamepadSystem* aSystem);
    void ControllerEvent(SDL_ControllerDeviceEvent aEvent, GamepadSystem* aSystem);
  };
}