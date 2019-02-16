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

#include "SDL.h"
#include "SDL_vulkan.h"

#include "YTE/Core/Engine.hpp"

#include "YTE/Platform/DialogBox.hpp"
#include "YTE/Platform/Keyboard.hpp"
#include "YTE/Platform/Window.hpp"


namespace YTE
{

  int GetPrimaryMonitor()
  {
    auto numOfDisplays = SDL_GetNumVideoDisplays();

    for (int i = 0; i < numOfDisplays; ++i)
    {
      SDL_Rect rect;
      
      if ((-1 != SDL_GetDisplayUsableBounds(i, &rect)) && 
          (0 == rect.x) && 
          (0 == rect.y))
      {
        return i;
      }
    }

    return -1;
  }

  struct WindowData
  {
    SDL_Window* mWindow;
    int mWindowId;
    std::string mWindowName;
    int mWidth;
    int mHeight;
    bool mMinimized;
    bool mFocus;
  };


  bool SetCursorView(bool aShow)
  {
    auto showing = SDL_ShowCursor(SDL_QUERY);
    SDL_ShowCursor(aShow);

    return showing;
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
    auto self = mData.Get<WindowData>();
    return self->mWindow;
  }

  void Window::SetWindowId(void *aId)
  {
    auto self = mData.Get<WindowData>();

    self->mWindow = SDL_CreateWindowFrom(aId);

    SDL_GetWindowSize(self->mWindow, &self->mWidth, &self->mHeight);

    mShouldBeRenderedTo = true;
  }

  Window::Window(Engine *aEngine) 
    : mEngine(aEngine)
  {
  }

  Window::Window(Engine *aEngine, RSValue *aProperties)
    : mEngine(aEngine)
  {
    Window *parent = nullptr;

    DeserializeByType(aProperties, this, TypeId<Window>());

    static std::string defaultName = "Yours Truly Engine";
    auto self = mData.ConstructAndGet<WindowData>();

    mShouldBeRenderedTo = mSerializedShouldBeRenderedTo;

    if (0 == mSerializedWindowName.size())
    {
      mSerializedWindowName = defaultName;
    }

    self->mWindowName = mSerializedWindowName;

    self->mWidth = mSerializedStartingWidth;
    self->mHeight = mSerializedStartingHeight;
    self->mMinimized = false;
    self->mFocus = true;

    SDL_Rect displayRect;
    auto primary = GetPrimaryMonitor();
    auto result = SDL_GetDisplayBounds(primary, &displayRect);


    UnusedArguments(result);
    DebugObjection(!result, "Couldn't retrieve monitor information.");
    

    auto width = displayRect.w;
    auto height = displayRect.h;
    auto right = displayRect.x;
    auto bottom = displayRect.y + displayRect.h;

    {
      width = mSerializedStartingWidth;
      height = mSerializedStartingHeight;

      self->mWidth = width;
      self->mHeight = height;

      mRequestedWidth = width;
      mRequestedHeight = height;
    }

    auto leftPos = (right / 2) - (width / 2);
    auto topPos = (bottom / 2) - (height / 2);

    self->mWindow = SDL_CreateWindow(
      self->mWindowName.c_str(),
      SDL_WINDOWPOS_UNDEFINED,
      SDL_WINDOWPOS_UNDEFINED,
      width,
      height,
      SDL_WINDOW_VULKAN
    );

    if (nullptr == self->mWindow)
    {
      return;
    }

    self->mWindowId = SDL_GetWindowID(self->mWindow);

    SetFullscreen(mSerializedStartingFullscreen, false);

    Constructed = true;
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
    auto self = mData.Get<WindowData>();
    SDL_SetWindowTitle(self->mWindow, aString);
    self->mWindowName = aString;
  }

  void Window::SetExtent(u32 aWidth, u32 aHeight)
  {
    auto self = mData.Get<WindowData>();

    self->mHeight = aHeight;
    self->mWidth = aWidth;
  }

  bool Window::IsMinimized()
  {
    auto self = mData.Get<WindowData>();
    return self->mMinimized;
  }

  bool Window::IsNotFocused()
  {
    auto self = mData.Get<WindowData>();
    return !self->mFocus;
  }

  bool Window::IsFocused()
  {
    auto self = mData.Get<WindowData>();
    return self->mFocus;
  }

  u32 Window::GetHeight()
  {
    auto self = mData.Get<WindowData>();
    return self->mHeight;
  }

  u32 Window::GetWidth()
  {
    auto self = mData.Get<WindowData>();
    return self->mWidth;
  }

  void Window::SetHeight(u32 aHeight)
  {
    auto self = mData.Get<WindowData>();
    self->mHeight = aHeight;
    //SDL_SetWindowSize(self->mWindow, self->mWidth, aHeight);
  }

  void Window::SetWidth(u32 aWidth)
  {
    auto self = mData.Get<WindowData>();
    self->mWidth = aWidth;
    //SDL_SetWindowSize(self->mWindow, aWidth, self->mHeight);
  }

  glm::i32vec2 Window::GetPosition()
  {
    return {};
  }

  std::any Window::SetUpVulkanWindow(void *aSetup)
  {
    auto self = mData.Get<WindowData>();

    auto instance = static_cast<vkhlf::Instance*>(aSetup);

    VkSurfaceKHR surface;
    SDL_Vulkan_CreateSurface(self->mWindow, static_cast<vk::Instance>(*instance), &surface);

    vk::SurfaceKHR surface2 = surface;
    return std::make_shared<vkhlf::Surface>(instance->shared_from_this(), surface2, nullptr);
    //instance->createSurface()

    //return instance->createSurface(self->mInstance, self->mWindowHandle);
  }
}
