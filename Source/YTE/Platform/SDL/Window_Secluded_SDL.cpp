
#include "SDL/src/video/SDL_sysvideo.h"

namespace YTE::Detail
{
  void ChangeSDLWindowFlags(SDL_Window* aWindow)
  {
    aWindow->flags |= SDL_WINDOW_VULKAN;
  }
}