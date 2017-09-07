/******************************************************************************/
/*!
 * \author Joshua T. Fisher
 * \date   2015-09-19
 *
 * \copyright All content 2016 DigiPen (USA) Corporation, all rights reserved.
 */
/******************************************************************************/
#pragma once

#ifndef YTE_Event_StandardEvents_h
#define YTE_Event_StandardEvents_h

#include "YTE/Core/ForwardDeclarations.hpp"

#include "YTE/Platform/Gamepad.hpp"
#include "YTE/Platform/Keyboard.hpp"
#include "YTE/Platform/Mouse.hpp"

#include "YTE/Event/Events.h"

namespace YTE
{
  namespace Events
  {
    // Core
    extern const std::string LogicUpdate;
    extern const std::string FrameUpdate;
    extern const std::string BeginDebugDrawUpdate;
    extern const std::string DebugDrawUpdate;
    extern const std::string EndDebugDrawUpdate;
    extern const std::string DeletionUpdate;
    extern const std::string BoundTypeChanged;

    // Input
    extern const std::string MousePress;
    extern const std::string MouseRelease;
    extern const std::string MousePersist;
    extern const std::string MouseScroll;
    extern const std::string MouseMove;
    extern const std::string KeyPress;
    extern const std::string KeyRelease;
    extern const std::string KeyPersist;
    extern const std::string XboxStickFlicked;
    extern const std::string XboxButtonPress;
    extern const std::string XboxButtonRelease;
    extern const std::string XboxButtonPersist;

    //Physics
    extern const std::string PositionChanged;
    extern const std::string RotationChanged;
    extern const std::string ScaleChanged;
    extern const std::string OrientationChanged;
    extern const std::string CollisionStarted;
    extern const std::string CollisionPersisted;
    extern const std::string CollisionEnded;
    extern const std::string MouseEnter;
    extern const std::string MouseExit;
  }

  class LogicUpdate : public Event
  {
  public:
    DeclareType(LogicUpdate);

    float Dt;
  };

  class BoundTypeChanged : public Event
  {
  public:
    DeclareType(BoundTypeChanged);

    BoundType *aOldType;
    BoundType *aNewType;
  };
    
  class MouseButtonEvent : public Event
  {
  public:
    DeclareType(MouseButtonEvent);

    glm::vec2 WorldCoordinates;
    Mouse_Buttons Button;
    Mouse *Mouse;
  };


  class MouseWheelEvent : public Event
  {
  public:
    DeclareType(MouseWheelEvent);

    glm::vec2 WorldCoordinates;
    glm::vec2 ScrollMovement;
    Mouse *Mouse;
  };

  class MouseMoveEvent : public Event
  {
  public:
    DeclareType(MouseMoveEvent);

    glm::vec2 WorldCoordinates;
    Mouse *Mouse;
  };

  class KeyboardEvent : public Event
  {
  public:
    DeclareType(KeyboardEvent);

    Keys Key;
    Keyboard *Keyboard;
  };

  class XboxFlickEvent :public Event
  {
  public:
    DeclareType(XboxFlickEvent);

    glm::vec2 FlickDirection;
    Xbox_Buttons FlickedStick;
    XboxController *Controller;
  };

  class XboxButtonEvent :public Event
  {
  public:
    DeclareType(XboxButtonEvent);

    Xbox_Buttons Button;
    XboxController *Controller;
  };

  class PositionChanged :public Event
  {
  public:
    DeclareType(PositionChanged);

    glm::vec3 Position;
  };

  class RotationChanged : public Event
  {
  public:
    DeclareType(RotationChanged);

    glm::quat Rotation;
  };

  class ScaleChanged : public Event
  {
  public:
    DeclareType(ScaleChanged);

    glm::vec3 Scale;
  };

  class OrientationChanged : public Event
  {
  public:
    DeclareType(OrientationChanged);

    glm::vec3 Forward;
    glm::vec3 Right;
    glm::vec3 Up;
  };

  class CollisionEvent : public Event
  {
  public:
    DeclareType(CollisionEvent);
    Composition *OtherObject;
  };

  class CollisionStarted : public Event
  {
  public:
    DeclareType(CollisionStarted);
    Composition *OtherObject;
    String Name;
    Composition* Object;
  };

  class CollisionPersisted : public Event
  {
  public:
    DeclareType(CollisionPersisted);
    Composition *OtherObject;

    String Name;
    Composition* Object;
  };

  class CollisionEnded : public Event
  {
  public:
    DeclareType(CollisionEnded);
    Composition *OtherObject;

    String Name;
    Composition* Object;
  };

  class MouseEnter : public Event
  {
  public:
    DeclareType(MouseEnter);

    String Name;
  };

  class MouseExit : public Event
  {
  public:
    DeclareType(MouseExit);

    String Name;
  };
} // End yte

#endif
