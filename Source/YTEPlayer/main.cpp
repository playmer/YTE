/******************************************************************************/
/*!
 * \author Joshua T. Fisher
 * \date   2015-10-26
 *
 * \copyright All content 2016 DigiPen (USA) Corporation, all rights reserved.
 */
/******************************************************************************/
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