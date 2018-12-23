/******************************************************************************/
/*!
* \author Joshua T. Fisher
* \date   6/7/2015
*
* \copyright All content 2016 DigiPen (USA) Corporation, all rights reserved.
*/
/******************************************************************************/
#define K_0 0x30
#define K_1 0x31
#define K_2 0x32
#define K_3 0x33
#define K_4 0x34
#define K_5 0x35
#define K_6 0x36
#define K_7 0x37
#define K_8 0x38
#define K_9 0x39
#define K_A 0x41
#define K_B 0x42
#define K_C 0x43
#define K_D 0x44
#define K_E 0x45
#define K_F 0x46
#define K_G 0x47
#define K_H 0x48
#define K_I 0x49
#define K_J 0x4A
#define K_K 0x4B
#define K_L 0x4C
#define K_M 0x4D
#define K_N 0x4E
#define K_O 0x4F
#define K_P 0x50
#define K_Q 0x51
#define K_R 0x52
#define K_S 0x53
#define K_T 0x54
#define K_U 0x55
#define K_V 0x56
#define K_W 0x57
#define K_X 0x58
#define K_Y 0x59
#define K_Z 0x5A


//#define LBUTTON 0x56
//#define RBUTTON 0x57
//#define MBUTTON 0x58
//#define XBUTTON1 0x59
//#define XBUTTON2 0x5A

ProcessKey(VK_LBUTTON, Mouse_Left)
ProcessKey(VK_RBUTTON, Mouse_Right)
ProcessKey(VK_MBUTTON, Mouse_Middle)
ProcessKey(VK_XBUTTON1, Mouse_Back)
ProcessKey(VK_XBUTTON2, Mouse_Forward)
ProcessKey(K_A, A)
ProcessKey(K_B, B)
ProcessKey(K_C, C)
ProcessKey(K_D, D)
ProcessKey(K_E, E)
ProcessKey(K_F, F)
ProcessKey(K_G, G)
ProcessKey(K_H, H)
ProcessKey(K_I, I)
ProcessKey(K_J, J)
ProcessKey(K_K, K)
ProcessKey(K_L, L)
ProcessKey(K_M, M)
ProcessKey(K_N, N)
ProcessKey(K_O, O)
ProcessKey(K_P, P)
ProcessKey(K_Q, Q)
ProcessKey(K_R, R)
ProcessKey(K_S, S)
ProcessKey(K_T, T)
ProcessKey(K_U, U)
ProcessKey(K_V, V)
ProcessKey(K_W, W)
ProcessKey(K_X, X)
ProcessKey(K_Y, Y)
ProcessKey(K_Z, Z)

ProcessKey(K_1, N1)
ProcessKey(K_2, N2)
ProcessKey(K_3, N3)
ProcessKey(K_4, N4)
ProcessKey(K_5, N5)
ProcessKey(K_6, N6)
ProcessKey(K_7, N7)
ProcessKey(K_8, N8)
ProcessKey(K_9, N9)
ProcessKey(K_0, N0)

ProcessKey(VK_RETURN, Return)
ProcessKey(VK_ESCAPE, Escape)
ProcessKey(VK_BACK, Backspace)
ProcessKey(VK_TAB, Tab)
ProcessKey(VK_SPACE, Space)
ProcessKey(VK_OEM_MINUS, Minus)
ProcessKey(VK_OEM_PLUS, KP_Plus)
ProcessKey(VK_OEM_PERIOD, Period)
ProcessKey(VK_PAUSE, Pause)

ProcessKey('=', Equals)
ProcessKey('[', LeftBracket)
ProcessKey(']', RightBracket)
ProcessKey('\\', Backslash)
ProcessKey(';', Semicolon)
//ProcessKey(, Apostrophe)
//ProcessKey(VK_OEM_GRAVE, Grave)
ProcessKey(VK_OEM_COMMA, Comma)
ProcessKey('/', ForwardSlash)

ProcessKey(VK_SNAPSHOT, PrintScreen)
ProcessKey(VK_INSERT, Insert)
ProcessKey(VK_DELETE, Delete)
ProcessKey(VK_PRIOR, PageUp)
ProcessKey(VK_NEXT, PageDown)
ProcessKey(VK_HOME, Home)
ProcessKey(VK_END, End)

ProcessKey(VK_UP, Up)
ProcessKey(VK_DOWN, Down)
ProcessKey(VK_RIGHT, Right)
ProcessKey(VK_LEFT, Left)

ProcessKey(VK_CAPITAL, Capslock)
ProcessKey(VK_SCROLL, ScrollLock)
ProcessKey(VK_NUMLOCK, NumLock)

//ProcessKey('/', KP_Divide)
ProcessKey('*', KP_Multiply)
ProcessKey(VK_NUMPAD1, KP_1)
ProcessKey(VK_NUMPAD2, KP_2)
ProcessKey(VK_NUMPAD3, KP_3)
ProcessKey(VK_NUMPAD4, KP_4)
ProcessKey(VK_NUMPAD5, KP_5)
ProcessKey(VK_NUMPAD6, KP_6)
ProcessKey(VK_NUMPAD7, KP_7)
ProcessKey(VK_NUMPAD8, KP_8)
ProcessKey(VK_NUMPAD9, KP_9)
ProcessKey(VK_NUMPAD0, KP_0)
//ProcessKey(VK_, KP_Period)
//ProcessKey(VK_, KP_Enter)
//ProcessKey(VK_, KP_Equals)
//ProcessKey(VK_, KP_Comma)

ProcessKey(VK_F1, F1)
ProcessKey(VK_F2, F2)
ProcessKey(VK_F3, F3)
ProcessKey(VK_F4, F4)
ProcessKey(VK_F5, F5)
ProcessKey(VK_F6, F6)
ProcessKey(VK_F7, F7)
ProcessKey(VK_F8, F8)
ProcessKey(VK_F9, F9)
ProcessKey(VK_F10, F10)
ProcessKey(VK_F11, F11)
ProcessKey(VK_F12, F12)
ProcessKey(VK_F13, F13)
ProcessKey(VK_F14, F14)
ProcessKey(VK_F15, F15)

ProcessKey(VK_CONTROL, Control)
ProcessKey(VK_LCONTROL, LeftControl)
ProcessKey(VK_RCONTROL, RightControl)
ProcessKey(VK_SHIFT, Shift)
ProcessKey(VK_LSHIFT, LeftShift)
ProcessKey(VK_RSHIFT, RightShift)
ProcessKey(VK_MENU, Alt)
ProcessKey(VK_LMENU, LeftAlt)
ProcessKey(VK_RMENU, RightAlt)