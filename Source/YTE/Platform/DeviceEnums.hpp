#pragma once

#ifndef YTE_Platform_DeviceEnums_h
#define YTE_Platform_DeviceEnums_h

#include <stdint.h>
#include <type_traits>

#include "YTE/Core/ScriptBind.hpp"

namespace YTE
{
  enum class ControllerId : uint32_t
  {
    Xbox_P1,
    Xbox_P2,
    Xbox_P3,
    Xbox_P4,
    Xbox_Controllers_Number,
    Unknown
  };

  enum class XboxButtons : uint32_t
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

  enum class MouseButtons : uint32_t
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
  inline XboxButtons& operator++(XboxButtons &aButton)
  {
    using IntType = std::underlying_type<XboxButtons>::type;

    aButton = static_cast<XboxButtons>(static_cast<IntType>(aButton) + 1);

    return aButton;
  }

  inline XboxButtons operator++(XboxButtons &aButton, int)
  {
    XboxButtons result = aButton;
    ++aButton;
    return result;
  }


  inline MouseButtons& operator++(MouseButtons &aButton)
  {
    using IntType = std::underlying_type<MouseButtons>::type;

    aButton = static_cast<MouseButtons>(static_cast<IntType>(aButton) + 1);

    return aButton;
  }

  inline MouseButtons operator++(MouseButtons &aButton, int)
  {
    MouseButtons result = aButton;
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

YTEDeclareExternalType(YTE::ControllerId);
YTEDeclareExternalType(YTE::XboxButtons);
YTEDeclareExternalType(YTE::MouseButtons);
YTEDeclareExternalType(YTE::Keys);

#endif
