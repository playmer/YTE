#include "DeviceEnums.hpp"

/*******************************************************************************
YTE Enums
*******************************************************************************/
YTEDefineExternalType(YTE::GamepadButtons)
{
  RegisterType<YTE::GamepadButtons>();
  TypeBuilder<YTE::GamepadButtons> builder;

  //builder.Enum<GamepadButtons::DPAD_Up>("DPAD_Up");
  //builder.Enum<GamepadButtons::DPAD_Down>("DPAD_Down");
  //builder.Enum<GamepadButtons::DPAD_Left>("DPAD_Left");
  //builder.Enum<GamepadButtons::DPAD_Right>("DPAD_Right");
  //builder.Enum<GamepadButtons::Start>("Start");
  //builder.Enum<GamepadButtons::Back>("Back");
  //builder.Enum<GamepadButtons::LeftStick>("LeftStick");
  //builder.Enum<GamepadButtons::RightStick>("RightStick");
  //builder.Enum<GamepadButtons::LeftShoulder>("LeftShoulder");
  //builder.Enum<GamepadButtons::RightShoulder>("RightShoulder");
  //builder.Enum<GamepadButtons::A>("A");
  //builder.Enum<GamepadButtons::B>("B");
  //builder.Enum<GamepadButtons::X>("X");
  //builder.Enum<GamepadButtons::Y>("Y");
  //builder.Enum<GamepadButtons::Unknown>("Unknown");
}


template<typename tEnumType, tEnumType tValue>
constexpr auto GetEnumValueAsUnderlyingType()
{
  return static_cast<typename std::underlying_type<tEnumType>::type>(tValue);
}

template <typename tEnumType, tEnumType tValue>
constexpr typename std::underlying_type<tEnumType>::type GetNativeType()
{
  return static_cast<typename std::underlying_type<tEnumType>::type>(tValue);
}

YTEDefineExternalType(YTE::MouseButtons)
{
  RegisterType<YTE::MouseButtons>();
  TypeBuilder<YTE::MouseButtons> builder;

  //builder.Enum<MouseButtons::Left>("Left");
  //builder.Enum<MouseButtons::Middle>("Middle");
  //builder.Enum<MouseButtons::Right>("Right");
  //builder.Enum<MouseButtons::Back>("Back");
  //builder.Enum<MouseButtons::Forward>("Forward");
  //builder.Enum<MouseButtons::Unknown>("Unknown");
}

YTEDefineExternalType(YTE::Keys)
{
  RegisterType<YTE::Keys>();
  TypeBuilder<YTE::Keys> builder;

  //builder.Enum<Keys::Mouse_Left>("Mouse_Left");
  //builder.Enum<Keys::Mouse_Right>("Mouse_Right");
  //builder.Enum<Keys::Mouse_Middle>("Mouse_Middle");
  //builder.Enum<Keys::Mouse_Forward>("Mouse_Forward");
  //builder.Enum<Keys::Mouse_Back>("Mouse_Back");
  //builder.Enum<Keys::A>("A");
  //builder.Enum<Keys::B>("B");
  //builder.Enum<Keys::C>("C");
  //builder.Enum<Keys::D>("D");
  //builder.Enum<Keys::E>("E");
  //builder.Enum<Keys::F>("F");
  //builder.Enum<Keys::G>("G");
  //builder.Enum<Keys::H>("H");
  //builder.Enum<Keys::I>("I");
  //builder.Enum<Keys::J>("J");
  //builder.Enum<Keys::K>("K");
  //builder.Enum<Keys::L>("L");
  //builder.Enum<Keys::M>("M");
  //builder.Enum<Keys::N>("N");
  //builder.Enum<Keys::O>("O");
  //builder.Enum<Keys::P>("P");
  //builder.Enum<Keys::Q>("Q");
  //builder.Enum<Keys::R>("R");
  //builder.Enum<Keys::S>("S");
  //builder.Enum<Keys::T>("T");
  //builder.Enum<Keys::U>("U");
  //builder.Enum<Keys::V>("V");
  //builder.Enum<Keys::W>("W");
  //builder.Enum<Keys::X>("X");
  //builder.Enum<Keys::Y>("Y");
  //builder.Enum<Keys::Z>("Z");
  //builder.Enum<Keys::N1>("N1");
  //builder.Enum<Keys::N2>("N2");
  //builder.Enum<Keys::N3>("N3");
  //builder.Enum<Keys::N4>("N4");
  //builder.Enum<Keys::N5>("N5");
  //builder.Enum<Keys::N6>("N6");
  //builder.Enum<Keys::N7>("N7");
  //builder.Enum<Keys::N8>("N8");
  //builder.Enum<Keys::N9>("N9");
  //builder.Enum<Keys::N0>("N0");
  //builder.Enum<Keys::Return>("Return");
  //builder.Enum<Keys::Escape>("Escape");
  //builder.Enum<Keys::Backspace>("Backspace");
  //builder.Enum<Keys::Tab>("Tab");
  //builder.Enum<Keys::Space>("Space");
  //builder.Enum<Keys::Minus>("Minus");
  //builder.Enum<Keys::Equals>("Equals");
  //builder.Enum<Keys::LeftBracket>("LeftBracket");
  //builder.Enum<Keys::RightBracket>("RightBracket");
  //builder.Enum<Keys::Backslash>("Backslash");
  //builder.Enum<Keys::Semicolon>("Semicolon");
  //builder.Enum<Keys::Apostrophe>("Apostrophe");
  //builder.Enum<Keys::Grave>("Grave");
  //builder.Enum<Keys::Comma>("Comma");
  //builder.Enum<Keys::Period>("Period");
  //builder.Enum<Keys::ForwardSlash>("ForwardSlash");
  //builder.Enum<Keys::Capslock>("Capslock");
  //builder.Enum<Keys::F1>("F1");
  //builder.Enum<Keys::F2>("F2");
  //builder.Enum<Keys::F3>("F3");
  //builder.Enum<Keys::F4>("F4");
  //builder.Enum<Keys::F5>("F5");
  //builder.Enum<Keys::F6>("F6");
  //builder.Enum<Keys::F7>("F7");
  //builder.Enum<Keys::F8>("F8");
  //builder.Enum<Keys::F9>("F9");
  //builder.Enum<Keys::F10>("F10");
  //builder.Enum<Keys::F11>("F11");
  //builder.Enum<Keys::F12>("F12");
  //builder.Enum<Keys::ScrollLock>("ScrollLock");
  //builder.Enum<Keys::Pause>("Pause");
  //builder.Enum<Keys::Insert>("Insert");
  //builder.Enum<Keys::Home>("Home");
  //builder.Enum<Keys::PageUp>("PageUp");
  //builder.Enum<Keys::Delete>("Delete");
  //builder.Enum<Keys::End>("End");
  //builder.Enum<Keys::PageDown>("PageDown");
  //builder.Enum<Keys::Right>("Right");
  //builder.Enum<Keys::Left>("Left");
  //builder.Enum<Keys::Down>("Down");
  //builder.Enum<Keys::Up>("Up");
  //builder.Enum<Keys::KP_Divide>("KP_Divide");
  //builder.Enum<Keys::KP_Multiply>("KP_Multiply");
  //builder.Enum<Keys::KP_Minus>("KP_Minus");
  //builder.Enum<Keys::KP_Plus>("KP_Plus");
  //builder.Enum<Keys::KP_Enter>("KP_Enter");
  //builder.Enum<Keys::KP_1>("KP_1");
  //builder.Enum<Keys::KP_2>("KP_2");
  //builder.Enum<Keys::KP_3>("KP_3");
  //builder.Enum<Keys::KP_4>("KP_4");
  //builder.Enum<Keys::KP_5>("KP_5");
  //builder.Enum<Keys::KP_6>("KP_6");
  //builder.Enum<Keys::KP_7>("KP_7");
  //builder.Enum<Keys::KP_8>("KP_8");
  //builder.Enum<Keys::KP_9>("KP_9");
  //builder.Enum<Keys::KP_0>("KP_0");
  //builder.Enum<Keys::KP_Period>("KP_Period");
  //builder.Enum<Keys::KP_Equals>("KP_Equals");
  //builder.Enum<Keys::F13>("F13");
  //builder.Enum<Keys::F14>("F14");
  //builder.Enum<Keys::F15>("F15");
  //builder.Enum<Keys::Mute>("Mute");
  //builder.Enum<Keys::VolumeUp>("VolumeUp");
  //builder.Enum<Keys::VolumeDown>("VolumeDown");
  //builder.Enum<Keys::KP_Comma>("KP_Comma");
  //builder.Enum<Keys::LeftControl>("LeftControl");
  //builder.Enum<Keys::LeftShift>("LeftShift");
  //builder.Enum<Keys::LeftAlt>("LeftAlt");
  //builder.Enum<Keys::RightControl>("RightControl");
  //builder.Enum<Keys::RightShift>("RightShift");
  //builder.Enum<Keys::RightAlt>("RightAlt");
  //builder.Enum<Keys::Keys_Number>("Keys_Number");
  //builder.Enum<Keys::Unknown>("Unknown");
}
