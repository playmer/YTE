#include <utility>

#include "YTE/Core/Engine.hpp"

#include "YTE/Platform/Keyboard.hpp"
#include "YTE/Platform/Mouse.hpp"
#include "YTE/Platform/Window.hpp"

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
    builder.Field<&MouseWheelEvent::WindowCoordinates>("WorldCoordinates", PropertyBinding::Get);
    builder.Field<&MouseWheelEvent::ScreenCoordinates>("ScreenCoordinates", PropertyBinding::Get);
    builder.Field<&MouseWheelEvent::ScrollMovement>("ScrollMovement", PropertyBinding::Get);
  }

  YTEDefineType(MouseButtonEvent)
  {
    RegisterType<MouseButtonEvent>();
    TypeBuilder<MouseButtonEvent> builder;
    builder.Field<&MouseButtonEvent::WindowCoordinates>("WorldCoordinates", PropertyBinding::Get);
    builder.Field<&MouseButtonEvent::ScreenCoordinates>("ScreenCoordinates", PropertyBinding::Get);
    builder.Field<&MouseButtonEvent::Button>("Button", PropertyBinding::Get);
  }

  YTEDefineType(MouseMoveEvent)
  {
    RegisterType<MouseMoveEvent>();
    TypeBuilder<MouseMoveEvent> builder;
    builder.Field<&MouseMoveEvent::WindowCoordinates>("WorldCoordinates", PropertyBinding::Get);
    builder.Field<&MouseMoveEvent::ScreenCoordinates>("ScreenCoordinates", PropertyBinding::Get);
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
    
    builder.Property<&Mouse::GetPositionInWindowCoordinates, NoSetter>("CursorPositionInWindowCoordinates")
      .SetDocumentation("Gets the current cursor position in window coordinates.");

    builder.Property<&Mouse::GetPositionInScreenCoordinates, NoSetter>("CursorPositionInScreenCoordinates")
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
    mouseEvent.WindowCoordinates = mWindowCoordinates;
    mouseEvent.ScreenCoordinates = mWindowCoordinates + mWindow->GetPosition();

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
    UpdatePosition(aPosition);
    UpdateButton(aButton, aDown);
  }

  void Mouse::UpdateButton(MouseButtons aButton, bool aDown)
  {
    size_t index = EnumCast(aButton);

    // Button got resent.
    if (mMouseCurrent[index] == aDown)
    {
      return;
    }

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
    mouseEvent.WindowCoordinates = mWindowCoordinates;
    mouseEvent.ScreenCoordinates = mWindowCoordinates + mWindow->GetPosition();
    mouseEvent.SendingMouse = this;

    SendEvent(*state, &mouseEvent);
  }

  void Mouse::UpdateWheel(glm::vec2 aWheelMove)
  {
    MouseWheelEvent mouseEvent;
    mouseEvent.ScrollMovement = aWheelMove;
    mouseEvent.WindowCoordinates = mWindowCoordinates;
    mouseEvent.ScreenCoordinates = mWindowCoordinates + mWindow->GetPosition();

    SendEvent(Events::MouseScroll, &mouseEvent);
  }

  void Mouse::UpdatePosition(glm::i32vec2 aPosition)
  {
    if (mWindowCoordinates != aPosition)
    {
      mPreviousWindowCoordinates = mWindowCoordinates;
      mWindowCoordinates = aPosition;
      mPositionChanged = true;
    }
  }

  glm::i32vec2 Mouse::GetPositionDelta()
  {
    return mWindowCoordinates - mPreviousWindowCoordinates;
  }

  glm::i32vec2 Mouse::GetPrevPosition()
  {
    return mPreviousWindowCoordinates;
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

  glm::i32vec2 Mouse::GetPositionInWindowCoordinates()
  {
    return mWindowCoordinates;
  }

  glm::i32vec2 Mouse::GetPositionInScreenCoordinates()
  {
    return mWindowCoordinates + mWindow->GetPosition();
  }
}