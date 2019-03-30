/******************************************************************************/
/*!
* \author Joshua T. Fisher
* \date   2015-6-7
*
* \copyright All content 2016 DigiPen (USA) Corporation, all rights reserved.
*/
/******************************************************************************/
#pragma once

#ifndef YTE_Platform_Window_h
#define YTE_Platform_Window_h

#include <any>
#include <string>

#include "YTE/Core/EventHandler.hpp"
#include "YTE/Core/ForwardDeclarations.hpp"

#include "YTE/Graphics/ForwardDeclarations.hpp"

#include "YTE/Platform/Keyboard.hpp"
#include "YTE/Platform/Mouse.hpp"
#include "YTE/Platform/TargetDefinitions.hpp"

#include "YTE/StandardLibrary/PrivateImplementation.hpp"

namespace YTE
{
  YTEDeclareEvent(WindowResize);
  YTEDeclareEvent(WindowFocusLostOrGained);
  YTEDeclareEvent(WindowMinimizedOrRestored);

  class WindowResize : public Event
  {
  public:
    int width;
    int height;
  };

  class WindowFocusLostOrGained : public Event
  {
  public:
    bool Focused;
  };

  class WindowMinimizedOrRestored : public Event
  {
  public:
    bool Minimized;
  };

  YTE_Shared bool SetCursorView(bool aShow);

  class Window : public EventHandler
  {
  public:
    YTEDeclareType(Window);

    YTE_Shared static i64 MessageHandler(void* aWindowHandle, u64 aMessage, u64 aWParam, i64 aLParam, Window *aWindow);

    YTE_Shared Window(Engine *aEngine, RSValue *aProperties);
    YTE_Shared Window(Engine *aEngine);

    YTE_Shared void SetResolution(u32 aWidth, u32 aHeight);
    YTE_Shared void SetFullscreen(bool aFullscreen);
    YTE_Shared void SetWindowTitle(const char *aString);

    YTE_Shared bool IsMinimized();
    YTE_Shared bool IsNotFocused();
    YTE_Shared bool IsFocused();

    YTE_Shared void SetExtent(u32 aWidth, u32 aHeight);

    YTE_Shared u32 GetHeight();
    YTE_Shared u32 GetWidth();

    // Does not change the Window width/height, just the internal representation
    // of the sizes, this is an escape hatch for editor mode.
    YTE_Shared void SetInternalDimensions(u32 aWidth, u32 aHeight);

    // Does not change the Window position, just the internal representation
    // of where the window is, this is an escape hatch for editor mode.
    YTE_Shared void SetInternalPosition(i32 aX, i32 aY);
    YTE_Shared glm::i32vec2 GetPosition();

    YTE_Shared std::any SetUpVulkanWindow(void *aSetup);

    YTE_Shared void SetCursorVisibility(bool aShow);

    YTE_Shared void SetWindowId(void *aId);
    YTE_Shared void* GetWindowId();

    YTE_Shared ~Window();

    YTE_Shared void PlatformUpdate();
    YTE_Shared void Update();

    Keyboard mKeyboard;
    Mouse mMouse;
    PrivateImplementationDynamic mData;
    Engine *mEngine;

    bool mFullscreen;
    bool Constructed = false;
    bool mShouldBeRenderedTo = false;

    int mRequestedWidth = 1920;
    int mRequestedHeight = 1080;

    std::string mSerializedWindowName;
    std::string mSerializedWindowIcon;
    std::string mSerializedCursorIcon;
    int mSerializedStartingWidth;
    int mSerializedStartingHeight;
    bool mSerializedStartingFullscreen = false;
    bool mSerializedShouldBeRenderedTo = true;

    std::string mName;
  };
}

#endif
