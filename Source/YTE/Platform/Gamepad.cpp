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
    RegisterType<XboxFlickEvent>();
    TypeBuilder<XboxFlickEvent> builder;
    builder.Field<&XboxFlickEvent::FlickDirection>( "FlickDirection", PropertyBinding::Get);
    builder.Field<&XboxFlickEvent::FlickedStick>( "FlickedStick", PropertyBinding::Get);
    builder.Field<&XboxFlickEvent::Controller>( "Controller", PropertyBinding::Get);
  }
  
  YTEDefineType(XboxStickEvent)
  {
    RegisterType<XboxStickEvent>();
    TypeBuilder<XboxStickEvent> builder;
    builder.Field<&XboxStickEvent::StickDirection>( "StickDirection", PropertyBinding::Get);
    builder.Field<&XboxStickEvent::Stick>( "Stick", PropertyBinding::Get);
    builder.Field<&XboxStickEvent::Controller>( "Controller", PropertyBinding::Get);
  }

  YTEDefineType(XboxButtonEvent)
  {
    RegisterType<XboxButtonEvent>();
    TypeBuilder<XboxButtonEvent> builder;
    builder.Field<&XboxButtonEvent::Button>( "Button", PropertyBinding::GetSet);
    builder.Field<&XboxButtonEvent::Controller>( "Controller", PropertyBinding::GetSet);
  }


  YTEDefineType(XboxController)
  {
    RegisterType<XboxController>();
    TypeBuilder<XboxController> builder;
    builder.Property<&XboxController::GetId, NoSetter>( "Id");
    builder.Property<&XboxController::GetLeftStick, NoSetter>( "LeftStick");
    builder.Property<&XboxController::GetRightStick, NoSetter>( "RightStick");
    builder.Property<&XboxController::GetLeftTrigger, NoSetter>( "LeftTrigger");
    builder.Property<&XboxController::GetRightTrigger, NoSetter>( "RightTrigger");
    builder.Property<&XboxController::Active, NoSetter>( "Active");
    
    builder.Function<&XboxController::IsButtonDown>( "IsButtonDown")
      .SetParameterNames("aButton")
      .SetDocumentation("Finds if the given button is pressed right now.");
    builder.Function<&XboxController::WasButtonDown>( "WasButtonDown")
      .SetParameterNames("aButton")
      .SetDocumentation("Finds if the given button is pressed last frame.");
    builder.Function<&XboxController::Vibrate>("Vibrate")
      .SetParameterNames("aLeftSpeed", "aRightSpeed")
      .SetDocumentation("Sets the controllers motors to vibrate via the specified amounts.");
    builder.Function<&XboxController::VibrateForTime>( "VibrateForTime")
      .SetParameterNames("aLeftSpeed", "aRightSpeed", "aTime")
      .SetDocumentation("The controller will vibrate for the given amount of time. If called again, will choose the higher vibration.");
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
