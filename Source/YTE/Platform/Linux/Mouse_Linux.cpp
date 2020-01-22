/******************************************************************************/
/*!
* \author Joshua T. Fisher
* \date   6/7/2015
*
* \copyright All content 2016 DigiPen (USA) Corporation, all rights reserved.
*/
/******************************************************************************/
#include "YTE/Platform/TargetDefinitions.hpp"

#include "YTE/Platform/Mouse.hpp"

namespace YTE
{
  uint64_t TranslateFromMouseButtonToOsKey(MouseButtons aOurButton)
  {
    return 0;
  }

  glm::i32vec2 GetMousePosition()
  {
    return {};
  }

  bool GetLRSwapped()
  {
    return false;
  }

  void Mouse::SetCursorPosition(glm::i32vec2 aPosition)
  {
  }
}
