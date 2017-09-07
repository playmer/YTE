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
  DefineEvent(WindowResizeEvent);
  DefineEvent(WindowFocusLostOrGained);
  DefineEvent(WindowMinimizedOrRestored);
  DefineType(Window)
  {
    YTERegisterType(Window);
    YTEBindField(&Window::mKeyboard, "Keyboard", PropertyBinding::GetSet);
    YTEBindField(&Window::mMouse, "Mouse", PropertyBinding::GetSet);

    YTEAddFunction( &Window::SetFullscreen, YTENoOverload, "SetFullscreen", YTEParameterNames("aFullscreen", "aForMetro"))->Description()
      = "Either switches to fullscreen or unfullscreen.";
    YTEAddFunction( &Window::SetCursorVisibility, YTENoOverload, "SetCursorVisibility", YTEParameterNames("aShow"))->Description()
      = "Turns the visibility of the mouse cursor on and off";

    YTEBindField(&Window::mSerializedWindowName,     "SerializedWindowName",     PropertyBinding::GetSet)->AddAttribute<EditorProperty>();
    YTEBindField(&Window::mSerializedWindowIcon,     "SerializedWindowIcon",     PropertyBinding::GetSet)->AddAttribute<EditorProperty>();
    YTEBindField(&Window::mSerializedCursorIcon,     "SerializedCursorIcon",     PropertyBinding::GetSet)->AddAttribute<EditorProperty>();
    YTEBindField(&Window::mSerializedStartingWidth,  "SerializedStartingWidth",  PropertyBinding::GetSet)->AddAttribute<EditorProperty>();
    YTEBindField(&Window::mSerializedStartingHeight, "SerializedStartingHeight", PropertyBinding::GetSet)->AddAttribute<EditorProperty>();
    YTEBindField(&Window::mSerializedStartingFullscreen, "SerializedStartingFullscreen", PropertyBinding::GetSet)->AddAttribute<EditorProperty>();
    YTEBindField(&Window::mSerializedShouldBeRenderedTo, "SerializedShouldBeRenderedTo", PropertyBinding::GetSet)->AddAttribute<EditorProperty>();
  }
}
