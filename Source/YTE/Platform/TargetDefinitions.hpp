#pragma once

#include <type_traits>

#if YTE_Windows
  #if defined(YTE_Internal)
    #define YTE_Shared __declspec( dllexport )
  #else
    #define YTE_Shared __declspec( dllimport )
  #endif
#elif YTE_Linux
  #if defined(YTE_Internal)
    #define YTE_Shared __attribute__((visibility("default")))
  #else
    #define YTE_Shared
  #endif
#endif

namespace YTE
{
  struct PlatformInformation
  {
    #if YTE_Windows
      using Windows = std::integral_constant<bool, true>;
      using Linux = std::integral_constant<bool, false>;
      using Android = std::integral_constant<bool, false>;
      using MacOS = std::integral_constant<bool, false>;
      using iOS = std::integral_constant<bool, false>;
    #elif YTE_Linux
      using Windows = std::integral_constant<bool, false>;
      using Linux = std::integral_constant<bool, true>;
      using Android = std::integral_constant<bool, false>;
      using MacOS = std::integral_constant<bool, false>;
      using iOS = std::integral_constant<bool, false>;
    #elif YTE_Darwin
      using Windows = std::integral_constant<bool, false>;
      using Linux = std::integral_constant<bool, false>;
      using Android = std::integral_constant<bool, false>;
      using MacOS = std::integral_constant<bool, true>;
      using iOS = std::integral_constant<bool, false>;
    #elif YTE_Android
      using Windows = std::integral_constant<bool, false>;
      using Linux = std::integral_constant<bool, false>;
      using Android = std::integral_constant<bool, true>;
      using MacOS = std::integral_constant<bool, false>;
      using iOS = std::integral_constant<bool, false>;
    #else // Otherwise we're no known platform.
      using Windows = std::integral_constant<bool, false>;
      using Linux = std::integral_constant<bool, false>;
      using Android = std::integral_constant<bool, false>;
      using MacOS = std::integral_constant<bool, false>;
      using iOS = std::integral_constant<bool, false>;
    #endif

    #ifdef YTE_x64
      using x64 = std::integral_constant<bool, true>;
      using x86 = std::integral_constant<bool, false>;
    #else
      using x64 = std::integral_constant<bool, false>;
      using x86 = std::integral_constant<bool, true>;
    #endif
  };

  struct CompilerOptions
  {
    #if YTE_DEBUG
      using Release = std::integral_constant<bool, false>;
      using Debug = std::integral_constant<bool, true>;
    #else
      using Release = std::integral_constant<bool, true>;
      using Debug = std::integral_constant<bool, false>;
    #endif
  };

  struct CompilerConfiguration
  {
#if YTE_CONFIG_DEBUG
    using Publish = std::integral_constant<bool, false>;
    using Release = std::integral_constant<bool, false>;
    using RelWithDebInfo = std::integral_constant<bool, false>;
    using MinSizeRel = std::integral_constant<bool, false>;
    using Debug = std::integral_constant<bool, true>;
#elif YTE_CONFIG_MINSIZEREL
    using Publish = std::integral_constant<bool, false>;
    using Release = std::integral_constant<bool, false>;
    using RelWithDebInfo = std::integral_constant<bool, false>;
    using MinSizeRel = std::integral_constant<bool, true>;
    using Debug = std::integral_constant<bool, false>;
#elif YTE_CONFIG_RELWITHDEBINFO
    using Publish = std::integral_constant<bool, false>;
    using Release = std::integral_constant<bool, false>;
    using RelWithDebInfo = std::integral_constant<bool, true>;
    using MinSizeRel = std::integral_constant<bool, false>;
    using Debug = std::integral_constant<bool, false>;
#elif YTE_CONFIG_RELEASE
    using Publish = std::integral_constant<bool, false>;
    using Release = std::integral_constant<bool, true>;
    using RelWithDebInfo = std::integral_constant<bool, false>;
    using MinSizeRel = std::integral_constant<bool, false>;
    using Debug = std::integral_constant<bool, false>;
#else // PUBLISH
    using Publish = std::integral_constant<bool, true>;
    using Release = std::integral_constant<bool, false>;
    using RelWithDebInfo = std::integral_constant<bool, false>;
    using MinSizeRel = std::integral_constant<bool, false>;
    using Debug = std::integral_constant<bool, false>;
#endif
  };
}