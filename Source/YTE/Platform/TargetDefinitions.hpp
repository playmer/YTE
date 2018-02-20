/******************************************************************************/
/*!
 * \author Joshua T. Fisher
 * \date   2015-6-7
 *
 * \copyright All content 2016 DigiPen (USA) Corporation, all rights reserved.
 */
/******************************************************************************/
#pragma once

#ifdef _WIN32
    // If _WIN32 is defined, we're on Windows.
  #define YTE_Windows

    // If _WIN64 is defined, we're on x64.
  #ifdef _WIN64
    #define YTE_x64

    // Else we're on 32 bit.
  #else
    #define x86
  #endif
#endif

namespace YTE
{
  struct CompilerOptions
  {
    #ifdef _DEBUG
      using Release = std::integral_constant<bool, false>;
      using Debug = std::integral_constant<bool, true>;
    #else
      using Release = std::integral_constant<bool, true>;
      using Debug = std::integral_constant<bool, false>;
    #endif

    #ifdef YTE_x64
      using x64 = std::integral_constant<bool, true>;
      using x86 = std::integral_constant<bool, false>;
    #else
      using x64 = std::integral_constant<bool, false>;
      using x86 = std::integral_constant<bool, true>;
    #endif
  };
}