/******************************************************************************/
/*!
 * \author Joshua T. Fisher
 * \date   2015-6-7
 *
 * \copyright All content 2016 DigiPen (USA) Corporation, all rights reserved.
 */
/******************************************************************************/
#include <utility>



#include "YTE/Platform/Mouse.hpp"

namespace
{
  const int YTE_NUM_MOUSEBUTTONS = 3;
}

namespace YTE
{
  DefineEvent(MousePress);
  DefineEvent(MouseRelease);
  DefineEvent(MousePersist);
  DefineEvent(MouseScroll);
  DefineEvent(MouseMove);

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

  DefineType(Mouse)
  {
    YTERegisterType(Mouse);
    YTEAddFunction( &Mouse::IsButtonDown, YTENoOverload, "IsButtonDown", YTEParameterNames("aButton"))->Description()
      = "Finds if the given button is pressed right now.";
    YTEAddFunction( &Mouse::WasButtonDown, YTENoOverload, "WasButtonDown", YTEParameterNames("aButton"))->Description()
      = "Finds if the given button is pressed last frame.";

    YTEBindProperty(&Mouse::GetCursorPosition, YTENoSetter, YTEParameterNames("CursorPosition"))->Description() = "Get's the current cursor position in screen coordinates.";
  }

  Mouse::Mouse()
  {
    std::memset(mArrayOne, 0, sizeof(mArrayOne));
    std::memset(mArrayTwo, 0, sizeof(mArrayTwo));
    mLRSwapped = GetLRSwapped();

    mMouseCurrent = mArrayOne;
    mMousePrevious = mArrayTwo;
  }

  void Mouse::Update()
  {
    MouseButtonEvent mouseEvent;

    for (size_t i = 0; i < enum_cast(Mouse_Buttons::Mouse_Buttons_Number); ++i)
    {
      mouseEvent.WorldCoordinates = mMousePosition;

      if (mMouseCurrent[i] && mMousePrevious[i])
      {
        mouseEvent.Button = static_cast<Mouse_Buttons>(i);
        mouseEvent.Mouse = this;

        std::cout << "Mouse Persist:" << i << std::endl;

        SendEvent(Events::MousePersist, &mouseEvent);
      }

      mMousePrevious[i] = mMouseCurrent[i];
    }
  }

  void Mouse::UpdateButton(Mouse_Buttons aButton, bool aDown, glm::vec2 aPosition)
  {
    size_t index = enum_cast(aButton);
        
      // Button got resent.
    if (mMouseCurrent[index] == aDown)
    {
      return;
    }

    auto truth = aDown ? "Down: " : "Up: ";

    std::cout << "Mouse " << truth << enum_cast(aButton) << std::endl;

    mMousePosition = aPosition;

    mMousePrevious[index] = mMouseCurrent[index];

    mMouseCurrent[index] = aDown;
  
    const std::string *state;

    if (aDown)
    {
      state = &Events::MousePress;
    }
    else
    {
      state = &Events::MouseRelease;
    }

    MouseButtonEvent mouseEvent;
    mouseEvent.Button = aButton;
    mouseEvent.WorldCoordinates = aPosition;
    mouseEvent.Mouse = this;

    SendEvent(*state, &mouseEvent);
  }


  void Mouse::UpdateWheel(glm::vec2 aWheelMove, glm::vec2 aPosition)
  {
    mMousePosition = aPosition;

    MouseWheelEvent mouseEvent;
    mouseEvent.ScrollMovement = aWheelMove;
    mouseEvent.WorldCoordinates = aPosition;

    SendEvent(Events::MouseScroll, &mouseEvent);
  }

  void Mouse::UpdatePosition(glm::vec2 aPosition)
  {
    mMousePosition = aPosition;

    MouseMoveEvent mouseEvent;
    mouseEvent.WorldCoordinates = aPosition;

    SendEvent(Events::MouseMove, &mouseEvent);
  }

  bool Mouse::IsButtonDown(Mouse_Buttons aButton)
  {
    return mMouseCurrent[enum_cast(aButton)];
  }
  bool Mouse::WasButtonDown(Mouse_Buttons aButton)
  {
    return mMousePrevious[enum_cast(aButton)];
  }
  glm::vec2 Mouse::GetCursorPosition()
  {
    return mMousePosition;
  }
}
