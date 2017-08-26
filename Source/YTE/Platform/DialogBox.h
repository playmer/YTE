/******************************************************************************/
/*!
* \author Joshua T. Fisher
* \date   6/7/2015
*
* \copyright All content 2016 DigiPen (USA) Corporation, all rights reserved.
*/
/******************************************************************************/
#pragma once

#ifndef YTE_Platform_DialogBox_h
#define YTE_Platform_DialogBox_h

#include <cstdint>

#include "YTE/Platform/TargetDefinitions.h"

namespace YTE
{
  enum class DebugErrorDialog
  {
    Continue     = 110,
    DebugBreak   = 111,
    Abort        = 112, 
    ErrorOccured = 113
  };

  DebugErrorDialog CreateDebugErrorDialog(const char *aFormatString, ...);

  //inline DebugErrorDialog CreateDebugErrorDialog(YTE::String aString)
  //{
  //  return CreateDebugErrorDialog(aString.c_str());
  //}
  //
  //inline DebugErrorDialog CreateDebugErrorDialog(std::string &aString)
  //{
  //  return CreateDebugErrorDialog(aString.c_str());
  //}
  //
  //inline DebugErrorDialog CreateDebugErrorDialog(YTE::StringRange aString)
  //{
  //  return CreateDebugErrorDialog(aString.data());
  //}
}

#ifdef _MSC_VER
  #define debugbreak __debugbreak
#else
  #define debugbreak __builtin_trap
#endif

#ifdef Windows
  #ifndef NDEBUG
    #define DebugAssert(aCondition, aString, ...)                    \
    do                                                               \
    {                                                                \
      if (aCondition)                                                \
      {                                                              \
        switch (YTE::CreateDebugErrorDialog(aString, ##__VA_ARGS__)) \
        {                                                            \
         case YTE::DebugErrorDialog::Continue:                       \
         {                                                           \
           /* Do nothing. */                                         \
           break;                                                    \
         }                                                           \
         case YTE::DebugErrorDialog::DebugBreak:                     \
         {                                                           \
           debugbreak();                                             \
           break;                                                    \
         }                                                           \
         case YTE::DebugErrorDialog::Abort:                          \
         {                                                           \
           exit(1);                                                  \
           break;                                                    \
         }                                                           \
         case YTE::DebugErrorDialog::ErrorOccured:                   \
         {                                                           \
           break;                                                    \
         }                                                           \
         default:                                                    \
         {                                                           \
           break;                                                    \
         }                                                           \
        }                                                            \
      }                                                              \
    } while (false)

  #else
    #define DebugAssert(aCondition, aString, ...)
  #endif
#else
  #define DebugAssert(aCondition, aString, ...)
#endif

#endif