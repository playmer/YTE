#pragma once

#include "SDL.h"

#include "YTE/Platform/Mouse.hpp"
#include "YTE/Platform/Window.hpp"

namespace YTE
{
  void MouseEventHandler(SDL_Event aEvent, Window* aWindow, Mouse* aMouse);
}