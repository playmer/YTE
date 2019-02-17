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

      default: return SDLK_UNKNOWN;
    }
  }
  
  void KeyboardEventHandler(SDL_Event aEvent, Window* aWindow, Keyboard* aKeyboard)
  {
    switch (aEvent.type)
    {
        // Key pressed
      case SDL_KEYDOWN:
      {
        aKeyboard->UpdateKey(TranslateFromOsToOurKey(aEvent.key.keysym.sym), true);
        break;
      }

        // Key released
      case SDL_KEYUP:
      {
        aKeyboard->UpdateKey(TranslateFromOsToOurKey(aEvent.key.keysym.sym), false);
        break;
      }

        // Keyboard text editing (composition)
      case SDL_TEXTEDITING:
      {
        break;
      }

        // Keyboard text input
      case SDL_TEXTINPUT:
      {
        break;
      }

        // Keymap changed due to a system event such as an input language or keyboard layout change.
      case SDL_KEYMAPCHANGED:
      {
        break;
      }
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
