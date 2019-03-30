/******************************************************************************/
/*!
* \author Joshua T. Fisher
* \date   2015-6-7
*
* \copyright All content 2016 DigiPen (USA) Corporation, all rights reserved.
*/
/******************************************************************************/
#pragma once

#ifndef YTE_Platform_Mouse_h
#define YTE_Platform_Mouse_h

#include <unordered_map>
#include <stdint.h>

#include "YTE/Core/EventHandler.hpp"

#include "YTE/Platform/DeviceEnums.hpp"
#include "YTE/Platform/ForwardDeclarations.hpp"

namespace YTE
{
  YTEDeclareEvent(MousePress);
  YTEDeclareEvent(MouseRelease);
  YTEDeclareEvent(MousePersist);
  YTEDeclareEvent(MouseScroll);
  YTEDeclareEvent(MouseMove);

  class MouseButtonEvent : public Event
  {
  public:
    YTEDeclareType(MouseButtonEvent);

    glm::i32vec2 WorldCoordinates;
    MouseButtons Button;
    Mouse *SendingMouse;
  };


  class MouseWheelEvent : public Event
  {
  public:
    YTEDeclareType(MouseWheelEvent);

    glm::i32vec2 WorldCoordinates;
    glm::vec2 ScrollMovement;
    Mouse *SendingMouse;
  };

  class MouseMoveEvent : public Event
  {
  public:
    YTEDeclareType(MouseMoveEvent);

    glm::i32vec2 WorldCoordinates;
    Mouse *SendingMouse;
  };

  YTE_Shared glm::i32vec2 GetMousePosition();
  YTE_Shared void SurveyMouse(bool *aMouse);

  class Mouse : public EventHandler
  {
  public:
    YTEDeclareType(Mouse);

    YTE_Shared Mouse(Window* aWindow);

    YTE_Shared void Update();
    YTE_Shared void UpdateButton(MouseButtons aButton, bool aDown, glm::i32vec2 aPosition);
    YTE_Shared void UpdateButton(MouseButtons aButton, bool aDown);
    YTE_Shared void UpdateWheel(glm::vec2 aWheelMove);
    YTE_Shared void UpdatePosition(glm::i32vec2 aPosition);

    YTE_Shared glm::i32vec2 GetPositionDelta();
    YTE_Shared glm::i32vec2 GetPrevPosition();

    // Updates the buttons via the OS.
    YTE_Shared void UpdateAllButtons(glm::i32vec2 aRelativePosition);
    YTE_Shared bool AnyButtonDown();
    YTE_Shared bool IsButtonDown(MouseButtons aButton);
    YTE_Shared bool WasButtonDown(MouseButtons aButton);
    YTE_Shared glm::i32vec2 GetCursorPosition();
    YTE_Shared void SetCursorPosition(glm::i32vec2 aPosition);

  private:
    glm::i32vec2 mPosition;
    glm::i32vec2 mPrevPosition;
    Window* mWindow;
    bool* mMouseCurrent;
    bool* mMousePrevious;

    bool mPositionChanged;


    bool mArrayOne[static_cast<size_t>(MouseButtons::Mouse_Buttons_Number)];
    bool mArrayTwo[static_cast<size_t>(MouseButtons::Mouse_Buttons_Number)];
  };
}

#endif