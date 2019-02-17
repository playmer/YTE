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
  
  void KeyboardEventHandler(SDL_Event aEvent, Keyboard* aKeyboard)
  {
    switch (aEvent.type)
    {
        // Key pressed
      case SDL_KEYDOWN:
      {
        auto key = TranslateFromOsToOurKey(aEvent.key.keysym.sym);

        aKeyboard->UpdateKey(key, true);

        switch (key)
        {
          case Keys::LeftShift:
          case Keys::RightShift:
          {
            aKeyboard->UpdateKey(Keys::Shift, true);
            break;
          }

          case Keys::LeftControl:
          case Keys::RightControl:
          {
            aKeyboard->UpdateKey(Keys::Control, true);
            break;
          }

          case Keys::LeftAlt:
          case Keys::RightAlt:
          {
            aKeyboard->UpdateKey(Keys::Alt, true);
            break;
          }
        }

        break;
      }

        // Key released
      case SDL_KEYUP:
      {
        auto key = TranslateFromOsToOurKey(aEvent.key.keysym.sym);

        aKeyboard->UpdateKey(TranslateFromOsToOurKey(aEvent.key.keysym.sym), false);
        
        switch (key)
        {
          case Keys::LeftShift:
          {
            if (false == aKeyboard->IsKeyDown(Keys::RightShift))
            {
              aKeyboard->UpdateKey(Keys::Shift, false);
            }

            break;
          }

          case Keys::RightShift:
          {
            if (false == aKeyboard->IsKeyDown(Keys::LeftShift))
            {
              aKeyboard->UpdateKey(Keys::Shift, false);
            }

            break;
          }

          case Keys::LeftControl:
          {
            if (false == aKeyboard->IsKeyDown(Keys::RightControl))
            {
              aKeyboard->UpdateKey(Keys::Control, false);
            }

            break;
          }

          case Keys::RightControl:
          {
            if (false == aKeyboard->IsKeyDown(Keys::LeftControl))
            {
              aKeyboard->UpdateKey(Keys::Control, false);
            }

            break;
          }

          case Keys::LeftAlt:
          {
            if (false == aKeyboard->IsKeyDown(Keys::RightAlt))
            {
              aKeyboard->UpdateKey(Keys::Alt, false);
            }

            break;
          }

          case Keys::RightAlt:
          {
            if (false == aKeyboard->IsKeyDown(Keys::LeftAlt))
            {
              aKeyboard->UpdateKey(Keys::Alt, false);
            }

            break;
          }
        }

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

  //bool CheckKey(Keys aKey)
  //{
  //  //auto key = TranslateFromOurToOSKey(aKey);
  //  //
  //  //bool state = GetAsyncKeyState(static_cast<int>(key)) & 0x8000;
  //  //
  //  //return state;
  //
  //  return false;
  //}

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
