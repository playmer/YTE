/******************************************************************************/
/*!
* \author Joshua T. Fisher
* \date   6/7/2015
*
* \copyright All content 2016 DigiPen (USA) Corporation, all rights reserved.
*/
/******************************************************************************/
#include "YTE/Platform/TargetDefinitions.hpp"

#include <stdint.h>
#include <iostream>
#include <string>
#include <locale>
#include <codecvt>

#include "SDL.h"

#include "YTE/Platform/Keyboard.hpp"

namespace YTE
{
  Keys TranslateFromOsToOurKey(uint64_t aOsKey)
  {
    switch (aOsKey)
    {
      #define ProcessKey(aOsKey, aOurKey) case (aOsKey) : return Keys::aOurKey;
        #include "YTE/Platform/SDL/OsKeys_SDL.hpp"
      #undef ProcessKey

      default: return Keys::Unknown;
    }
  }

  uint64_t TranslateFromOurToOSKey(Keys aOurKey)
  {
    switch (aOurKey)
    {
      #define ProcessKey(aOsKey, aOurKey) case (Keys::aOurKey) : return aOsKey;
        #include "YTE/Platform/SDL/OsKeys_SDL.hpp"
      #undef ProcessKey

      default: return 0;
    }
  }

  bool CheckKey(Keys aKey)
  {
    //auto key = TranslateFromOurToOSKey(aKey);
    //
    //bool state = GetAsyncKeyState(static_cast<int>(key)) & 0x8000;
    //
    //return state;

    return false;
  }

  std::string GetClipboardText()
  {
    auto text = SDL_GetClipboardText();

    std::string toReturn{ text };
    SDL_free(text);

    return toReturn;
  }

  void SetClipboardText(std::string &aText)
  {
    SDL_SetClipboardText(aText.c_str());
  }
}
