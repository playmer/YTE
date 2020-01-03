#include "SDL.h"
#include "SDL_vulkan.h"

#include "YTE/Core/Engine.hpp"

#include "YTE/Platform/DialogBox.hpp"

#include "YTEMain/YTEMain.hpp"


bool InitializeSDL()
{
  OPTICK_EVENT();
  if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_AUDIO | SDL_INIT_GAMECONTROLLER | SDL_INIT_HAPTIC) != 0) {
    SDL_Log("Unable to initialize SDL: %s", SDL_GetError());
    return static_cast<bool>(1);
  }

  SDL_GameControllerEventState(SDL_QUERY);
}

void InitializeYTETypes()
{
  OPTICK_EVENT();
  YTE::InitializeYTETypes();
}

int SDL_main(int aArgumentsSize, char** aArguments)
{
  //OPTICK_START_CAPTURE();
  //OPTICK_THREAD("MainThread");
  
  if (!InitializeSDL())
  {
    return 1;
  }

  InitializeYTETypes();

  ApplicationArguments arguments{ aArgumentsSize, aArguments };

  auto toReturn = YTE_Main(arguments);

  SDL_Quit();

  return toReturn;
}

