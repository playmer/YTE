/******************************************************************************/
/*!
 * \author Joshua T. Fisher
 * \date   2015-6-7
 *
 * \copyright All content 2016 DigiPen (USA) Corporation, all rights reserved.
 */
/******************************************************************************/
#include "DeviceEnums.hpp"

/*******************************************************************************
YTE Enums
*******************************************************************************/
YTEDefineExternalType(YTE::Controller_Id)
{
  YTERegisterType(YTE::Controller_Id);

//  YTEBindEnumValue(Controller_Id::Xbox_P1, "Xbox_P1");
//  YTEBindEnumValue(Controller_Id::Xbox_P2, "Xbox_P2");
//  YTEBindEnumValue(Controller_Id::Xbox_P3, "Xbox_P3");
//  YTEBindEnumValue(Controller_Id::Xbox_P4, "Xbox_P4");
//  YTEBindEnumValue(Controller_Id::Xbox_Controllers_Number, "Xbox_Controllers_Number");
//  YTEBindEnumValue(Controller_Id::Unknown, "Unknown");
}

YTEDefineExternalType(YTE::Xbox_Buttons)
{
  YTERegisterType(YTE::Xbox_Buttons);
//
//  YTEBindEnumValue(Xbox_Buttons::DPAD_Up, "DPAD_Up");
//  YTEBindEnumValue(Xbox_Buttons::DPAD_Down, "DPAD_Down");
//  YTEBindEnumValue(Xbox_Buttons::DPAD_Left, "DPAD_Left");
//  YTEBindEnumValue(Xbox_Buttons::DPAD_Right, "DPAD_Right");
//  YTEBindEnumValue(Xbox_Buttons::Start, "Start");
//  YTEBindEnumValue(Xbox_Buttons::Back, "Back");
//  YTEBindEnumValue(Xbox_Buttons::LeftStick, "LeftStick");
//  YTEBindEnumValue(Xbox_Buttons::RightStick, "RightStick");
//  YTEBindEnumValue(Xbox_Buttons::LeftShoulder, "LeftShoulder");
//  YTEBindEnumValue(Xbox_Buttons::RightShoulder, "RightShoulder");
//  YTEBindEnumValue(Xbox_Buttons::A, "A");
//  YTEBindEnumValue(Xbox_Buttons::B, "B");
//  YTEBindEnumValue(Xbox_Buttons::X, "X");
//  YTEBindEnumValue(Xbox_Buttons::Y, "Y");
//  YTEBindEnumValue(Xbox_Buttons::Unknown, "Unknown");
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

YTEDefineExternalType(YTE::Mouse_Buttons)
{
  YTERegisterType(YTE::Mouse_Buttons);

  //auto size = CountFunctionArguments<decltype(GetEnumAsNativeType<decltype(GetEnumValueAsUnderlyingType<decltype(Mouse_Buttons::Left), Mouse_Buttons::Left>()),
  //                                            GetEnumValueAsUnderlyingType<decltype(Mouse_Buttons::Left), Mouse_Buttons::Left>()>)>::template Size();

  //::YTE::BindFunction<decltype(GetEnumAsNativeType<decltype(GetEnumValueAsUnderlyingType<decltype(Mouse_Buttons::Left), Mouse_Buttons::Left>()), 
  //                                                 GetEnumValueAsUnderlyingType<decltype(Mouse_Buttons::Left), Mouse_Buttons::Left>()>),
  //                    GetEnumAsNativeType<decltype(GetEnumValueAsUnderlyingType<decltype(Mouse_Buttons::Left), Mouse_Buttons::Left>()),
  //                                        GetEnumValueAsUnderlyingType<decltype(Mouse_Buttons::Left), Mouse_Buttons::Left>()>,
  //                    std::initializer_list<const char*>YTENoNames.size()>("Left", 
  //                                                                         ::YTE::TypeId<decltype(Mouse_Buttons::Left)>(), 
  //                                                                         YTENoNames);

  ::YTE::BindFunction<decltype(&GetNativeType<decltype(Mouse_Buttons::Left), Mouse_Buttons::Left>),
                      &GetNativeType<decltype(Mouse_Buttons::Left), Mouse_Buttons::Left>,
                      std::initializer_list<const char*>YTENoNames.size()>("Left", 
                                                                           ::YTE::TypeId<decltype(Mouse_Buttons::Left)>(), 
                                                                           YTENoNames);



  //YTEBindEnumValue(Mouse_Buttons::Left, "Left");
  //YTEBindEnumValue(Mouse_Buttons::Middle, "Middle");
  //YTEBindEnumValue(Mouse_Buttons::Right, "Right");
  //YTEBindEnumValue(Mouse_Buttons::Back, "Back");
  //YTEBindEnumValue(Mouse_Buttons::Forward, "Forward");
  //YTEBindEnumValue(Mouse_Buttons::Unknown, "Unknown");
}

YTEDefineExternalType(YTE::Keys)
{
  YTERegisterType(YTE::Keys);

//  YTEBindEnumValue(Keys::Mouse_Left, "Mouse_Left");
//  YTEBindEnumValue(Keys::Mouse_Right, "Mouse_Right");
//  YTEBindEnumValue(Keys::Mouse_Middle, "Mouse_Middle");
//  YTEBindEnumValue(Keys::Mouse_Forward, "Mouse_Forward");
//  YTEBindEnumValue(Keys::Mouse_Back, "Mouse_Back");
//  YTEBindEnumValue(Keys::A, "A");
//  YTEBindEnumValue(Keys::B, "B");
//  YTEBindEnumValue(Keys::C, "C");
//  YTEBindEnumValue(Keys::D, "D");
//  YTEBindEnumValue(Keys::E, "E");
//  YTEBindEnumValue(Keys::F, "F");
//  YTEBindEnumValue(Keys::G, "G");
//  YTEBindEnumValue(Keys::H, "H");
//  YTEBindEnumValue(Keys::I, "I");
//  YTEBindEnumValue(Keys::J, "J");
//  YTEBindEnumValue(Keys::K, "K");
//  YTEBindEnumValue(Keys::L, "L");
//  YTEBindEnumValue(Keys::M, "M");
//  YTEBindEnumValue(Keys::N, "N");
//  YTEBindEnumValue(Keys::O, "O");
//  YTEBindEnumValue(Keys::P, "P");
//  YTEBindEnumValue(Keys::Q, "Q");
//  YTEBindEnumValue(Keys::R, "R");
//  YTEBindEnumValue(Keys::S, "S");
//  YTEBindEnumValue(Keys::T, "T");
//  YTEBindEnumValue(Keys::U, "U");
//  YTEBindEnumValue(Keys::V, "V");
//  YTEBindEnumValue(Keys::W, "W");
//  YTEBindEnumValue(Keys::X, "X");
//  YTEBindEnumValue(Keys::Y, "Y");
//  YTEBindEnumValue(Keys::Z, "Z");
//  YTEBindEnumValue(Keys::N1, "N1");
//  YTEBindEnumValue(Keys::N2, "N2");
//  YTEBindEnumValue(Keys::N3, "N3");
//  YTEBindEnumValue(Keys::N4, "N4");
//  YTEBindEnumValue(Keys::N5, "N5");
//  YTEBindEnumValue(Keys::N6, "N6");
//  YTEBindEnumValue(Keys::N7, "N7");
//  YTEBindEnumValue(Keys::N8, "N8");
//  YTEBindEnumValue(Keys::N9, "N9");
//  YTEBindEnumValue(Keys::N0, "N0");
//  YTEBindEnumValue(Keys::Return, "Return");
//  YTEBindEnumValue(Keys::Escape, "Escape");
//  YTEBindEnumValue(Keys::Backspace, "Backspace");
//  YTEBindEnumValue(Keys::Tab, "Tab");
//  YTEBindEnumValue(Keys::Space, "Space");
//  YTEBindEnumValue(Keys::Minus, "Minus");
//  YTEBindEnumValue(Keys::Equals, "Equals");
//  YTEBindEnumValue(Keys::LeftBracket, "LeftBracket");
//  YTEBindEnumValue(Keys::RightBracket, "RightBracket");
//  YTEBindEnumValue(Keys::Backslash, "Backslash");
//  YTEBindEnumValue(Keys::Semicolon, "Semicolon");
//  YTEBindEnumValue(Keys::Apostrophe, "Apostrophe");
//  YTEBindEnumValue(Keys::Grave, "Grave");
//  YTEBindEnumValue(Keys::Comma, "Comma");
//  YTEBindEnumValue(Keys::Period, "Period");
//  YTEBindEnumValue(Keys::ForwardSlash, "ForwardSlash");
//  YTEBindEnumValue(Keys::Capslock, "Capslock");
//  YTEBindEnumValue(Keys::F1, "F1");
//  YTEBindEnumValue(Keys::F2, "F2");
//  YTEBindEnumValue(Keys::F3, "F3");
//  YTEBindEnumValue(Keys::F4, "F4");
//  YTEBindEnumValue(Keys::F5, "F5");
//  YTEBindEnumValue(Keys::F6, "F6");
//  YTEBindEnumValue(Keys::F7, "F7");
//  YTEBindEnumValue(Keys::F8, "F8");
//  YTEBindEnumValue(Keys::F9, "F9");
//  YTEBindEnumValue(Keys::F10, "F10");
//  YTEBindEnumValue(Keys::F11, "F11");
//  YTEBindEnumValue(Keys::F12, "F12");
//  YTEBindEnumValue(Keys::ScrollLock, "ScrollLock");
//  YTEBindEnumValue(Keys::Pause, "Pause");
//  YTEBindEnumValue(Keys::Insert, "Insert");
//  YTEBindEnumValue(Keys::Home, "Home");
//  YTEBindEnumValue(Keys::PageUp, "PageUp");
//  YTEBindEnumValue(Keys::Delete, "Delete");
//  YTEBindEnumValue(Keys::End, "End");
//  YTEBindEnumValue(Keys::PageDown, "PageDown");
//  YTEBindEnumValue(Keys::Right, "Right");
//  YTEBindEnumValue(Keys::Left, "Left");
//  YTEBindEnumValue(Keys::Down, "Down");
//  YTEBindEnumValue(Keys::Up, "Up");
//  YTEBindEnumValue(Keys::KP_Divide, "KP_Divide");
//  YTEBindEnumValue(Keys::KP_Multiply, "KP_Multiply");
//  YTEBindEnumValue(Keys::KP_Minus, "KP_Minus");
//  YTEBindEnumValue(Keys::KP_Plus, "KP_Plus");
//  YTEBindEnumValue(Keys::KP_Enter, "KP_Enter");
//  YTEBindEnumValue(Keys::KP_1, "KP_1");
//  YTEBindEnumValue(Keys::KP_2, "KP_2");
//  YTEBindEnumValue(Keys::KP_3, "KP_3");
//  YTEBindEnumValue(Keys::KP_4, "KP_4");
//  YTEBindEnumValue(Keys::KP_5, "KP_5");
//  YTEBindEnumValue(Keys::KP_6, "KP_6");
//  YTEBindEnumValue(Keys::KP_7, "KP_7");
//  YTEBindEnumValue(Keys::KP_8, "KP_8");
//  YTEBindEnumValue(Keys::KP_9, "KP_9");
//  YTEBindEnumValue(Keys::KP_0, "KP_0");
//  YTEBindEnumValue(Keys::KP_Period, "KP_Period");
//  YTEBindEnumValue(Keys::KP_Equals, "KP_Equals");
//  YTEBindEnumValue(Keys::F13, "F13");
//  YTEBindEnumValue(Keys::F14, "F14");
//  YTEBindEnumValue(Keys::F15, "F15");
//  YTEBindEnumValue(Keys::Mute, "Mute");
//  YTEBindEnumValue(Keys::VolumeUp, "VolumeUp");
//  YTEBindEnumValue(Keys::VolumeDown, "VolumeDown");
//  YTEBindEnumValue(Keys::KP_Comma, "KP_Comma");
//  YTEBindEnumValue(Keys::LeftControl, "LeftControl");
//  YTEBindEnumValue(Keys::LeftShift, "LeftShift");
//  YTEBindEnumValue(Keys::LeftAlt, "LeftAlt");
//  YTEBindEnumValue(Keys::RightControl, "RightControl");
//  YTEBindEnumValue(Keys::RightShift, "RightShift");
//  YTEBindEnumValue(Keys::RightAlt, "RightAlt");
//  YTEBindEnumValue(Keys::Keys_Number, "Keys_Number");
//  YTEBindEnumValue(Keys::Unknown, "Unknown");
}
