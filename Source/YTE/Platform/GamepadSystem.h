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

#include "YTE/Platform/Gamepad.h"

namespace YTE
{
  class GamepadSystem
  {
  public:
    DeclareType(GamepadSystem);
      
    GamepadSystem();

    void Update(float aDt);
    void CheckForControllers(bool aChecking) { mChecking = aChecking; };
    XboxController* GetXboxController(Controller_Id aId);

  private:
    void UpdateXboxControllers(float aDt);

    std::array<std::pair<XboxController, float>, 4> mXboxControllers;
    bool mChecking;

  };
}

#endif