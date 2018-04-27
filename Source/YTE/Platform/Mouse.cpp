/******************************************************************************/
/*!
* \author Joshua T. Fisher
* \date   2015-6-7
*
* \copyright All content 2016 DigiPen (USA) Corporation, all rights reserved.
*/
/******************************************************************************/
#include <utility>

#include "YTE/Platform/Keyboard.hpp"
#include "YTE/Platform/Mouse.hpp"

namespace YTE
{
  YTEDefineEvent(MousePress);
  YTEDefineEvent(MouseRelease);
  YTEDefineEvent(MousePersist);
  YTEDefineEvent(MouseScroll);
  YTEDefineEvent(MouseMove);

  YTEDefineType(MouseWheelEvent)
  {
    YTERegisterType(MouseWheelEvent);
    YTEBindField(&MouseWheelEvent::WorldCoordinates, "WorldCoordinates", PropertyBinding::Get);
    YTEBindField(&MouseWheelEvent::ScrollMovement, "ScrollMovement", PropertyBinding::Get);
  }

  YTEDefineType(MouseButtonEvent)
  {
    YTERegisterType(MouseButtonEvent);
    YTEBindField(&MouseButtonEvent::WorldCoordinates, "WorldCoordinates", PropertyBinding::Get);
    YTEBindField(&MouseButtonEvent::Button, "Button", PropertyBinding::Get);
  }

  YTEDefineType(MouseMoveEvent)
  {
    YTERegisterType(MouseMoveEvent);
    YTEBindField(&MouseMoveEvent::WorldCoordinates, "WorldCoordinates", PropertyBinding::Get);
  }

  YTEDefineType(Mouse)
  {
    //YTERegisterType(Mouse);
    //YTEBindFunction(&Mouse::IsButtonDown, YTENoOverload, "IsButtonDown", YTEParameterNames("aButton")).Description()
    //  = "Finds if the given button is pressed right now.";
    //YTEBindFunction(&Mouse::WasButtonDown, YTENoOverload, "WasButtonDown", YTEParameterNames("aButton")).Description()
    //  = "Finds if the given button is pressed last frame.";
    //
    //YTEBindProperty(&Mouse::GetCursorPosition, YTENoSetter, YTEParameterNames("CursorPosition")).Description()
    //  = "Gets the current cursor position in screen coordinates.";
  }

  Mouse::Mouse()
    : mPositionChanged(false)
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
    mouseEvent.WorldCoordinates = mPosition;

    for (size_t i = 0; i < enum_cast(MouseButtons::Mouse_Buttons_Number); ++i)
    {
      if (mMouseCurrent[i] && mMousePrevious[i])
      {
        mouseEvent.Button = static_cast<MouseButtons>(i);
        mouseEvent.Mouse = this;

        SendEvent(Events::MousePersist, &mouseEvent);
      }

      mMousePrevious[i] = mMouseCurrent[i];
    }

    if (mPositionChanged)
    {
      SendEvent(Events::MouseMove, &mouseEvent);
      mPositionChanged = false;
    }
  }

  void Mouse::UpdateButton(MouseButtons aButton, bool aDown, glm::i32vec2 aPosition)
  {
    size_t index = enum_cast(aButton);

    // Button got resent.
    if (mMouseCurrent[index] == aDown)
    {
      return;
    }

    UpdatePosition(aPosition);

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


  void Mouse::UpdateWheel(glm::vec2 aWheelMove, glm::i32vec2 aPosition)
  {
    UpdatePosition(aPosition);

    MouseWheelEvent mouseEvent;
    mouseEvent.ScrollMovement = aWheelMove;
    mouseEvent.WorldCoordinates = aPosition;

    SendEvent(Events::MouseScroll, &mouseEvent);
  }

  void Mouse::UpdatePosition(glm::i32vec2 aPosition)
  {
    
    if (mPosition != aPosition)
    {
      mPrevPosition = mPosition;
      mPosition = aPosition;
      mPositionChanged = true;
    }

  }

  glm::i32vec2 Mouse::GetPositionDelta()
  {
    return mPosition - mPrevPosition;
  }

  glm::i32vec2 Mouse::GetPrevPosition()
  {
    return mPrevPosition;
  }

  void Mouse::UpdateAllButtons(glm::i32vec2 aRelativePosition)
  {
    for (size_t i = 0; i < enum_cast(MouseButtons::Mouse_Buttons_Number); ++i)
    {
      auto button = static_cast<MouseButtons>(i);
      auto osButton = TranslateFromMouseButtonToOsKey(button);
      auto key = TranslateFromOsToOurKey(osButton);
      auto down = CheckKey(key);
      UpdateButton(button, down, aRelativePosition);
    }
  }

  bool Mouse::AnyButtonDown()
  {
    for (size_t i = 0; i < enum_cast(MouseButtons::Mouse_Buttons_Number); ++i)
    {
      if (mMouseCurrent[i])
      {
        return true;
      }
    }

    return false;
  }

  bool Mouse::IsButtonDown(MouseButtons aButton)
  {
    return mMouseCurrent[enum_cast(aButton)];
  }

  bool Mouse::WasButtonDown(MouseButtons aButton)
  {
    return mMousePrevious[enum_cast(aButton)];
  }

  glm::i32vec2 Mouse::GetCursorPosition()
  {
    return mPosition;
  }
}