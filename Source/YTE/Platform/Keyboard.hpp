/******************************************************************************/
/*!
* \author Joshua T. Fisher
* \date   2015-6-7
*
* \copyright All content 2016 DigiPen (USA) Corporation, all rights reserved.
*/
/******************************************************************************/
#pragma once

#ifndef YTE_Platform_Keyboard_h
#define YTE_Platform_Keyboard_h

#include <stdint.h>

#include "YTE/Core/EventHandler.hpp"

#include "YTE/Platform/DeviceEnums.hpp"
#include "YTE/Platform/ForwardDeclarations.hpp"

namespace YTE
{
  YTEDeclareEvent(KeyPress);
  YTEDeclareEvent(KeyRelease);
  YTEDeclareEvent(KeyPersist);

  class KeyboardEvent : public Event
  {
  public:
    YTEDeclareType(KeyboardEvent);

    Keys Key;
    Keyboard *Keyboard;
  };

  Keys TranslateFromOsToOurKey(uint64_t aOsKey);
  uint64_t TranslateFromOurToOSKey(Keys aOsKey);
  void SurveyKeyboard(bool *aKeyboard);

  // Checks key with OS.
  bool CheckKey(Keys aKey);

  class Keyboard : public EventHandler
  {
  public:
    YTEDeclareType(Keyboard);

    Keyboard();

    void Update();
    void UpdateKey(Keys aKey, bool aDown);
    void ForceAllKeysUp();

    bool IsKeyPressed(Keys aKey);
    bool IsKeyDown(Keys aKey);
    bool WasKeyDown(Keys aKey);

    bool *mKeysPrevious;
    bool *mKeysCurrent;

    Keyboard(const Keyboard&) { DebugObjection(true, "This should have never been called."); }
  private:
    bool mArrayOne[static_cast<size_t>(Keys::Keys_Number)];
    bool mArrayTwo[static_cast<size_t>(Keys::Keys_Number)];
  };
}

#endif
