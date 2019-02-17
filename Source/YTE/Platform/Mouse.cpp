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
    RegisterType<MouseWheelEvent>();
    TypeBuilder<MouseWheelEvent> builder;
    builder.Field<&MouseWheelEvent::WorldCoordinates>("WorldCoordinates", PropertyBinding::Get);
    builder.Field<&MouseWheelEvent::ScrollMovement>("ScrollMovement", PropertyBinding::Get);
  }

  YTEDefineType(MouseButtonEvent)
  {
    RegisterType<MouseButtonEvent>();
    TypeBuilder<MouseButtonEvent> builder;
    builder.Field<&MouseButtonEvent::WorldCoordinates>("WorldCoordinates", PropertyBinding::Get);
    builder.Field<&MouseButtonEvent::Button>("Button", PropertyBinding::Get);
  }

  YTEDefineType(MouseMoveEvent)
  {
    RegisterType<MouseMoveEvent>();
    TypeBuilder<MouseMoveEvent> builder;
    builder.Field<&MouseMoveEvent::WorldCoordinates>("WorldCoordinates", PropertyBinding::Get);
  }

  YTEDefineType(Mouse)
  {
    RegisterType<Mouse>();
    TypeBuilder<Mouse> builder;
    builder.Function<&Mouse::IsButtonDown>("IsButtonDown")
      .SetParameterNames("aButton")
      .SetDocumentation("Finds if the given button is pressed right now.");
    builder.Function<&Mouse::WasButtonDown>("WasButtonDown")
      .SetParameterNames("aButton")
      .SetDocumentation("Finds if the given button is pressed last frame.");
    
    builder.Property<&Mouse::GetCursorPosition, NoSetter>("CursorPosition")
      .SetDocumentation("Gets the current cursor position in screen coordinates.");
  }

  Mouse::Mouse(Window* aWindow)
    : mPositionChanged{ false }
    , mWindow{aWindow}
  {
    std::memset(mArrayOne, 0, sizeof(mArrayOne));
    std::memset(mArrayTwo, 0, sizeof(mArrayTwo));

    mMouseCurrent = mArrayOne;
    mMousePrevious = mArrayTwo;
  }

  void Mouse::Update()
  {
    MouseButtonEvent mouseEvent;
    mouseEvent.WorldCoordinates = mPosition;

    for (size_t i = 0; i < EnumCast(MouseButtons::Mouse_Buttons_Number); ++i)
    {
      if (mMouseCurrent[i] && mMousePrevious[i])
      {
        mouseEvent.Button = static_cast<MouseButtons>(i);
        mouseEvent.SendingMouse = this;

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
    size_t index = EnumCast(aButton);

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
    mouseEvent.SendingMouse = this;

    SendEvent(*state, &mouseEvent);
  }

  void Mouse::UpdateWheel(glm::vec2 aWheelMove)
  {
    MouseWheelEvent mouseEvent;
    mouseEvent.ScrollMovement = aWheelMove;
    mouseEvent.WorldCoordinates = mPosition;

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

  bool Mouse::AnyButtonDown()
  {
    for (size_t i = 0; i < EnumCast(MouseButtons::Mouse_Buttons_Number); ++i)
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
    return mMouseCurrent[EnumCast(aButton)];
  }

  bool Mouse::WasButtonDown(MouseButtons aButton)
  {
    return mMousePrevious[EnumCast(aButton)];
  }

  glm::i32vec2 Mouse::GetCursorPosition()
  {
    return mPosition;
  }
}