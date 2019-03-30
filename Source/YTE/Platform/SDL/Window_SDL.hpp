#pragma once

#include <string>

#include "SDL.h"

#include "YTE/Platform/PlatformManager.hpp"
#include "YTE/Platform/Window.hpp"

struct WindowData
{
  SDL_Window* mWindow;
  int mWindowId;
  std::string mWindowName;
  int mX;
  int mY;
  int mWidth;
  int mHeight;
  bool mMinimized;
  bool mFocus;
};

namespace YTE
{
  void WindowEventHandler(SDL_WindowEvent aEvent, PlatformManager* aManager, Window* aWindow);
}
