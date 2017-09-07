/******************************************************************************/
/*!
 * \author Joshua T. Fisher
 * \date   6/7/2015
 *
 * \copyright All content 2016 DigiPen (USA) Corporation, all rights reserved.
 */
/******************************************************************************/
#include "YTE/Platform/TargetDefinitions.hpp"
#ifdef Windows

#include <YTE/Platform/Windows/WindowsInclude.hpp>
#include <Winuser.h>
#include "YTE/Platform/Mouse.hpp"

namespace YTE
{
  glm::vec2 GetMousePosition()
  {
    POINT point;
    GetCursorPos(&point);

    return { static_cast<float>(point.x), static_cast<float>(point.y) };
  }

  bool GetLRSwapped()
  {
    return GetSystemMetrics(SM_SWAPBUTTON) != 0;
  }
}

#endif