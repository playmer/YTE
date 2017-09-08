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

namespace YTE
{
  DeclareEvent(MousePress);
  DeclareEvent(MouseRelease);
  DeclareEvent(MousePersist);
  DeclareEvent(MouseScroll);
  DeclareEvent(MouseMove);
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

  glm::vec2 GetMousePosition();
  bool GetLRSwapped();
  void SurveyMouse(bool *aMouse);


  struct Mouse : public EventHandler
  {
  public:
    DeclareType(Mouse);

    Mouse();

    void Update();
    void UpdateButton(Mouse_Buttons aButton, bool aDown, glm::vec2 aPosition);
    void UpdateWheel(glm::vec2 aWheelMove, glm::vec2 aPosition);
    void UpdatePosition(glm::vec2 aPosition);

    bool IsButtonDown(Mouse_Buttons aButton);
    bool WasButtonDown(Mouse_Buttons aButton);
    glm::vec2 GetCursorPosition();
      
  private:
    glm::vec2 mMousePosition;
      
    bool *mMouseCurrent;
    bool *mMousePrevious;

    bool mArrayOne[static_cast<size_t>(Mouse_Buttons::Mouse_Buttons_Number)];
    bool mArrayTwo[static_cast<size_t>(Mouse_Buttons::Mouse_Buttons_Number)];

    bool mLRSwapped;
  };
}

#endif