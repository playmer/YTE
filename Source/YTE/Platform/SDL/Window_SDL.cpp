#include "YTE/Platform/TargetDefinitions.hpp"

#include "YTE/Graphics/Vulkan/VkFunctionLoader.hpp"

#include <map>
#include <unordered_map>

#include "SDL.h"
#include "SDL_vulkan.h"

#include "YTE/Core/Engine.hpp"

#include "YTE/Platform/SDL/PlatformManager_SDL.hpp"
#include "YTE/Platform/SDL/Window_SDL.hpp"

#include "YTE/Platform/DialogBox.hpp"
#include "YTE/Platform/Keyboard.hpp"
#include "YTE/Platform/Window.hpp"


namespace YTE
{
  namespace Detail
  {
    void ChangeSDLWindowFlags(SDL_Window* aWindow);
  }

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


  bool SetCursorView(bool aShow)
  {
    auto showing = SDL_ShowCursor(SDL_QUERY);
    SDL_ShowCursor(aShow);
    
    return showing;
  }


  i64 Window::MessageHandler(void*, u64, u64, i64, Window*)
  {
    return 0;
  }

  void DoResize(Window* aWindow)
  {
    OPTICK_EVENT();

    if (true == aWindow->mEngine->IsEditor())
    {
      return;
    }
    
    auto self = aWindow->mData.Get<WindowData>();

    int width;
    int height;
    SDL_GetWindowSize(self->mWindow, &width, &height);

    WindowResize resizeEvent;
    resizeEvent.width = width;
    resizeEvent.height = height;

    self->mWidth = resizeEvent.width;
    self->mHeight = resizeEvent.height;

    aWindow->SendEvent(Events::WindowResize, &resizeEvent);
  }
  
  void WindowEventHandler(SDL_WindowEvent aEvent, PlatformManager* aManager, Window* aWindow)
  {
    WindowData *self = nullptr;

    if (nullptr != aWindow)
    {
      self = aWindow->mData.Get<WindowData>();
    }

    switch (aEvent.event)
    {
        // Window has been shown
      case SDL_WINDOWEVENT_SHOWN:
      {
        break;
      }
      // Window has been hidden
      case SDL_WINDOWEVENT_HIDDEN:
      {
        break;
      }
      // Window has been exposed and should be redrawn
      case SDL_WINDOWEVENT_EXPOSED:
      {
        break;
      }
      // Window has been moved to data1, data2
      case SDL_WINDOWEVENT_MOVED:
      {
        if (true == aWindow->mEngine->IsEditor())
        {
          break;
        }

        self->mX = aEvent.data1;
        self->mY = aEvent.data2;

        break;
      }
      // Window has been resized to data1xdata2
      case SDL_WINDOWEVENT_RESIZED:
      // The window size has changed, either as a result of an API call or through the system or user changing the window size.
      case SDL_WINDOWEVENT_SIZE_CHANGED:
      {
        DoResize(aWindow);
        break;
      }
      // Window has been minimized
      case SDL_WINDOWEVENT_MINIMIZED:
      {
        if (true == aWindow->mEngine->IsEditor())
        {
          break;
        }

        self->mMinimized = true;

        WindowMinimizedOrRestored minimizeEvent;
        minimizeEvent.Minimized = true;
        aWindow->SendEvent(Events::WindowMinimizedOrRestored, &minimizeEvent);
        break;
      }
      // Window has been maximized
      case SDL_WINDOWEVENT_MAXIMIZED:
      {
        break;
      }
      // Window has been restored to normal size and position
      case SDL_WINDOWEVENT_RESTORED:
      {
        if (true == aWindow->mEngine->IsEditor())
        {
          break;
        }

        if (self->mMinimized == true)
        {
          self->mMinimized = false;

          WindowMinimizedOrRestored minimizeEvent;
          minimizeEvent.Minimized = false;
          aWindow->SendEvent(Events::WindowMinimizedOrRestored, &minimizeEvent);
        }

        DoResize(aWindow);
        break;
      }
      // Window has gained mouse focus
      case SDL_WINDOWEVENT_ENTER:
      {
        aManager->SetMouseFocusedWindow(aWindow);
        break;
      }
      // Window has lost mouse focus
      case SDL_WINDOWEVENT_LEAVE:
      {
        aManager->SetMouseFocusedWindow(nullptr);
        break;
      }
      // Window has gained keyboard focus
      case SDL_WINDOWEVENT_FOCUS_GAINED:
      {
        aManager->SetKeyboardFocusedWindow(aWindow);

        WindowFocusLostOrGained focusEvent;
        focusEvent.Focused = true;
        self->mFocus = true;

        aWindow->SendEvent(Events::WindowFocusLostOrGained, &focusEvent);
        break;
      }
      // Window has lost keyboard focus
      case SDL_WINDOWEVENT_FOCUS_LOST:
      {
        aManager->SetKeyboardFocusedWindow(nullptr);

        WindowFocusLostOrGained focusEvent;
        focusEvent.Focused = false;
        self->mFocus = false;

        aWindow->mKeyboard.ForceAllKeysUp();
        aWindow->SendEvent(Events::WindowFocusLostOrGained, &focusEvent);
        break;
      }
      // The window manager requests that the window be closed
      case SDL_WINDOWEVENT_CLOSE:
      {
        aWindow->mEngine->EndExecution();
        break;
      }
      // Window is being offered a focus (should SetWindowInputFocus() on itself or a subwindow, or ignore)
      case SDL_WINDOWEVENT_TAKE_FOCUS:
      {
        break;
      }
      // Window had a hit test that wasn't SDL_HITTEST_NORMAL.
      case SDL_WINDOWEVENT_HIT_TEST:
      {
        break;
      }
    }
  }



  Window::Window(Engine* aEngine)
    : mMouse{ this }
    , mEngine{ aEngine }
  {
    mData.ConstructAndGet<WindowData>();
  }

  Window::Window(Engine* aEngine, RSValue* aProperties)
    : mMouse{ this }
    , mEngine { aEngine }
  {
    OPTICK_EVENT();

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
    DebugAssert(result >= 0, "Couldn't retrieve monitor information.");
    

    auto width = displayRect.w;
    auto height = displayRect.h;

    {
      width = mSerializedStartingWidth;
      height = mSerializedStartingHeight;

      self->mWidth = width;
      self->mHeight = height;

      mRequestedWidth = width;
      mRequestedHeight = height;
    }

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
    
    // Despite this not being a window related function, SDL will just...unload this library if
    // a Vulkan related Window is destroyed, which causes issues with future Windows, so we need
    // to load it every time...just in case.
    SDL_Vulkan_LoadLibrary(nullptr);

    SDL_SetWindowResizable(self->mWindow, SDL_TRUE);

    SDL_SetWindowData(self->mWindow, "YTE_Window", this);

    int x, y;
    SDL_GetWindowPosition(self->mWindow, &x, &y);
    SetInternalPosition(x, y);

    self->mWindowId = SDL_GetWindowID(self->mWindow);

    SetFullscreen(mSerializedStartingFullscreen);
    SDL_SetWindowBlockerData(self->mWindow, &PlatformManagerBlockedUpdate, aEngine->GetPlatformManager());

    Constructed = true;
  }

  Window::~Window()
  {
    auto self = mData.Get<WindowData>();
    SDL_DestroyWindow(self->mWindow);
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

  void* Window::GetWindowId()
  {
    auto self = mData.Get<WindowData>();
    return self->mWindow;
  }

  void Window::SetWindowId(void* aId)
  {
    OPTICK_EVENT();

    auto self = mData.Get<WindowData>();

    self->mWindow = SDL_CreateWindowFrom(aId);
    
    // Despite this not being a window related function, SDL will just...unload this library if
    // a Vulkan related Window is destroyed, which causes issues with future Windows, so we need
    // to load it every time...just in case.
    SDL_Vulkan_LoadLibrary(nullptr);
    Detail::ChangeSDLWindowFlags(self->mWindow);
    SDL_SetWindowData(self->mWindow, "YTE_Window", this);

    SDL_GetWindowSize(self->mWindow, &self->mWidth, &self->mHeight);
    
    self->mWindowId = SDL_GetWindowID(self->mWindow);

    mShouldBeRenderedTo = true;
  }

  void Window::SetCursorVisibility(bool aShow)
  {
    SetCursorView(aShow);
  }

  void Window::SetResolution(u32 aWidth, u32 aHeight)
  {
    if (mEngine->IsEditor())
    {
      return;
    }

    auto self = mData.Get<WindowData>();

    SDL_SetWindowSize(self->mWindow, static_cast<int>(aWidth), static_cast<int>(aHeight));
  }

  void Window::SetFullscreen(bool aFullscreen)
  {
    auto self = mData.Get<WindowData>();
    
    mFullscreen = aFullscreen;
    int fullscreenMode = 0;
    
    if (aFullscreen)
    {
      fullscreenMode = SDL_WINDOW_FULLSCREEN_DESKTOP;
    }
    
    SDL_SetWindowFullscreen(self->mWindow, fullscreenMode);
  }
  
  void Window::SetWindowTitle(char const* aString)
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

  void Window::SetInternalDimensions(u32 aWidth, u32 aHeight)
  {
    auto self = mData.Get<WindowData>();
    self->mWidth = aWidth;
    self->mHeight = aHeight;
  }

  void Window::SetInternalPosition(i32 aX, i32 aY)
  {
    auto self = mData.Get<WindowData>();

    self->mX = aX;
    self->mY = aY;
  }

  glm::i32vec2 Window::GetPosition()
  {
    auto self = mData.Get<WindowData>();

    return{ self->mX, self->mY };
  }

  std::any Window::SetUpVulkanWindow(void* aSetup)
  {
    OPTICK_EVENT();

    auto self = mData.Get<WindowData>();

    auto instance = static_cast<vkhlf::Instance*>(aSetup);

    VkSurfaceKHR surface;
    auto result = SDL_Vulkan_CreateSurface(self->mWindow, static_cast<vk::Instance>(*instance), &surface);

    printf(result ? "true\n" : "false\n");

    vk::SurfaceKHR surface2 = surface;
    return std::make_shared<vkhlf::Surface>(instance->shared_from_this(), surface2, nullptr);
    //instance->createSurface()

    //return instance->createSurface(self->mInstance, self->mWindowHandle);
  }
}
