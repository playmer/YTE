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

#include "YTE/Event/Events.h"

#include "YTE/Platform/DeviceEnums.hpp"

namespace YTE
{
  glm::vec2 GetMousePosition();
  bool GetLRSwapped();
  void SurveyMouse(bool *aMouse);


  struct Mouse : public BaseEventHandler
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