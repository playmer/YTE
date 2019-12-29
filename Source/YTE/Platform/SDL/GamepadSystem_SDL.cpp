#include "SDL.h"

#include "YTE/Platform/SDL/Gamepad_SDL.hpp"
#include "YTE/Platform/SDL/GamepadSystem_SDL.hpp"

#include "YTE/Platform/TargetDefinitions.hpp"

#include "YTE/Platform/GamepadSystem.hpp"
namespace YTE
{
  void GamepadSystem::PlatformInitialize()
  {
    mData.ConstructAndGet<GamepadSystemData>();
  }

  void GamepadSystemData::ControllerDeviceEvent(SDL_ControllerDeviceEvent aEvent, GamepadSystem* aSystem)
  {
    auto self = aSystem->mData.Get<GamepadSystemData>();

    auto& gamepads = aSystem->mGamepads;

    switch (aEvent.type)
    {
      case SDL_CONTROLLERDEVICEADDED:
      {
        auto pad = SDL_GameControllerOpen(aEvent.which);
          
        if (pad)
        {
          auto name = SDL_GameControllerName(pad);
          SDL_Joystick* joystick = SDL_GameControllerGetJoystick(pad);
          SDL_JoystickID instanceId = SDL_JoystickInstanceID(joystick);
          
          printf("Added (%s)\n", name);

          auto gamepadIt = gamepads.emplace(instanceId, Gamepad{});
          
          gamepadIt.first->second.mData.ConstructAndGet<GamepadData>(&gamepadIt.first->second, pad, name, instanceId);
        }

        break;
      }
      case SDL_CONTROLLERDEVICEREMOVED:
      {
        puts("Removed\n");
        if (auto it = gamepads.find(aEvent.which); 
            it != gamepads.end())
        {
          auto gamepadData = it->second.mData.Get<GamepadData>();

          if (gamepadData->mRemovedPreviously)
          {
            gamepads.erase(it);
          }
          else
          {
            gamepadData->mRemovedPreviously = true;
          }
        }

        break;
      }
      case SDL_CONTROLLERDEVICEREMAPPED:
      {
        //if (auto it = mGamepadSystem.mGamepads.find(event.cdevice.which);
        //    it != mGamepadSystem.mGamepads.end())
        //{
        //  auto data = it->second.mData.Get<GamepadData>();
        //
        //}
        puts("Remaped\n");
        break;
      }
    }
  }
}
