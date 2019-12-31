#include <thread>

#include "YTE/Core/Engine.hpp"

#include "YTEMain/YTEMain.hpp"

using namespace std;

//int SDL_main(int, char*[])
int YTE_Main(ApplicationArguments&)
{
  YTE::Engine mainEngine{
    { 
      "../../../../../Assets/Bin/Config",
      "./Config"
    } 
  };
  
  mainEngine.Initialize();
  
  while (mainEngine.KeepRunning())
  {
    OPTICK_FRAME("MainThread");
    mainEngine.Update();
  }

  OPTICK_STOP_CAPTURE();
  OPTICK_SAVE_CAPTURE("capture.opt");

  return 0;
}