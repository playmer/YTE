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
    YTERegisterType(Window);
    YTEBindField(&Window::mKeyboard, "Keyboard", PropertyBinding::GetSet);
    YTEBindField(&Window::mMouse, "Mouse", PropertyBinding::GetSet);

    YTEBindFunction(&Window::SetFullscreen, YTENoOverload, "SetFullscreen", YTEParameterNames("aFullscreen", "aForMetro")).Description()
      = "Either switches to fullscreen or unfullscreen.";
    YTEBindFunction(&Window::SetCursorVisibility, YTENoOverload, "SetCursorVisibility", YTEParameterNames("aShow")).Description()
      = "Turns the visibility of the mouse cursor on and off";

    YTEBindField(&Window::mSerializedWindowName, "SerializedWindowName", PropertyBinding::GetSet)
      .AddAttribute<Serializable>()
      .AddAttribute<EditorProperty>();
    YTEBindField(&Window::mSerializedWindowIcon, "SerializedWindowIcon", PropertyBinding::GetSet)
      .AddAttribute<Serializable>()
      .AddAttribute<EditorProperty>();
    YTEBindField(&Window::mSerializedCursorIcon, "SerializedCursorIcon", PropertyBinding::GetSet)
      .AddAttribute<Serializable>()
      .AddAttribute<EditorProperty>();
    YTEBindField(&Window::mSerializedStartingWidth, "SerializedStartingWidth", PropertyBinding::GetSet)
      .AddAttribute<Serializable>()
      .AddAttribute<EditorProperty>();
    YTEBindField(&Window::mSerializedStartingHeight, "SerializedStartingHeight", PropertyBinding::GetSet)
      .AddAttribute<Serializable>()
      .AddAttribute<EditorProperty>();
    YTEBindField(&Window::mSerializedStartingFullscreen, "SerializedStartingFullscreen", PropertyBinding::GetSet)
      .AddAttribute<Serializable>()
      .AddAttribute<EditorProperty>();
    YTEBindField(&Window::mSerializedShouldBeRenderedTo, "SerializedShouldBeRenderedTo", PropertyBinding::GetSet)
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