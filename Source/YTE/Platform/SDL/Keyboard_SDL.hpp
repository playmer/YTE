#pragma once

#include "SDL.h"

#include "YTE/Platform/Keyboard.hpp"
#include "YTE/Platform/Window.hpp"

namespace YTE
{
  void KeyboardEventHandler(SDL_Event aEvent, Window* aWindow, Keyboard* aKeyboard);
}