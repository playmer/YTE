#include "SDL.h"
#include "SDL_vulkan.h"

#include "YTE/Core/Engine.hpp"

#include "YTE/Platform/DialogBox.hpp"

#include "YTEMain/YTEMain.hpp"


int SDL_main(int aArgumentsSize, char** aArguments)
{
  if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_AUDIO) != 0) {
    SDL_Log("Unable to initialize SDL: %s", SDL_GetError());
    return 1;
  }

  SDL_Vulkan_LoadLibrary(nullptr);

  YTE::InitializeYTETypes();

  ApplicationArguments arguments{ aArgumentsSize, aArguments };

  auto toReturn = YTE_Main(arguments);

  SDL_Quit();

  return toReturn;
}

