#pragma once

#ifndef YTE_Platform_GamepadSystem_h
#define YTE_Platform_GamepadSystem_h

#include <array>

#include "YTE/Platform/ForwardDeclarations.hpp"
#include "YTE/Platform/Gamepad.hpp"

#include "YTE/StandardLibrary/PrivateImplementation.hpp"

namespace YTE
{
  struct GamepadSystemData;
  class PlatformManager;

  class GamepadSystem
  {
  public:
    YTEDeclareType(GamepadSystem);
      
    YTE_Shared GamepadSystem();

    YTE_Shared Gamepad* GetGamepad(i64 aId);

  private:
    friend GamepadSystemData;
    friend PlatformManager;

    void PlatformInitialize();
    void UpdateGamepads(double aDt);
    // Call PreUpdate before pumping the event queue.
    void PreUpdateGamepads();

    std::unordered_map<i64, Gamepad> mGamepads;
    PrivateImplementationLocal<32> mData;
  };
}

#endif