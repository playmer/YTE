/******************************************************************************/
/*!
* \author Joshua T. Fisher
* \date   6/7/2015
*
* \copyright All content 2016 DigiPen (USA) Corporation, all rights reserved.
*/
/******************************************************************************/
#include "SDL.h"

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
    glm::i32vec2 toReturn;
    SDL_GetGlobalMouseState(&toReturn.x, &toReturn.y);
    return toReturn;
  }
  
  void Mouse::SetCursorPosition(glm::i32vec2 aPosition)
  {
    SDL_WarpMouseGlobal(aPosition.x, aPosition.y);
  }


  void Mouse::UpdateAllButtons(glm::i32vec2 aRelativePosition)
  {
    glm::i32vec2 position;
    auto buttonState = SDL_GetGlobalMouseState(&position.x, &position.y);

    UpdateButton(MouseButtons::Left, SDL_BUTTON_LMASK & buttonState, aRelativePosition);
    UpdateButton(MouseButtons::Right, SDL_BUTTON_RMASK & buttonState , aRelativePosition);
    UpdateButton(MouseButtons::Middle, SDL_BUTTON_MMASK & buttonState, aRelativePosition);
    UpdateButton(MouseButtons::Forward, SDL_BUTTON_X1MASK & buttonState, aRelativePosition);
    UpdateButton(MouseButtons::Back, SDL_BUTTON_X2MASK & buttonState, aRelativePosition);
  }
}
