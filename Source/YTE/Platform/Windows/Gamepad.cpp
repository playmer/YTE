/******************************************************************************/
/*!
 * \author Joshua T. Fisher
 * \date   6/7/2015
 *
 * \copyright All content 2016 DigiPen (USA) Corporation, all rights reserved.
 */
/******************************************************************************/
#include "YTE/Platform/TargetDefinitions.h"

#ifdef Windows

#include <YTE/Platform/Windows/WindowsInclude.hpp>
#include <XInput.h>
#include <algorithm>

#include "YTE/Event/StandardEvents.h"

#include "YTE/Platform/Gamepad.h"
#include "YTE/Platform/DeviceEnums.h"

#undef max
#undef min
namespace YTE
{
  inline glm::vec2 GetStickState(float aX, float aY, bool aLeft)
  {
    float deadzone;
      
    if (aLeft)
    {
      deadzone = static_cast<float>(XINPUT_GAMEPAD_LEFT_THUMB_DEADZONE);
    }
    else
    {
      deadzone = static_cast<float>(XINPUT_GAMEPAD_RIGHT_THUMB_DEADZONE);
    }

    if (sqrt(aX*aX + aY*aY) < deadzone)
    {
      aX = 0.0f;
      aY = 0.0f;
    }

      //Normalize stick values
      //There is 1 more negative value in a unsigned short
      //than positive so clamp to -1 to 1
    aX = std::max(-1.0f, std::min(aX / (float)SHRT_MAX, 1.0f));
    aY = std::max(-1.0f, std::min(aY / (float)SHRT_MAX, 1.0f));

    return glm::vec2(aX, aY);
  }

  #define ProcessButton(aOsButton, aOurKey)                             \
    index = static_cast<size_t>(Xbox_Buttons::aOurKey);                 \
                                                                        \
    /* Key got resent. */                                               \
                                                                        \
    if (state->Gamepad.wButtons & aOsButton)                            \
    {                                                                   \
      down  = true;                                                     \
    }                                                                   \
    else                                                                \
    {                                                                   \
      down = false;                                                     \
    }                                                                   \
                                                                        \
    mPreviousButtons[index] = mCurrentButtons[index];                   \
    mCurrentButtons[index] = down;                                      \
                                                                        \
    xboxEvent.Button = Xbox_Buttons::aOurKey;                           \
    xboxEvent.Controller = this;                                        \
                                                                        \
    /* Key has been persisted */                                        \
    if (mCurrentButtons[index] && mPreviousButtons[index])              \
    {                                                                   \
      eventState = &Events::XboxButtonPersist;                          \
      Trigger(*eventState, &xboxEvent);                                 \
    }                                                                   \
    /* Key has been pressed */                                          \
    else if (down)                                                      \
    {                                                                   \
      eventState = &Events::XboxButtonPress;                            \
      Trigger(*eventState, &xboxEvent);                                 \
    }                                                                   \
    /* Key has been released */                                         \
    else if (down == false && mPreviousButtons[index] == true)          \
    {                                                                   \
      eventState = &Events::XboxButtonRelease;                          \
      Trigger(*eventState, &xboxEvent);                                 \
    }

  void XboxController::UpdateState(XboxControllerState *aState, float aDt)
  {
    if (aState == nullptr)
    {
      mActive = false;
      return;
    }
    else
    {
      mActive = true;
    }

    XINPUT_STATE *state = reinterpret_cast<XINPUT_STATE *>(aState);
      
      // Some variables for the macroexpansion.
    const std::string *eventState;
    XboxButtonEvent xboxEvent;
    size_t index;
    bool down;

    // TODO (Josh): Make some way to do this contiguously instead of using the TODO trick.
      // Include trick for updating button states.
    #include "OsXboxButtons.hpp"
    #undef ProcessButton

    mLeftStick = GetStickState(state->Gamepad.sThumbLX, state->Gamepad.sThumbLY, true);
    mRightStick = GetStickState(state->Gamepad.sThumbRX, state->Gamepad.sThumbRY, false);

    //Stick flicking
    bool leftStickFlicked = (glm::length(mLeftStick) >= cFlickMagnitude);
    bool rightStickFlicked = (glm::length(mRightStick) >= cFlickMagnitude);

    if (leftStickFlicked && !mLeftStickFlicked)
    {
      XboxFlickEvent flickEvent;
      flickEvent.FlickedStick = Xbox_Buttons::LeftStick;
      flickEvent.FlickDirection = glm::normalize(mLeftStick);
      flickEvent.Controller = this;
      Trigger(Events::XboxStickFlicked, &flickEvent);
    }

    if (rightStickFlicked && !mRightStickFlicked)
    {
      XboxFlickEvent flickEvent;
      flickEvent.FlickedStick = Xbox_Buttons::RightStick;
      flickEvent.FlickDirection = glm::normalize(mRightStick);
      flickEvent.Controller = this;
      Trigger(Events::XboxStickFlicked, &flickEvent);
    }

    mLeftStickFlicked = leftStickFlicked;
    mRightStickFlicked = rightStickFlicked;


    mLeftTrigger = state->Gamepad.bLeftTrigger / 255.0f;
    mRightTrigger = state->Gamepad.bRightTrigger / 255.0f;

    float rightVibe = 0.0f;
    float leftVibe = 0.0f;

    for (auto &vibe : mVibrations)
    {
      if (vibe.mLeft > leftVibe)
      {
        leftVibe = vibe.mLeft;
      }

      if (vibe.mRight > rightVibe)
      {
        rightVibe = vibe.mRight;
      }

      vibe.mTime -= aDt;
    }

    if (mVibrations.size() > 0 && ((leftVibe != mLeftVibe) || (rightVibe != mRightVibe)))
    {
      mLeftVibe = leftVibe;
      mRightVibe = rightVibe;
      std::cout << "Vibrating: " << aDt << std::endl;
      Vibrate(leftVibe, rightVibe);
    }
    else if ((mLeftVibe != 0.0f || mRightVibe != 0.0f) && (mVibrations.size() == 0))
    {
      mLeftVibe = 0.0f;
      mRightVibe = 0.0f;
      std::cout << "No longer Vibrating." << std::endl;
      Vibrate(0.0f, 0.0f);
    }

    auto iterator = std::remove_if(mVibrations.begin(), mVibrations.end(),
                                    [](const Vibration &aVibe) { return aVibe.mTime < 0.0f; });

    mVibrations.erase(iterator, mVibrations.end());
  }

  void XboxController::Vibrate(float aLeftSpeed, float aRightSpeed)
  {
    XINPUT_VIBRATION zerovibration;
    zerovibration.wLeftMotorSpeed = (WORD)(aLeftSpeed * cMaxRumble);
    zerovibration.wRightMotorSpeed = (WORD)(aRightSpeed * cMaxRumble);
    XInputSetState(mGamepadIndex, &zerovibration);
  }

  void XboxController::VibrateForTime(float aLeftSpeed, float aRightSpeed, float aTime)
  {
    Vibration vibe;
    vibe.mLeft = aLeftSpeed;
    vibe.mRight = aRightSpeed;
    vibe.mTime = aTime;
    mVibrations.push_back(vibe);
  }
}

#endif