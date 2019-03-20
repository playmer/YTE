#pragma once

#include "SDL.h"

#include "YTE/Platform/Mouse.hpp"
#include "YTE/Platform/PlatformManager.hpp"

namespace YTE
{
    void SDLCALL PlatformManagerBlockedUpdate(void* aUserData, SDL_Window* aWindow);
}