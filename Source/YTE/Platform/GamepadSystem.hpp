#pragma once

#ifndef YTE_Platform_GamepadSystem_h
#define YTE_Platform_GamepadSystem_h

#include <array>

#include "YTE/Platform/Gamepad.hpp"

namespace YTE
{
  class GamepadSystem
  {
  public:
    YTEDeclareType(GamepadSystem);
      
    YTE_Shared GamepadSystem();

    YTE_Shared void Update(double aDt);
    YTE_Shared void CheckForControllers(bool aChecking) { mChecking = aChecking; };
    YTE_Shared XboxController* GetXboxController(ControllerId aId);

  private:
    void UpdateXboxControllers(double aDt);

    std::array<std::pair<XboxController, double>, 4> mXboxControllers;
    bool mChecking;

  };
}

#endif