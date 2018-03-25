/******************************************************************************/
/*!
* \author Joshua T. Fisher
* \date   6/7/2015
*
* \copyright All content 2016 DigiPen (USA) Corporation, all rights reserved.
*/
/******************************************************************************/
#include "YTE/Platform/TargetDefinitions.hpp"

#ifdef YTE_Windows

//#include <shellapi.h>

#include "YTE/Graphics/Vulkan/VkFunctionLoader.hpp"

#include <YTE/Platform/Windows/WindowsInclude_Windows.hpp>
#include <Winuser.h>

#if defined(MemoryBarrier)
#undef MemoryBarrier
#endif

#include <map>
#include <unordered_map>

#include "YTE/Core/Engine.hpp"

#include "YTE/Platform/DialogBox.hpp"
#include "YTE/Platform/Keyboard.hpp"
#include "YTE/Platform/Window.hpp"


namespace YTE
{
  struct WindowData
  {
    WNDCLASS mWindowsData;
    std::string mWindowName;
    HWND mWindowHandle;
    HDC mDeviceContext;
    HINSTANCE mInstance;
    int mWidth;
    int mHeight;
    bool mMinimized;
    bool mFocus;
  };

  bool SetCursorView(bool aShow)
  {
    CURSORINFO cursorinfo;
    GetCursorInfo(&cursorinfo);

    bool previous = true;

    if (cursorinfo.flags == 0)
    {
      previous = false;
    }

    if (aShow)
    {
      int counter = ShowCursor(TRUE);
      while (counter <= -1)
      {
        counter = ShowCursor(TRUE);
      }
    }
    else
    {
      int counter = ShowCursor(FALSE);
      while (counter > -1)
      {
        counter = ShowCursor(FALSE);
      }
    }

    return previous;
  }


  static std::map<HWND, int> *cFullScreenWindows;

  ///////////////////////////////////////
  // class by Chromium (BSD)
  ///////////////////////////////////////
  // Scoping class that ensures a HWND remains hidden while it enters or leaves
  // the fullscreen state. This reduces some flicker-jank that an application UI
  // might suffer.
  class ScopedFullscreenVisibility
  {
  public:
    explicit ScopedFullscreenVisibility(HWND aHandle);

    ~ScopedFullscreenVisibility();

    // Returns true if |hwnd| is currently hidden due to instance(s) of this
    // class.
    static bool IsHiddenForFullscreen(HWND hwnd);

  private:
    HWND mHandle;
  };


  ScopedFullscreenVisibility::ScopedFullscreenVisibility(HWND aHandle)
    : mHandle(aHandle)
  {
    if (cFullScreenWindows == nullptr)
    {
      cFullScreenWindows = new std::map<HWND, int>();
    }

    auto iterator = cFullScreenWindows->find(mHandle);

    if (iterator != cFullScreenWindows->end())
    {
      iterator->second++;
    }
    else
    {
      cFullScreenWindows->insert(std::make_pair(mHandle, (int)1));

      // NOTE: Be careful not to activate any windows here (for example, calling
      // ShowWindow(SW_HIDE) will automatically activate another window).  This
      // code can be called while a window is being deactivated, and activating
      // another window will screw up the activation that is already in progress.
      SetWindowPos(mHandle,
                   NULL,
                   0,
                   0,
                   0,
                   0,
                   SWP_HIDEWINDOW | SWP_NOACTIVATE | SWP_NOMOVE |
                   SWP_NOREPOSITION | SWP_NOSIZE | SWP_NOZORDER);
    }
  }

  ScopedFullscreenVisibility::~ScopedFullscreenVisibility()
  {
    std::map<HWND, int>::iterator it = cFullScreenWindows->find(mHandle);

    DebugObjection((it == cFullScreenWindows->end()),
                   "Destructing window doesn't exist!");

    if (--it->second == 0)
    {
      cFullScreenWindows->erase(it);
      ShowWindow(mHandle, SW_SHOW);
    }

    if (cFullScreenWindows->empty())
    {
      delete cFullScreenWindows;
      cFullScreenWindows = NULL;
    }
  }

  // Returns true if |hwnd| is currently hidden due to instance(s) of this
  // class.
  bool ScopedFullscreenVisibility::IsHiddenForFullscreen(HWND hwnd)
  {
    if (!cFullScreenWindows)
    {
      return false;
    }

    return cFullScreenWindows->find(hwnd) != cFullScreenWindows->end();
  }


  ///////////////////////////////////////
  // Static Functions
  ///////////////////////////////////////
  // Should change this to be an IntegerVector2D
  glm::i32vec2 PositionFromLParam(LPARAM lParam)
  {
    // Systems with multiple monitors can have negative x and y coordinates
    return glm::vec2((int)(short)LOWORD(lParam), (int)(short)HIWORD(lParam));
  }

  MouseButtons XButtonFromWParam(WPARAM aButton)
  {
    if (HIWORD(aButton) == XBUTTON1)
    {
      return MouseButtons::Back;
    }
    else
    {
      return MouseButtons::Forward;
    }
  }

  // Should change this to be an IntegerVector2D
  glm::i32vec2 LocalScreenToClient(HWND aWindowHandle, LPARAM windowPosition)
  {
    glm::i32vec2 screenPosition = PositionFromLParam(windowPosition);

    POINT point;
    point.x = screenPosition.x;
    point.y = screenPosition.y;

    ::ScreenToClient(aWindowHandle, &point);

    glm::i32vec2 localPosition;


    localPosition.x = point.x;
    localPosition.y = point.y;

    return localPosition;
  }

  inline Window *GetWindowPointer(HWND aWindowHandle)
  {
    return reinterpret_cast<Window *>(GetWindowLongPtr(aWindowHandle, GWLP_USERDATA));
  }

  inline void SetWindowPointer(HWND aWindowHandle, Window *aWindow)
  {
    SetWindowLongPtr(aWindowHandle, GWLP_USERDATA, reinterpret_cast<LONG_PTR>(aWindow));
  }

  static LRESULT WindowsMessageHandler(HWND aWindowHandle, UINT aMessage, WPARAM aWParam, LPARAM aLParam, Window *aWindow)
  {
    if (aWindow == nullptr && WM_CREATE == aMessage)
    {
      aWindow = (Window*)((CREATESTRUCT*)(aLParam))->lpCreateParams;
    }

    WindowData *self = nullptr;

    if (nullptr != aWindow)
    {
      self = aWindow->mData.Get<WindowData>();
    }

    switch (aMessage)
    {
      // Window Creation
    case WM_CREATE:
    {
      // Set it on the user data section of the window
      SetWindowPointer(aWindowHandle, aWindow);

      aWindow->mData.Get<WindowData>()->mWindowHandle = aWindowHandle;
      break;
    }

    // Mouse Button was pressed.
    case WM_LBUTTONDOWN:
      aWindow->mMouse.UpdateButton(MouseButtons::Left, true, PositionFromLParam(aLParam));
      break;
    case WM_RBUTTONDOWN:
      aWindow->mMouse.UpdateButton(MouseButtons::Right, true, PositionFromLParam(aLParam));
      break;
    case WM_MBUTTONDOWN:
      aWindow->mMouse.UpdateButton(MouseButtons::Middle, true, PositionFromLParam(aLParam));
      break;
    case WM_XBUTTONDOWN:
      aWindow->mMouse.UpdateButton(XButtonFromWParam(aWParam), true, PositionFromLParam(aLParam));
      break;

      // Mouse Button was released.
    case WM_LBUTTONUP:
      aWindow->mMouse.UpdateButton(MouseButtons::Left, false, PositionFromLParam(aLParam));
      break;
    case WM_RBUTTONUP:
      aWindow->mMouse.UpdateButton(MouseButtons::Right, false, PositionFromLParam(aLParam));
      break;
    case WM_MBUTTONUP:
      aWindow->mMouse.UpdateButton(MouseButtons::Middle, false, PositionFromLParam(aLParam));
      break;
    case WM_XBUTTONUP:
      aWindow->mMouse.UpdateButton(XButtonFromWParam(aWParam), false, PositionFromLParam(aLParam));
      break;

      // A key has been pressed.
    case WM_KEYDOWN:
    case WM_SYSKEYDOWN:
      aWindow->mKeyboard.UpdateKey(TranslateFromOsToOurKey(aWParam), true);
      break;

      // A key has been released.
    case WM_KEYUP:
    case WM_SYSKEYUP:
      aWindow->mKeyboard.UpdateKey(TranslateFromOsToOurKey(aWParam), false);
      break;

      // Should do mouse trapping
      // Only activates when mouse is over window.
    case WM_MOUSEMOVE:
    {
      aWindow->mMouse.UpdatePosition(PositionFromLParam(aLParam));
      break;
    }

    // Vertical Scroll
    case WM_MOUSEWHEEL:
    {
      glm::i32vec2 mousePosition = LocalScreenToClient(aWindowHandle, aLParam);

      int scrollAmount = GET_WHEEL_DELTA_WPARAM(aWParam);

      glm::vec2 scrollMove;
      scrollMove.y = scrollAmount / (float)WHEEL_DELTA;

      aWindow->mMouse.UpdateWheel(scrollMove, mousePosition);
      break;
    }

    // Horizontal Scroll
    case WM_MOUSEHWHEEL:
    {
      glm::i32vec2 mousePosition = LocalScreenToClient(aWindowHandle, aLParam);

      int scrollAmount = GET_WHEEL_DELTA_WPARAM(aWParam);

      glm::vec2 scrollMove;
      scrollMove.x = scrollAmount / (float)WHEEL_DELTA;

      aWindow->mMouse.UpdateWheel(scrollMove, mousePosition);
      break;
    }

    case WM_SIZE:
    {
      if (true == aWindow->mEngine->IsEditor())
      {
        break;
      }

      if (aWParam == SIZE_MINIMIZED)
      {
        self->mMinimized = true;

        WindowMinimizedOrRestored minimizeEvent;
        minimizeEvent.Minimized = true;
        aWindow->SendEvent(Events::WindowMinimizedOrRestored, &minimizeEvent);
      }
      else if (aWParam == SIZE_RESTORED)
      {
        if (self->mMinimized == true)
        {
          self->mMinimized = false;

          WindowMinimizedOrRestored minimizeEvent;
          minimizeEvent.Minimized = false;
          aWindow->SendEvent(Events::WindowMinimizedOrRestored, &minimizeEvent);
        }

        RECT windowDimensions;
        GetClientRect(aWindowHandle, &windowDimensions);
        WindowResize resizeEvent;
        resizeEvent.width = windowDimensions.right;
        resizeEvent.height = windowDimensions.bottom;

        self->mWidth = resizeEvent.width;
        self->mHeight = resizeEvent.height;

        aWindow->SendEvent(Events::WindowResize, &resizeEvent);
      }

      break;
    }

    case WM_ACTIVATE:
    {
      if (aWParam != WA_INACTIVE)
      {
        WindowFocusLostOrGained focusEvent;
        focusEvent.Focused = true;
        self->mFocus = true;

        aWindow->SendEvent(Events::WindowFocusLostOrGained, &focusEvent);

        break;
      }
    }
    case WM_KILLFOCUS:
    {
      WindowFocusLostOrGained focusEvent;
      focusEvent.Focused = false;
      self->mFocus = false;

      aWindow->mKeyboard.ForceAllKeysUp();
      aWindow->SendEvent(Events::WindowFocusLostOrGained, &focusEvent);

      break;
    }

    // Should probably make sure DefWindowProc doesn't get called here. Return?
    case WM_CLOSE:
    case WM_DESTROY:

      // TODO (Josh): Fix multiple windows.
      //if (window->mEngine->GetWindows().size() == 1)
      //{
      //  window->mEngine->EndExecution();
      //}
      //else
      //{
      //  window->mEngine.GetWindows().find()
      //}

      if (aWindow)
      {
        aWindow->mEngine->EndExecution();
      }

      SetWindowPointer(aWindowHandle, nullptr);
      break;

    case WM_NCCREATE:
      return DefWindowProc(aWindowHandle, aMessage, aWParam, aLParam);

    default:
      break;
    }

    if ((nullptr != aWindow) && (aWindow->mEngine) && (false == aWindow->mEngine->IsEditor()))
    {
      // Probably want to only do this sometimes not every time we pump.
      return DefWindowProc(aWindowHandle, aMessage, aWParam, aLParam);
    }
    else
    {
      return 0;
    }
  }

  static LRESULT CALLBACK WindowsMessagePump(HWND aWindowHandle, UINT aMessage, WPARAM aWParam, LPARAM aLParam)
  {
    Window *window = GetWindowPointer(aWindowHandle);
    return WindowsMessageHandler(aWindowHandle, aMessage, aWParam, aLParam, window);
  }

  i64 Window::MessageHandler(void* aWindowHandle, u64 aMessage, u64 aWParam, i64 aLParam, Window *aWindow)
  {
    return WindowsMessageHandler(static_cast<HWND>(aWindowHandle), 
                                 static_cast<UINT>(aMessage), 
                                 aWParam, 
                                 aLParam, 
                                 aWindow);
  }

  HMONITOR GetPrimaryMonitor()
  {
    POINT ptZero = { 0, 0 };
    return MonitorFromPoint(ptZero, MONITOR_DEFAULTTOPRIMARY);
  }


  Window::Window(Engine *aEngine) : mEngine(aEngine)
  {
    mData.ConstructAndGet<WindowData>();
  }

  void* Window::GetWindowId()
  {
    auto self = mData.Get<WindowData>();

    return self->mWindowHandle;
  }

  void Window::SetWindowId(void *aId)
  {
    auto self = mData.Get<WindowData>();

    self->mWindowHandle = static_cast<HWND>(aId);
    self->mWindowsData.hInstance = GetModuleHandle(nullptr);


    RECT clientSize;
    GetClientRect(self->mWindowHandle, &clientSize);

    self->mHeight = clientSize.bottom - clientSize.top;
    self->mWidth = clientSize.right - clientSize.left;

    mShouldBeRenderedTo = true;
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
    self->mWindowsData.style = CS_HREDRAW | CS_VREDRAW;
    //self->mWindowsData.style = WS_OVERLAPPED | WS_CAPTION | WS_SYSMENU | WS_MINIMIZEBOX;
    self->mWindowsData.lpfnWndProc = WindowsMessagePump;
    self->mWindowsData.cbClsExtra = 0;
    self->mWindowsData.cbWndExtra = 0;


    // This needs to be changed for multiple windows to work, I think.
    self->mWindowsData.hInstance = GetModuleHandle(nullptr);

    self->mWindowsData.hIcon = LoadIcon(NULL, 0 != mSerializedWindowIcon.size() ?
      mSerializedWindowIcon.c_str() :
      IDI_APPLICATION);

    self->mWindowsData.hCursor = LoadCursor(NULL, 0 != mSerializedCursorIcon.size() ?
      mSerializedCursorIcon.c_str() :
      IDC_ARROW);

    self->mWindowsData.hbrBackground = (HBRUSH)GetStockObject(BLACK_BRUSH);
    self->mWindowsData.lpszMenuName = self->mWindowName.c_str();
    self->mWindowsData.lpszClassName = TEXT("YTE_GAME");

    self->mWidth = mSerializedStartingWidth;
    self->mHeight = mSerializedStartingHeight;
    self->mMinimized = false;
    self->mFocus = true;

    auto atom = RegisterClass(&self->mWindowsData);

    HMONITOR monitorHandle = GetPrimaryMonitor();

    MONITORINFO monitorInformation = { sizeof(MONITORINFO) };

    auto result = GetMonitorInfo(monitorHandle, &monitorInformation);
    YTEUnusedArgument(result);
    DebugObjection(!result, "Couldn't retrieve monitor information.");

    auto right = monitorInformation.rcMonitor.right;
    auto bottom = monitorInformation.rcMonitor.bottom;
    auto width = monitorInformation.rcMonitor.right - monitorInformation.rcMonitor.left;
    auto height = monitorInformation.rcMonitor.bottom - monitorInformation.rcMonitor.top;

    if (false == mSerializedStartingFullscreen)
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

    self->mWindowHandle =
      CreateWindow(MAKEINTATOM(atom),
        self->mWindowsData.lpszMenuName,
        WS_POPUP | WS_VISIBLE,
        leftPos,
        topPos,
        width,
        height,
        parent ? parent->mData.Get<WindowData>()->mWindowHandle : nullptr,
        NULL,
        self->mWindowsData.hInstance,
        this);

    ShowWindow(self->mWindowHandle, SW_SHOWNORMAL);
    UpdateWindow(self->mWindowHandle);

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
      MSG message;
      while (PeekMessage(&message, NULL, 0, 0, PM_REMOVE))
      {
        TranslateMessage(&message);
        DispatchMessage(&message);
      }
    }
  }

  void Window::SetCursorVisibility(bool aShow)
  {
    SetCursorView(aShow);
  }

  //////////////////////////////////////////
  // Implementation mostly by Chromium (BSD)
  //////////////////////////////////////////
  void Window::SetFullscreen(bool aFullscreen, bool aForMetro)
  {
    YTEUnusedArgument(aForMetro);

    auto self = mData.Get<WindowData>();
    ScopedFullscreenVisibility visibility(self->mWindowHandle);

    mFullscreen = aFullscreen;

    if (mFullscreen)
    {
      // Set new window style and size.
      SetWindowLong(self->mWindowHandle,
        GWL_STYLE,
        WS_POPUP | WS_VISIBLE);

      // On expand, if we're given a window_rect, grow to it, otherwise do
      // not resize.
      MONITORINFO monitorInformation;
      monitorInformation.cbSize = sizeof(MONITORINFO);
      GetMonitorInfo(MonitorFromWindow(self->mWindowHandle, MONITOR_DEFAULTTOPRIMARY),
        &monitorInformation);

      RECT window_rect(monitorInformation.rcMonitor);

      SetWindowPos(self->mWindowHandle,
        NULL,
        monitorInformation.rcMonitor.left,
        monitorInformation.rcMonitor.top,
        monitorInformation.rcMonitor.right - monitorInformation.rcMonitor.left,
        monitorInformation.rcMonitor.bottom - monitorInformation.rcMonitor.top,
        SWP_NOZORDER | SWP_NOACTIVATE | SWP_FRAMECHANGED);
    }
    else
    {
      // Reset original window style and size.  The multiple window size/moves
      // here are ugly, but if SetWindowPos() doesn't redraw, the taskbar won't be
      // repainted.  Better-looking methods welcome.
      SetWindowLong(self->mWindowHandle, GWL_STYLE, CS_HREDRAW |
        CS_VREDRAW |
        WS_OVERLAPPED |
        WS_SYSMENU |
        WS_MINIMIZEBOX |
        WS_CAPTION);

      // The window was sized to the values we want for the client area.
      // Add the difference between the two to grow the client area correctly,
      // unless the windows border size changes based on window size or 
      // something but thats weird so I'm not going to deal with that.
      RECT clientSize;
      GetClientRect(self->mWindowHandle, &clientSize);

      // TODO (Josh): Fix resolution being set at 720p always.
      // Keep the previous position (more or less)
      RECT forPosition;
      GetWindowRect(self->mWindowHandle, &forPosition);
      int differenceX = mRequestedWidth - clientSize.right;
      int differenceY = mRequestedHeight - clientSize.bottom;

      SetWindowPos(self->mWindowHandle,
        NULL,
        forPosition.left - differenceX / 2,
        forPosition.top - differenceY / 2,
        mRequestedWidth,
        mRequestedHeight,
        SWP_NOZORDER | SWP_NOACTIVATE | SWP_FRAMECHANGED);
    }

    UpdateWindow(self->mWindowHandle);
  }

  void Window::SetWindowTitle(const char *aString)
  {
    auto self = mData.Get<WindowData>();
    SetWindowText(self->mWindowHandle, aString);
  }

  void Window::SetExtent(u32 aHeight, u32 aWidth)
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
  }

  void Window::SetWidth(u32 aWidth)
  {
    auto self = mData.Get<WindowData>();
    self->mWidth = aWidth;
  }

  glm::i32vec2 Window::GetPosition()
  {
    auto self = mData.Get<WindowData>();

    glm::i32vec2 toReturn;
    RECT rect;
    if (GetWindowRect(self->mWindowHandle, &rect))
    {
      toReturn.x = rect.left;
      toReturn.y = rect.top;
    }

    return toReturn;
  }

  std::shared_ptr<vkhlf::Surface> Window::SetUpVulkanWindow(void *aSetup)
  {
    auto self = mData.Get<WindowData>();

    auto instance = static_cast<vkhlf::Instance*>(aSetup);

    return instance->createSurface(self->mInstance, self->mWindowHandle);
  }
}
#endif