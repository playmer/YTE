/******************************************************************************/
/*!
 * \author Joshua T. Fisher
 * \date   2015-6-7
 *
 * \copyright All content 2016 DigiPen (USA) Corporation, all rights reserved.
 */
/******************************************************************************/
#pragma once

#include <type_traits>

#ifdef _WIN32
    // If _WIN32 is defined, we're on Windows.
  #define YTE_Windows 1

    // If _WIN64 is defined, we're on x64.
  #ifdef _WIN64
    #define YTE_x64 1

    // Else we're on 32 bit.
  #else
    #define YTE_x86
  #endif
#endif

#if defined(YTE_Windows) && defined(YTE_Internal)
  #define YTE_Shared __declspec( dllexport )
#elif defined(YTE_Windows)
  #define YTE_Shared __declspec( dllimport )
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
}