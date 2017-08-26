/******************************************************************************/
/*!
 * \author Joshua T. Fisher
 * \date   2015-09-19
 *
 * \copyright All content 2016 DigiPen (USA) Corporation, all rights reserved.
 */
/******************************************************************************/
#include "YTE/Core/Composition.hpp"

#include "YTE/Event/StandardEvents.h"
#include "YTE/Core/Composition.hpp"

namespace YTE
{
  namespace Events
  {
    // Core 
    const std::string LogicUpdate{ "LogicUpdate" };
    const std::string FrameUpdate{ "FrameUpdate" };
    const std::string BeginDebugDrawUpdate{ "BeginDebugDrawUpdate" };
    const std::string DebugDrawUpdate{ "DebugDrawUpdate" };
    const std::string EndDebugDrawUpdate{ "EndDebugDrawUpdate" };
    const std::string DeletionUpdate{ "DeletionUpdate" };
    const std::string BoundTypeChanged{ "BoundTypeChanged" };

    // Input
    const std::string MousePress{ "MousePress" };
    const std::string MouseRelease{ "MouseRelease" };
    const std::string MousePersist{ "MousePersist" };
    const std::string MouseScroll{ "MouseScroll" };
    const std::string MouseMove{ "MouseMove" };
    const std::string KeyPress{ "KeyPress" };
    const std::string KeyRelease{ "KeyRelease" };
    const std::string KeyPersist{ "KeyPersist" };
    const std::string WindowResize{ "WindowResize" };
    const std::string RendererResize{ "RendererResize" };
    const std::string WindowFocusLostOrGained{ "WindowFocusLostOrGained" };
    const std::string WindowMinimizedOrRestored{ "WindowMinimizedOrRestored" };
    const std::string XboxStickFlicked{ "XboxStickFlicked" };
    const std::string XboxButtonPress{ "XboxButtonPress" };
    const std::string XboxButtonRelease{ "XboxButtonRelease" };
    const std::string XboxButtonPersist{ "XboxButtonPersist" };

    // Physics
    const std::string PositionChanged{ "PositionChanged" };
    const std::string RotationChanged{ "RotationChanged" };
    const std::string ScaleChanged{ "ScaleChanged" };
    const std::string OrientationChanged{ "OrientationChanged" };
    const std::string CollisionStarted{ "CollisionStarted" };
    const std::string CollisionPersisted{ "CollisionPersisted" };
    const std::string CollisionEnded{ "CollisionEnded" };
    const std::string MouseEnter{ "MouseEnter" };
    const std::string MouseExit{ "MouseExit" };
  }
  DefineType(LogicUpdate)
  {
    YTERegisterType(LogicUpdate);
    YTEBindField(&LogicUpdate::Dt, "Dt", PropertyBinding::GetSet);
  }

  DefineType(BoundTypeChanged)
  {
    YTERegisterType(BoundTypeChanged);
    YTEBindField(&BoundTypeChanged::aOldType, "OldType", PropertyBinding::GetSet);
    YTEBindField(&BoundTypeChanged::aNewType, "NewType", PropertyBinding::GetSet);
  }

  DefineType(MouseWheelEvent)
  {
    YTERegisterType(MouseWheelEvent);
    YTEBindField(&MouseWheelEvent::WorldCoordinates, "WorldCoordinates", PropertyBinding::Get);
    YTEBindField(&MouseWheelEvent::ScrollMovement, "ScrollMovement", PropertyBinding::Get);
  }

  DefineType(MouseButtonEvent)
  {
    YTERegisterType(MouseButtonEvent);
    YTEBindField(&MouseButtonEvent::WorldCoordinates, "WorldCoordinates", PropertyBinding::Get);
    YTEBindField(&MouseButtonEvent::Button, "Button", PropertyBinding::Get);
  }

  DefineType(MouseMoveEvent)
  {
    YTERegisterType(MouseMoveEvent);
    YTEBindField(&MouseMoveEvent::WorldCoordinates, "WorldCoordinates", PropertyBinding::Get);
  }

  DefineType(KeyboardEvent)
  {
    YTERegisterType(KeyboardEvent);
    YTEBindField(&KeyboardEvent::Key, "Key", PropertyBinding::Get);
  }

  DefineType(XboxFlickEvent)
  {
    YTERegisterType(XboxFlickEvent);
    YTEBindField(&XboxFlickEvent::FlickDirection, "FlickDirection", PropertyBinding::Get);
    YTEBindField(&XboxFlickEvent::FlickedStick, "FlickedStick", PropertyBinding::Get);
    YTEBindField(&XboxFlickEvent::Controller, "Controller", PropertyBinding::Get);
  }

  DefineType(XboxButtonEvent)
  {
    YTERegisterType(XboxButtonEvent);
    YTEBindField(&XboxButtonEvent::Button, "Button", PropertyBinding::GetSet);
    YTEBindField(&XboxButtonEvent::Controller, "Controller", PropertyBinding::GetSet);
  }

  DefineType(PositionChanged)
  {
    YTERegisterType(PositionChanged);
    YTEBindField(&PositionChanged::Position, "Position", PropertyBinding::Get);
  }

  DefineType(RotationChanged)
  {
    YTERegisterType(RotationChanged);
    YTEBindField(&RotationChanged::Rotation, "Rotation", PropertyBinding::Get);
  }

  DefineType(ScaleChanged)
  {
    YTERegisterType(ScaleChanged);
    YTEBindField(&ScaleChanged::Scale, "Scale", PropertyBinding::Get);
  }

  DefineType(OrientationChanged)
  {
    YTERegisterType(OrientationChanged);
    YTEBindField(&OrientationChanged::Forward, "Forward", PropertyBinding::Get);
    YTEBindField(&OrientationChanged::Right, "Right", PropertyBinding::Get);
    YTEBindField(&OrientationChanged::Up, "Up", PropertyBinding::Get);
  }

  DefineType(CollisionEvent)
  {
    YTERegisterType(CollisionEvent);
    YTEBindField(&CollisionEvent::OtherObject, "OtherObject", PropertyBinding::Get);
  }


  DefineType(CollisionStarted)
  {
    YTERegisterType(CollisionStarted);
    YTEBindField(&CollisionStarted::Name, "Name", PropertyBinding::Get);
    YTEBindField(&CollisionStarted::Object, "Object", PropertyBinding::Get);
  }

  DefineType(CollisionPersisted)
  {
    YTERegisterType(CollisionPersisted);
    YTEBindField(&CollisionPersisted::Name, "Name", PropertyBinding::Get);
    YTEBindField(&CollisionPersisted::Object, "Object", PropertyBinding::Get);
  }

  DefineType(CollisionEnded)
  {
    YTERegisterType(CollisionEnded);
    YTEBindField(&CollisionEnded::Name, "Name", PropertyBinding::Get);
    YTEBindField(&CollisionEnded::Object, "Object", PropertyBinding::Get);
  }

  DefineType(MouseEnter)
  {
    YTERegisterType(MouseEnter);
    YTEBindField(&MouseEnter::Name, "Name", PropertyBinding::Get);
  }

  DefineType(MouseExit)
  {
    YTERegisterType(MouseExit);
    YTEBindField(&MouseExit::Name, "Name", PropertyBinding::Get);
  }
}