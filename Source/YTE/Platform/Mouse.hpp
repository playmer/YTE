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
    Mouse *Mouse;
  };


  class MouseWheelEvent : public Event
  {
  public:
    YTEDeclareType(MouseWheelEvent);

    glm::i32vec2 WorldCoordinates;
    glm::vec2 ScrollMovement;
    Mouse *Mouse;
  };

  class MouseMoveEvent : public Event
  {
  public:
    YTEDeclareType(MouseMoveEvent);

    glm::i32vec2 WorldCoordinates;
    Mouse *Mouse;
  };

  glm::i32vec2 GetMousePosition();
  bool GetLRSwapped();
  void SurveyMouse(bool *aMouse);
  uint64_t TranslateFromMouseButtonToOsKey(MouseButtons aOsKey);

  class Mouse : public EventHandler
  {
  public:
    YTEDeclareType(Mouse);

    Mouse();

    void Update();
    void UpdateButton(MouseButtons aButton, bool aDown, glm::i32vec2 aPosition);
    void UpdateWheel(glm::vec2 aWheelMove, glm::i32vec2 aPosition);
    void UpdatePosition(glm::i32vec2 aPosition);

    glm::i32vec2 GetPositionDelta();
    glm::i32vec2 GetPrevPosition();

    // Updates the buttons via the OS.
    void UpdateAllButtons(glm::i32vec2 aRelativePosition);
    bool AnyButtonDown();
    bool IsButtonDown(MouseButtons aButton);
    bool WasButtonDown(MouseButtons aButton);
    glm::i32vec2 GetCursorPosition();
    void SetCursorPosition(glm::i32vec2 aPosition);

  private:
    glm::i32vec2 mPosition;
    glm::i32vec2 mPrevPosition;

    bool mPositionChanged;

    bool *mMouseCurrent;
    bool *mMousePrevious;

    bool mArrayOne[static_cast<size_t>(MouseButtons::Mouse_Buttons_Number)];
    bool mArrayTwo[static_cast<size_t>(MouseButtons::Mouse_Buttons_Number)];

    bool mLRSwapped;
  };
}

#endif