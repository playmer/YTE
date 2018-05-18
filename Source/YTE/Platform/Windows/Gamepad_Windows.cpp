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
#include <algorithm>



#include "YTE/Platform/Gamepad.hpp"
#include "YTE/Platform/DeviceEnums.hpp"

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


    void XboxController::ProcessButton(void *aState, size_t aOsButton, XboxButtons aOurKey)
    {
      XINPUT_STATE *state = static_cast<XINPUT_STATE*>(aState);
      XboxButtonEvent xboxEvent;
      size_t index = EnumCast(aOurKey);
      bool down;
                                                            
      /* Key got resent. */                                     
                                                            
      if (state->Gamepad.wButtons & aOsButton)
      {                                                         
        down  = true;                                           
      }                                                         
      else                                                      
      {                                                         
        down = false;                                           
      }                                                         
                                                            
      mPreviousButtons[index] = mCurrentButtons[index];         
      mCurrentButtons[index] = down;                            
                                                            
      xboxEvent.Button = aOurKey;                 
      xboxEvent.Controller = this;                              
                                                            
      /* Key has been persisted */                              
      if (mCurrentButtons[index] && mPreviousButtons[index])    
      {
        SendEvent(Events::XboxButtonPersist, &xboxEvent);
      }                                                         
      /* Key has been pressed */                                
      else if (down)                                            
      {                                                         
        SendEvent(Events::XboxButtonPress, &xboxEvent);
      }                                                         
      /* Key has been released */                               
      else if (down == false && mPreviousButtons[index] == true)
      { 
        SendEvent(Events::XboxButtonRelease, &xboxEvent);
      }
    }


  void XboxController::UpdateState(XboxControllerState *aState, double aDt)
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
      
    // TODO (Josh): Make some way to do this contiguously instead of using the TODO trick.
    // Include trick for updating button states.
    #define ProcessButtonMacro(aOsKey, aOurKey) ProcessButton(state, aOsKey, aOurKey);
    #include "YTE/Platform/Windows/OsXboxButtons_Windows.hpp"
    #undef ProcessButtonMacro

    mPreviousLeftStick = mLeftStick;
    mPreviousRightStick = mRightStick;
    mLeftStick = GetStickState(state->Gamepad.sThumbLX, state->Gamepad.sThumbLY, true);
    mRightStick = GetStickState(state->Gamepad.sThumbRX, state->Gamepad.sThumbRY, false);

    /* Evan, the genius gave the great point that comparing these to the prev stick value would 
       function as a true stick change (and could save some event constructions in the case
       of a stick being held in the same position that is not zero) */

    // Left Stick not zero
    if (mLeftStick.x != 0.0f || mLeftStick.y != 0.0f)
    {
      XboxStickEvent stickEvent;
      stickEvent.StickDirection = mLeftStick;
      stickEvent.Stick = XboxButtons::LeftStick;
      stickEvent.Controller = this;
      SendEvent(Events::XboxStickEvent, &stickEvent);
    }
    // Right Stick not zero
    if (mRightStick.x != 0.0f || mRightStick.y != 0.0f)
    {
      XboxStickEvent stickEvent;
      stickEvent.StickDirection = mRightStick;
      stickEvent.Stick = XboxButtons::RightStick;
      stickEvent.Controller = this;
      SendEvent(Events::XboxStickEvent, &stickEvent);
    }
    // Left Stick recentered, this allows gameplay to depend on a stick returning to zero
    if (mLeftStick.x == 0.0f && mLeftStick.y == 0.0f && mPreviousLeftStick.x != 0.0f && mPreviousLeftStick.y != 0.0f)
    {
      XboxStickEvent stickEvent;
      stickEvent.StickDirection = mLeftStick;
      stickEvent.Stick = XboxButtons::LeftStick;
      stickEvent.Controller = this;
      SendEvent(Events::XboxStickEvent, &stickEvent);
    }
    // Right Stick recentered, this allows gameplay to depend on a stick returning to zero
    if (mRightStick.x == 0.0f && mRightStick.y == 0.0f && mPreviousRightStick.x != 0.0f && mPreviousRightStick.y != 0.0f)
    {
      XboxStickEvent stickEvent;
      stickEvent.StickDirection = mRightStick;
      stickEvent.Stick = XboxButtons::RightStick;
      stickEvent.Controller = this;
      SendEvent(Events::XboxStickEvent, &stickEvent);
    }

    //Stick flicking
    bool leftStickFlicked = (glm::length(mLeftStick) >= cFlickMagnitude);
    bool rightStickFlicked = (glm::length(mRightStick) >= cFlickMagnitude);

    if (leftStickFlicked && !mLeftStickFlicked)
    {
      XboxFlickEvent flickEvent;
      flickEvent.FlickedStick = XboxButtons::LeftStick;
      flickEvent.FlickDirection = glm::normalize(mLeftStick);
      flickEvent.Controller = this;
      SendEvent(Events::XboxStickFlicked, &flickEvent);
    }

    if (rightStickFlicked && !mRightStickFlicked)
    {
      XboxFlickEvent flickEvent;
      flickEvent.FlickedStick = XboxButtons::RightStick;
      flickEvent.FlickDirection = glm::normalize(mRightStick);
      flickEvent.Controller = this;
      SendEvent(Events::XboxStickFlicked, &flickEvent);
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
      Vibrate(leftVibe, rightVibe);
    }
    else if ((mLeftVibe != 0.0f || mRightVibe != 0.0f) && (mVibrations.size() == 0))
    {
      mLeftVibe = 0.0f;
      mRightVibe = 0.0f;
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

  void XboxController::VibrateForTime(float aLeftSpeed, float aRightSpeed, double aTime)
  {
    Vibration vibe;
    vibe.mLeft = aLeftSpeed;
    vibe.mRight = aRightSpeed;
    vibe.mTime = aTime;
    mVibrations.push_back(vibe);
  }
}

#endif