#include <thread>

#include "YTE/Core/Engine.hpp"

#include "YTEMain/YTEMain.hpp"

using namespace std;

//int SDL_main(int, char*[])
int YTE_Main(ApplicationArguments&)
{
  //if constexpr (YTE_CAN_PROFILE)
  //{
  //  EASY_PROFILER_ENABLE;
  //  profiler::startListen();
  //}
  //
  //YTEProfileFunction();

  using namespace std::chrono_literals;
  std::this_thread::sleep_for(7s);
  
  YTE::Engine mainEngine{
    { 
      "../../../../../Assets/Bin/Config",
      "./Config"
    } 
  };
  
  mainEngine.Initialize();
  
  while (mainEngine.KeepRunning())
  {
    mainEngine.Update();
  }

  return 0;
}