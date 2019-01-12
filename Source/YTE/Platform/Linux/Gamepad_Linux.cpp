/******************************************************************************/
/*!
 * \author Joshua T. Fisher
 * \date   6/7/2015
 *
 * \copyright All content 2016 DigiPen (USA) Corporation, all rights reserved.
 */
/******************************************************************************/
#include "YTE/Platform/TargetDefinitions.hpp"

#include "YTE/Platform/Gamepad.hpp"
#include "YTE/Platform/DeviceEnums.hpp"

namespace YTE
{
  inline glm::vec2 GetStickState(float aX, float aY, bool aLeft)
  {
    return {};
  }


  void XboxController::ProcessButton(void *aState, size_t aOsButton, XboxButtons aOurKey)
  {
  }


  void XboxController::UpdateState(XboxControllerState *aState, double aDt)
  {
  }

  void XboxController::Vibrate(float aLeftSpeed, float aRightSpeed)
  {
  }
}