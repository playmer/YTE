/******************************************************************************/
/*!
 * \author Joshua T. Fisher
 * \date   2015-6-7
 *
 * \copyright All content 2016 DigiPen (USA) Corporation, all rights reserved.
 */
/******************************************************************************/
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
      
    GamepadSystem();

    void Update(double aDt);
    void CheckForControllers(bool aChecking) { mChecking = aChecking; };
    XboxController* GetXboxController(Controller_Id aId);

  private:
    void UpdateXboxControllers(double aDt);

    std::array<std::pair<XboxController, double>, 4> mXboxControllers;
    bool mChecking;

  };
}

#endif