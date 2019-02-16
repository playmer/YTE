#include "SDL/include/SDL.h"

#include "YTE/Core/Engine.hpp"

#include "YTE/Platform/DialogBox.hpp"

#include "YTEMain/YTEMain.hpp"


int SDL_main(int aArgumentsSize, char** aArguments)
{
  YTE::InitializeYTETypes();

  ApplicationArguments arguments{ aArgumentsSize, aArguments };

  return YTE_Main(arguments);
}

