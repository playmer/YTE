#pragma once

#include "YTE/Platform/TargetDefinitions.hpp"

#if YTE_Windows
  #if defined(YTETools_Internal)
    #define YTETools_Shared __declspec( dllexport )
  #else
    #define YTETools_Shared __declspec( dllimport )
  #endif
#elif YTE_Linux
  #if defined(YTETools_Internal)
    #define YTETools_Shared __attribute__((visibility("default")))
  #else
    #define YTETools_Shared
  #endif
#endif
