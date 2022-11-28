/******************************************************************************/
/*!
* \author Joshua T. Fisher
* \date   6/7/2015
*
* \copyright All content 2016 DigiPen (USA) Corporation, all rights reserved.
*/
/******************************************************************************/
#include "YTE/Platform/TargetDefinitions.hpp"

#include "YTE/Graphics/Vulkan/VkFunctionLoader.hpp"

#include <map>
#include <unordered_map>

#include "YTE/Core/Engine.hpp"

#include "YTE/Platform/DialogBox.hpp"
#include "YTE/Platform/Keyboard.hpp"
#include "YTE/Platform/Window.hpp"


namespace YTE
{
  bool SetCursorView(bool aShow)
  {
    return false;
  }


  ///////////////////////////////////////
  // Static Functions
  ///////////////////////////////////////
  i64 Window::MessageHandler(void* aWindowHandle, u64 aMessage, u64 aWParam, i64 aLParam, Window *aWindow)
  {
    return 0;
  }

  void* Window::GetWindowId()
  {
    return nullptr;
  }

  void Window::SetWindowId(void *aId)
  {
  }

  Window::Window(Engine *aEngine) 
    : mEngine(aEngine)
  {
  }

  Window::Window(Engine *aEngine, RSValue *aProperties)
    : mEngine(aEngine)
  {
  }

  Window::~Window()
  {
  }

  void Window::PlatformUpdate()
  {
    if (!mEngine)
    {
      return;
    }

    if (false == mEngine->IsEditor())
    {
    }
  }

  void Window::SetCursorVisibility(bool aShow)
  {
    SetCursorView(aShow);
  }

  //////////////////////////////////////////
  // Implementation mostly by Chromium (BSD)
  //////////////////////////////////////////
  void Window::SetResolution(u32 aWidth, u32 aHeight)
  {
    if (mEngine->IsEditor())
    {
      return;
    }
  }

  void Window::SetFullscreen(bool aFullscreen, bool aForMetro)
  {
  }

  void Window::SetWindowTitle(const char *aString)
  {
  }

  void Window::SetExtent(u32 aWidth, u32 aHeight)
  {
  }

  bool Window::IsMinimized()
  {
    return false;
  }

  bool Window::IsNotFocused()
  {
    return false;
  }

  bool Window::IsFocused()
  {
    return false;
  }

  u32 Window::GetHeight()
  {
    return 0;
  }

  u32 Window::GetWidth()
  {
    return 0;
  }

  void Window::SetHeight(u32 aHeight)
  {
  }

  void Window::SetWidth(u32 aWidth)
  {
  }

  glm::i32vec2 Window::GetPosition()
  {
    return {};
  }

  std::any Window::SetUpVulkanWindow(void *aSetup)
  {
    return {};
  }
}
