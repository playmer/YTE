/******************************************************************************/
/*!
* \author Joshua T. Fisher
* \date   2015-6-7
*
* \copyright All content 2016 DigiPen (USA) Corporation, all rights reserved.
*/
/******************************************************************************/
#include "YTE/Platform/Window.hpp"

namespace YTE
{
  YTEDefineEvent(WindowResize);
  YTEDefineEvent(WindowFocusLostOrGained);
  YTEDefineEvent(WindowMinimizedOrRestored);

  YTEDefineType(Window)
  {
    RegisterType<Window>();
    TypeBuilder<Window> builder;
    builder.Field<&Window::mKeyboard>("Keyboard", PropertyBinding::Get);
    builder.Field<&Window::mMouse>("Mouse", PropertyBinding::Get);
    
    builder.Function<&Window::SetFullscreen>("SetFullscreen")
      .SetParameterNames("aFullscreen", "aForMetro")
      .SetDocumentation("Either switches to fullscreen or unfullscreen.");
    builder.Function<&Window::SetCursorVisibility>( "SetCursorVisibility")
      .SetParameterNames("aShow")
      .SetDocumentation("Turns the visibility of the mouse cursor on and off");
    
    builder.Field<&Window::mSerializedWindowName>("SerializedWindowName", PropertyBinding::GetSet)
      .AddAttribute<Serializable>()
      .AddAttribute<EditorProperty>();
    builder.Field<&Window::mSerializedWindowIcon>("SerializedWindowIcon", PropertyBinding::GetSet)
      .AddAttribute<Serializable>()
      .AddAttribute<EditorProperty>();
    builder.Field<&Window::mSerializedCursorIcon>("SerializedCursorIcon", PropertyBinding::GetSet)
      .AddAttribute<Serializable>()
      .AddAttribute<EditorProperty>();
    builder.Field<&Window::mSerializedStartingWidth>("SerializedStartingWidth", PropertyBinding::GetSet)
      .AddAttribute<Serializable>()
      .AddAttribute<EditorProperty>();
    builder.Field<&Window::mSerializedStartingHeight>("SerializedStartingHeight", PropertyBinding::GetSet)
      .AddAttribute<Serializable>()
      .AddAttribute<EditorProperty>();
    builder.Field<&Window::mSerializedStartingFullscreen>("SerializedStartingFullscreen", PropertyBinding::GetSet)
      .AddAttribute<Serializable>()
      .AddAttribute<EditorProperty>();
    builder.Field<&Window::mSerializedShouldBeRenderedTo>("SerializedShouldBeRenderedTo", PropertyBinding::GetSet)
      .AddAttribute<Serializable>()
      .AddAttribute<EditorProperty>();
  }

  bool BeyondExtent(glm::i32vec2 aWindowSize, glm::i32vec2 aMousePosition)
  {
    if (aMousePosition.x < 0 || aMousePosition.y < 0 ||
      aMousePosition.x > aWindowSize.x || aMousePosition.y > aWindowSize.y)
    {
      return true;
    }

    return false;
  }

  void Window::Update()
  {
    YTEProfileFunction();
    PlatformUpdate();

    // We want to handle the situation where the mouse is 
    // off the window but we're still in focus.
    auto mousePosisiton = GetMousePosition();
    auto windowPosition = GetPosition();

    glm::i32vec2 windowSize{ GetWidth(), GetHeight() };

    auto relativeMousePostion = mousePosisiton - windowPosition;

    auto beyond = BeyondExtent(windowSize, relativeMousePostion);
    auto mouseDown = mMouse.AnyButtonDown();

    if (beyond && mouseDown)
    {
      mMouse.UpdateAllButtons(relativeMousePostion);
      mouseDown = mMouse.AnyButtonDown();
    }

    mMouse.Update();
    mKeyboard.Update();
  }
}