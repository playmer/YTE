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

#include "YTE/Core/EventHandler.hpp"

#include "YTE/Platform/DeviceEnums.hpp"
namespace YTE
{
  YTEDeclareEvent(XboxStickFlicked);
  YTEDeclareEvent(XboxStickEvent);
  YTEDeclareEvent(XboxButtonPress);
  YTEDeclareEvent(XboxButtonRelease);
  YTEDeclareEvent(XboxButtonPersist);

  class XboxController;
  class XboxFlickEvent : public Event
  {
  public:
    YTEDeclareType(XboxFlickEvent);

    glm::vec2 FlickDirection;
    XboxButtons FlickedStick;
    XboxController *Controller;
  };

  class XboxStickEvent : public Event
  {
  public:
    YTEDeclareType(XboxStickEvent);

    glm::vec2 StickDirection;
    XboxButtons Stick;
    XboxController *Controller;
  };

  class XboxButtonEvent : public Event
  {
  public:
    YTEDeclareType(XboxButtonEvent);

    XboxButtons Button;
    XboxController *Controller;
  };

  class XboxControllerState;

  struct Vibration
  {
    float mLeft;
    float mRight;
    double mTime;
  };


  class XboxController : public EventHandler
  {
  public:
    YTEDeclareType(XboxController);

    YTE_Shared XboxController();
    YTE_Shared ~XboxController();

    YTE_Shared void UpdateState(XboxControllerState *state, double aDt);
    YTE_Shared void Vibrate(float aLeftSpeed, float aRightSpeed);
    YTE_Shared void VibrateForTime(float aLeftSpeed, float aRightSpeed, double aTime);

    YTE_Shared bool IsButtonDown(XboxButtons aButton);
    YTE_Shared bool WasButtonDown(XboxButtons aButton);
    inline bool Active() { return mActive; };
    inline glm::vec2 GetLeftStick() { return mLeftStick; };
    inline glm::vec2 GetRightStick() { return mRightStick; };
    inline float GetLeftTrigger() { return mLeftTrigger; };
    inline float GetRightTrigger() { return mRightTrigger; };
    inline float GetId() { return mGamepadIndex; };

    uint8_t mGamepadIndex;
  private:
    void ProcessButton(void *aState, size_t aOsButton, XboxButtons aOurKey);
    
    XboxController& operator=(XboxController&);

    bool mButtonArrayOne[static_cast<size_t>(XboxButtons::Xbox_Buttons_Number)];
    bool mButtonArrayTwo[static_cast<size_t>(XboxButtons::Xbox_Buttons_Number)];

    bool *mCurrentButtons;
    bool *mPreviousButtons;

    float mLeftVibe;
    float mRightVibe;

    glm::vec2 mLeftStick;
    glm::vec2 mRightStick;
    glm::vec2 mPreviousLeftStick;
    glm::vec2 mPreviousRightStick;

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