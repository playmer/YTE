/******************************************************************************/
/*!
 * \author Joshua T. Fisher
 * \date   2015-6-7
 *
 * \copyright All content 2016 DigiPen (USA) Corporation, all rights reserved.
 */
/******************************************************************************/
#pragma once

#ifndef YTE_Platform_Gamepad_h
#define YTE_Platform_Gamepad_h

#include "YTE/Event/Events.h"

#include "YTE/Platform/DeviceEnums.hpp"
namespace YTE
{
  class XboxControllerState;

  struct Vibration
  {
    float mLeft;
    float mRight;
    float mTime;
  };


  class XboxController : public BaseEventHandler
  {
  public:
    DeclareType(XboxController);

    XboxController(); 
    ~XboxController();

    void UpdateState(XboxControllerState *state, float aDt);
    void Vibrate(float aLeftSpeed, float aRightSpeed);
    void VibrateForTime(float aLeftSpeed, float aRightSpeed, float aTime);

    bool IsButtonDown(Xbox_Buttons aButton);
    bool WasButtonDown(Xbox_Buttons aButton);
    inline bool Active() { return mActive; };
    inline glm::vec2 GetLeftStick() { return mLeftStick; };
    inline glm::vec2 GetRightStick() { return mRightStick; };
    inline float GetLeftTrigger() { return mLeftTrigger; };
    inline float GetRightTrigger() { return mRightTrigger; };
    inline float GetId() { return mGamepadIndex; };

    uint8_t mGamepadIndex;
  private:
    XboxController& operator=(XboxController&);

    bool mButtonArrayOne[static_cast<size_t>(Xbox_Buttons::Xbox_Buttons_Number)];
    bool mButtonArrayTwo[static_cast<size_t>(Xbox_Buttons::Xbox_Buttons_Number)];

    bool *mCurrentButtons;
    bool *mPreviousButtons;

    float mLeftVibe;
    float mRightVibe;

    glm::vec2 mLeftStick;
    glm::vec2 mRightStick;

    float mLeftTrigger;
    float mRightTrigger;

    bool mLeftStickFlicked;
    bool mRightStickFlicked;

    bool mActive;

    std::vector<Vibration> mVibrations;

    const float cFlickMagnitude = 0.8f;
    const float cMaxRumble = 65535.0f;
  };
}

#endif