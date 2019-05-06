#pragma once

#ifndef YTE_Platform_Keyboard_hpp
#define YTE_Platform_Keyboard_hpp

#include <stdint.h>

#include "YTE/Core/EventHandler.hpp"

#include "YTE/Platform/DeviceEnums.hpp"
#include "YTE/Platform/ForwardDeclarations.hpp"

namespace YTE
{
  YTEDeclareEvent(KeyPress);
  YTEDeclareEvent(KeyRelease);
  YTEDeclareEvent(KeyPersist);
  YTEDeclareEvent(CharacterTyped);

  class KeyboardEvent : public Event
  {
  public:
    YTEDeclareType(KeyboardEvent);

    Keys Key;
    u32 CharacterTyped;
    Keyboard *SendingKeyboard;
  };

  YTE_Shared Keys TranslateFromOsToOurKey(uint64_t aOsKey);
  YTE_Shared uint64_t TranslateFromOurToOSKey(Keys aOsKey);
  YTE_Shared void SurveyKeyboard(bool *aKeyboard);

  YTE_Shared std::string GetClipboardText();
  YTE_Shared void SetClipboardText(std::string &aText);

  // Checks key with OS.
  //YTE_Shared bool CheckKey(Keys aKey);

  class Keyboard : public EventHandler
  {
  public:
    YTEDeclareType(Keyboard);

    YTE_Shared Keyboard();

    YTE_Shared void Update();
    YTE_Shared void UpdateKey(Keys aKey, bool aDown);
    YTE_Shared void ForceAllKeysUp();

    YTE_Shared bool IsKeyPressed(Keys aKey);
    YTE_Shared bool IsKeyDown(Keys aKey);
    YTE_Shared bool WasKeyDown(Keys aKey);

    bool *mKeysPrevious;
    bool *mKeysCurrent;

    Keyboard(const Keyboard&) { DebugObjection(true, "This should have never been called."); }
  private:
    bool mArrayOne[static_cast<size_t>(Keys::Keys_Number)];
    bool mArrayTwo[static_cast<size_t>(Keys::Keys_Number)];
  };
}

#endif
