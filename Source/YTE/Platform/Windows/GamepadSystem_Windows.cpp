/******************************************************************************/
/*!
 * \author Joshua T. Fisher
 * \date   6/7/2015
 *
 * \copyright All content 2016 DigiPen (USA) Corporation, all rights reserved.
 */
/******************************************************************************/
#include "YTE/Platform/TargetDefinitions.hpp"

#ifdef YTE_Windows
#include "YTE/Platform/Windows/WindowsInclude_Windows.hpp"
#include <XInput.h>

#include "YTE/Platform/GamepadSystem.hpp"
namespace YTE
{
  inline void UpdateController(XboxController &aController, DWORD aIndex, double aDt)
  {
    XINPUT_STATE state;

    memset(&state, 0, sizeof(XINPUT_STATE));

    // Simply get the state of the controller from XInput.
    DWORD dwResult = XInputGetState(aIndex, &state);

    if (dwResult == ERROR_SUCCESS)
    {
      aController.UpdateState(reinterpret_cast<XboxControllerState*>(&state), aDt);
    }
    else
    {
      aController.UpdateState(nullptr, aDt);
    }
  }

  void GamepadSystem::UpdateXboxControllers(double aDt)
  {
    for (DWORD i = 0; i< XUSER_MAX_COUNT; i++)
    {
      mXboxControllers[i].second += aDt;

      if (mXboxControllers[i].first.Active())
      {
        UpdateController(mXboxControllers[i].first, i, aDt);
      }
        // Only check for non-active controllers every 4 seconds, 
        // if we're checking at all
      else if (mChecking && (mXboxControllers[i].second > 4.0f))
      {
        UpdateController(mXboxControllers[i].first, i, aDt);
        mXboxControllers[i].second = 0.0f;
      }
    }
  }
}

#endif