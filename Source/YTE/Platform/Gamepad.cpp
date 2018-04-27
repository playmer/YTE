/******************************************************************************/
/*!
 * \author Joshua T. Fisher
 * \date   2015-6-7
 *
 * \copyright All content 2016 DigiPen (USA) Corporation, all rights reserved.
 */
/******************************************************************************/
#include "YTE/Platform/Gamepad.hpp"

namespace YTE
{
  YTEDefineEvent(XboxStickFlicked);
  YTEDefineEvent(XboxStickEvent);
  YTEDefineEvent(XboxButtonPress);
  YTEDefineEvent(XboxButtonRelease);
  YTEDefineEvent(XboxButtonPersist);

  YTEDefineType(XboxFlickEvent)
  {
    YTERegisterType(XboxFlickEvent);
    YTEBindField(&XboxFlickEvent::FlickDirection, "FlickDirection", PropertyBinding::Get);
    YTEBindField(&XboxFlickEvent::FlickedStick, "FlickedStick", PropertyBinding::Get);
    YTEBindField(&XboxFlickEvent::Controller, "Controller", PropertyBinding::Get);
  }
  
  YTEDefineType(XboxStickEvent)
  {
    YTERegisterType(XboxStickEvent);
    YTEBindField(&XboxStickEvent::StickDirection, "StickDirection", PropertyBinding::Get);
    YTEBindField(&XboxStickEvent::Stick, "Stick", PropertyBinding::Get);
    YTEBindField(&XboxStickEvent::Controller, "Controller", PropertyBinding::Get);
  }

  YTEDefineType(XboxButtonEvent)
  {
    YTERegisterType(XboxButtonEvent);
    YTEBindField(&XboxButtonEvent::Button, "Button", PropertyBinding::GetSet);
    YTEBindField(&XboxButtonEvent::Controller, "Controller", PropertyBinding::GetSet);
  }


  YTEDefineType(XboxController)
  {
    //YTERegisterType(XboxController);
    //YTEBindProperty(&XboxController::GetId, YTENoSetter, "Id");
    //YTEBindProperty(&XboxController::GetLeftStick, YTENoSetter, "LeftStick");
    //YTEBindProperty(&XboxController::GetRightStick, YTENoSetter, "RightStick");
    //YTEBindProperty(&XboxController::GetLeftTrigger, YTENoSetter, "LeftTrigger");
    //YTEBindProperty(&XboxController::GetRightTrigger, YTENoSetter, "RightTrigger");
    //YTEBindProperty(&XboxController::Active, YTENoSetter, "Active");
    //
    //YTEBindFunction(&XboxController::IsButtonDown, YTENoOverload, "IsButtonDown", YTEParameterNames("aButton")).Description()
    //  = "Finds if the given button is pressed right now.";
    //YTEBindFunction(&XboxController::WasButtonDown, YTENoOverload, "WasButtonDown", YTEParameterNames("aButton")).Description()
    //  = "Finds if the given button is pressed last frame.";
    ////YTEBindFunction(&XboxController::Vibrate, YTENoOverload, "Vibrate", "aLeftSpeed, aRightSpeed").Description()
    ////  = "Sets the controllers motors to vibrate via the specified amounts.";
    //YTEBindFunction(&XboxController::VibrateForTime, YTENoOverload, "VibrateForTime", YTEParameterNames("aLeftSpeed", "aRightSpeed", "aTime")).Description()
    //  = "The controller will vibrate for the given amount of time. If called again, will choose the higher vibration.";
  }

  ///////////////////////////////////////
  // Public Functions
  ///////////////////////////////////////
  //////////////////////////////
  // Constructors
  //////////////////////////////
  XboxController::XboxController()
  {
    std::memset(mButtonArrayOne, 0, sizeof(mButtonArrayOne));
    std::memset(mButtonArrayTwo, 0, sizeof(mButtonArrayTwo));

    mCurrentButtons = mButtonArrayOne;
    mPreviousButtons = mButtonArrayTwo;

    mLeftStickFlicked = false;
    mRightStickFlicked = false;
    mLeftVibe = 0.0f;
    mRightVibe = 0.0f;
  }


  XboxController::~XboxController()
  {
    Vibrate(0.0f, 0.0f);
  }

  //////////////////////////////
  // Getters/Setters
  //////////////////////////////
  bool XboxController::IsButtonDown(XboxButtons aButton)
  {
    return mCurrentButtons[static_cast<size_t>(aButton)];
  }
  bool XboxController::WasButtonDown(XboxButtons aButton)
  {
    return mPreviousButtons[static_cast<size_t>(aButton)];
  }
}
