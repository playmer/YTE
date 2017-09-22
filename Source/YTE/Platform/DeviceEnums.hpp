/******************************************************************************/
/*!
* \author Joshua T. Fisher
* \date   2015-6-7
*
* \copyright All content 2016 DigiPen (USA) Corporation, all rights reserved.
*/
/******************************************************************************/
#pragma once

#ifndef YTE_Platform_DeviceEnums_h
#define YTE_Platform_DeviceEnums_h

#include <stdint.h>
#include <type_traits>

#include "YTE/Core/ScriptBind.hpp"

namespace YTE
{
  enum class Controller_Id : uint32_t
  {
    Xbox_P1,
    Xbox_P2,
    Xbox_P3,
    Xbox_P4,
    Xbox_Controllers_Number,
    Unknown
  };

  enum class Xbox_Buttons : uint32_t
  {
    DPAD_Up,
    DPAD_Down,
    DPAD_Left,
    DPAD_Right,
    Start,
    Back,
    LeftStick,
    RightStick,
    LeftShoulder,
    RightShoulder,
    A,
    B,
    X,
    Y,
    Xbox_Buttons_Number,
    Unknown
  };

  enum class Mouse_Buttons : uint32_t
  {
    Left,
    Right,
    Middle,
    Back,
    Forward,
    Mouse_Buttons_Number,
    Unknown,
  };

  enum class Keys : uint32_t
  {
    // Mouse Buttons are sometimes represented in keyboard space.
    Mouse_Left,
    Mouse_Right,
    Mouse_Middle,
    Mouse_Back,
    Mouse_Forward,
    // Standard English Alphabet
    A,
    B,
    C,
    D,
    E,
    F,
    G,
    H,
    I,
    J,
    K,
    L,
    M,
    N,
    O,
    P,
    Q,
    R,
    S,
    T,
    U,
    V,
    W,
    X,
    Y,
    Z,

    // Numbers
    N1,
    N2,
    N3,
    N4,
    N5,
    N6,
    N7,
    N8,
    N9,
    N0,

    Return,
    Escape,
    Backspace,
    Tab,
    Space,
    Minus,
    Equals,
    LeftBracket,
    RightBracket,
    Backslash,
    Semicolon,
    Apostrophe,
    Grave,
    Comma,
    Period,
    ForwardSlash,
    Pause,

    PrintScreen,
    Insert,
    Delete,
    PageUp,
    PageDown,
    Home,
    End,

    // Directions
    Right,
    Left,
    Down,
    Up,

    // Key locks
    Capslock,
    ScrollLock,
    NumLock,

    // KP stands for Keypad.
    KP_Divide,
    KP_Multiply,
    KP_Minus,
    KP_Plus,
    KP_1,
    KP_2,
    KP_3,
    KP_4,
    KP_5,
    KP_6,
    KP_7,
    KP_8,
    KP_9,
    KP_0,
    KP_Period,
    KP_Enter,
    KP_Equals,
    KP_Comma,

    F1,
    F2,
    F3,
    F4,
    F5,
    F6,
    F7,
    F8,
    F9,
    F10,
    F11,
    F12,
    F13,
    F14,
    F15,

    Mute,
    VolumeUp,
    VolumeDown,

    Control,
    LeftControl,
    RightControl,
    Shift,
    LeftShift,
    RightShift,
    Alt,
    LeftAlt,
    RightAlt,

    Keys_Number,
    Unknown,
  };


  ///////////////////////////////////////
  // Static Functions
  ///////////////////////////////////////
  inline Xbox_Buttons& operator++(Xbox_Buttons &aButton)
  {
    using IntType = std::underlying_type<Xbox_Buttons>::type;

    aButton = static_cast<Xbox_Buttons>(static_cast<IntType>(aButton) + 1);

    return aButton;
  }

  inline Xbox_Buttons operator++(Xbox_Buttons &aButton, int)
  {
    Xbox_Buttons result = aButton;
    ++aButton;
    return result;
  }


  inline Mouse_Buttons& operator++(Mouse_Buttons &aButton)
  {
    using IntType = std::underlying_type<Mouse_Buttons>::type;

    aButton = static_cast<Mouse_Buttons>(static_cast<IntType>(aButton) + 1);

    return aButton;
  }

  inline Mouse_Buttons operator++(Mouse_Buttons &aButton, int)
  {
    Mouse_Buttons result = aButton;
    ++aButton;
    return result;
  }

  inline Keys& operator++(Keys &aKey)
  {
    using IntType = std::underlying_type<Keys>::type;

    aKey = static_cast<Keys>(static_cast<IntType>(aKey) + 1);

    return aKey;
  }

  inline Keys operator++(Keys &aKey, int)
  {
    Keys result = aKey;
    ++aKey;
    return result;
  }
}

YTEDeclareExternalType(YTE::Controller_Id);
YTEDeclareExternalType(YTE::Xbox_Buttons);
YTEDeclareExternalType(YTE::Mouse_Buttons);
YTEDeclareExternalType(YTE::Keys);

#endif
