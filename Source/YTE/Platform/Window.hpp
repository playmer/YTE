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

#include <string>

#include "YTE/Core/EventHandler.hpp"
#include "YTE/Core/ForwardDeclarations.hpp"
#include "YTE/Core/PrivateImplementation.hpp"

#include "YTE/Platform/Keyboard.hpp"
#include "YTE/Platform/Mouse.hpp"
#include "YTE/Platform/TargetDefinitions.hpp"

#include "YTE/Graphics/ForwardDeclarations.hpp"
#include "YTE/Graphics/Vulkan/VkFunctionLoader.hpp"

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

  bool SetCursorView(bool aShow);
    
  class Window : public EventHandler
  {
  public:
    YTEDeclareType(Window);

    static i64 MessageHandler(void* aWindowHandle, u64 aMessage, u64 aWParam, i64 aLParam, Window *aWindow);
 
    Window(Engine *aEngine, RSValue *aProperties);
    Window(Engine *aEngine);
      
    void SetFullscreen(bool aFullscreen, bool aMetro);
    void SetWindowTitle(const char *aString);

    bool IsMinimized();
    bool IsNotFocused();

    void SetExtent(u32 aHeight, u32 aWidth);

    u32 GetHeight();
    u32 GetWidth();

    std::shared_ptr<vkhlf::Surface> SetUpVulkanWindow(void *aSetup);

    void SetCursorVisibility(bool aShow);

    void SetWindowId(void *aId);

    ~Window();

    void Update();

    Keyboard mKeyboard;
    Mouse mMouse;
    PrivateImplementationDynamic mData;
    Engine *mEngine;

    bool mFullscreen;
    bool Constructed = false;
    bool mShouldBeRenderedTo = false;


    int mRequestedWidth;
    int mRequestedHeight;

    std::string mSerializedWindowName;
    std::string mSerializedWindowIcon;
    std::string mSerializedCursorIcon;
    int mSerializedStartingWidth;
    int mSerializedStartingHeight;
    bool mSerializedStartingFullscreen;
    bool mSerializedShouldBeRenderedTo;
  };
}

#endif