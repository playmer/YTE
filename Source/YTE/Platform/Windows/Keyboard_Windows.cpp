/******************************************************************************/
/*!
* \author Joshua T. Fisher
* \date   6/7/2015
*
* \copyright All content 2016 DigiPen (USA) Corporation, all rights reserved.
*/
/******************************************************************************/
#include "YTE/Platform/TargetDefinitions.hpp"

#ifdef YTE_Windows

#include <stdint.h>

#include "YTE/Platform/Windows/WindowsInclude_Windows.hpp"
#include <Winuser.h>

#include "YTE/Platform/Keyboard.hpp"

namespace YTE
{
  Keys TranslateFromOsToOurKey(uint64_t aOsKey)
  {
    switch (aOsKey)
    {
#define ProcessKey(aOsKey, aOurKey) case (aOsKey) : return Keys::aOurKey;
#include "YTE/Platform/Windows/OsKeys_Windows.hpp"
#undef ProcessKey
    default: return Keys::Unknown;
    }
  }

  uint64_t TranslateFromOurToOSKey(Keys aOurKey)
  {
    switch (aOurKey)
    {
#define ProcessKey(aOsKey, aOurKey) case (Keys::aOurKey) : return aOsKey;
#include "YTE/Platform/Windows/OsKeys_Windows.hpp"
#undef ProcessKey
    default: return 0;
    }
  }

  bool CheckKey(Keys aKey)
  {
    auto key = TranslateFromOurToOSKey(aKey);

    bool state = GetAsyncKeyState(static_cast<int>(key)) & 0x8000;

    return state;
  }
}
#endif
