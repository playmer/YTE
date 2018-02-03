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
    Xbox_Buttons FlickedStick;
    XboxController *Controller;
  };

  class XboxStickEvent : public Event
  {
  public:
    YTEDeclareType(XboxStickEvent);

    glm::vec2 StickDirection;
    Xbox_Buttons Stick;
    XboxController *Controller;
  };

  class XboxButtonEvent : public Event
  {
  public:
    YTEDeclareType(XboxButtonEvent);

    Xbox_Buttons Button;
    XboxController *Controller;
  };

  class XboxControllerState;

  struct Vibration
  {
    float mLeft;
    float mRight;
    float mTime;
  };


  class XboxController : public EventHandler
  {
  public:
    YTEDeclareType(XboxController);

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
    void ProcessButton(void *aState, size_t aOsButton, Xbox_Buttons aOurKey);
    
    XboxController& operator=(XboxController&);

    bool mButtonArrayOne[static_cast<size_t>(Xbox_Buttons::Xbox_Buttons_Number)];
    bool mButtonArrayTwo[static_cast<size_t>(Xbox_Buttons::Xbox_Buttons_Number)];

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