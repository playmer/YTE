/******************************************************************************/
/*!
 * \author Joshua T. Fisher
 * \date   6/7/2015
 *
 * \copyright All content 2016 DigiPen (USA) Corporation, all rights reserved.
 */
/******************************************************************************/
#include "YTE/Platform/TargetDefinitions.h"

#ifdef Windows

#include <YTE/Platform/Windows/WindowsInclude.hpp>
#include <Winuser.h>
#include <stdint.h>

#include "YTE/Platform/Keyboard.h"

namespace YTE
{
  Keys TranslateKey(uint64_t aOsKey)
  {
    switch (aOsKey)
    {
    #define ProcessKey(aOsKey, aOurKey) case (aOsKey) : return Keys::aOurKey;
    #include "OsKeys.hpp"
    #undef ProcessKey
    default: return Keys::Unknown;
    }
  }
}
#endif