/******************************************************************************/
/*!
 * \author Joshua T. Fisher
 * \date   6/30/2015
 *
 * \copyright All content 2016 DigiPen (USA) Corporation, all rights reserved.
 */
/******************************************************************************/
#pragma once

#ifndef YTE_Utilities_Assert_H
#define YTE_Utilities_Assert_H
#include "YTE/Platform/TargetDefinitions.h"

#ifndef NDEBUG
  #define YTE_Error_Checking
#endif

#ifdef YTE_Error_Checking
  #define Assert(aExpression) assert(aExpression)
  #define YTE_Debug_Break Assert(false)
#else
  #define Assert(aExpression) (void)0;
  #define YTE_Debug_Break (void)0
#endif

#endif