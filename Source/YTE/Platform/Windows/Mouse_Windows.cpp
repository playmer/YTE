/******************************************************************************/
/*!
* \author Joshua T. Fisher
* \date   6/7/2015
*
* \copyright All content 2016 DigiPen (USA) Corporation, all rights reserved.
*/
/******************************************************************************/
#include "YTE/Platform/TargetDefinitions.hpp"
#ifdef YTE_Windows

#include "YTE/Platform/Windows/WindowsInclude_Windows.hpp"
#include <Winuser.h>

#include "YTE/Platform/Mouse.hpp"

namespace YTE
{
  uint64_t TranslateFromMouseButtonToOsKey(Mouse_Buttons aOurButton)
  {
    switch (aOurButton)
    {
      #define ProcessKey(aOsKey, aOurKey) case (Mouse_Buttons::aOurKey) : return aOsKey;
      #include "YTE/Platform/Windows/OsMouseButtons_Windows.hpp"
      #undef ProcessKey
      default: return 0;
    }
  }

  glm::i32vec2 GetMousePosition()
  {
    POINT point;
    GetCursorPos(&point);

    return { static_cast<i32>(point.x), static_cast<i32>(point.y) };
  }

  bool GetLRSwapped()
  {
    return GetSystemMetrics(SM_SWAPBUTTON) != 0;
  }

  void Mouse::SetCursorPosition(glm::i32vec2 aPosition)
  {
    SetCursorPos(aPosition.x, aPosition.y);
  }
}

#endif
