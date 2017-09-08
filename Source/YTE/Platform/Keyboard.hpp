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

#include "YTE/Core/EventHandler.hpp"
#include "YTE/Platform/DeviceEnums.hpp"
#include <stdint.h>

namespace YTE
{
  DeclareEvent(KeyPress);
  DeclareEvent(KeyRelease);
  DeclareEvent(KeyPersist);
  class KeyboardEvent : public Event
  {
  public:
    DeclareType(KeyboardEvent);

    Keys Key;
    Keyboard *Keyboard;
  };

  Keys TranslateKey(uint64_t aOsKey);
  void SurveyKeyboard(bool *aKeyboard);

  struct Keyboard : public EventHandler
  {
  public:
    DeclareType(Keyboard);
      
    Keyboard();

    void Update();
    void UpdateKey(Keys aKey, bool aDown);

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