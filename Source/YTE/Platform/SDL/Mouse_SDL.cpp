#include "SDL.h"

#include "YTE/Platform/TargetDefinitions.hpp"

#include "YTE/Platform/SDL/Window_SDL.hpp"

#include "YTE/Platform/Mouse.hpp"

namespace YTE
{
  MouseButtons SdlToOurButton(Uint8 aButton)
  {
    switch (aButton)
    {
      case SDL_BUTTON_LEFT:  return MouseButtons::Left;
      case SDL_BUTTON_RIGHT: return MouseButtons::Right;
      case SDL_BUTTON_MIDDLE: return MouseButtons::Middle;
      case SDL_BUTTON_X1: return MouseButtons::Back;
      case SDL_BUTTON_X2: return MouseButtons::Forward;
      default: return MouseButtons::Unknown;
    }
  }

  void MouseEventHandler(SDL_Event aEvent, Window* aWindow, Mouse* aMouse)
  {
    switch (aEvent.type)
    {
        // Mouse moved
      case SDL_MOUSEMOTION:
      {
        glm::i32vec2 position{ aEvent.motion.x, aEvent.motion.y };
        aMouse->UpdatePosition(position);
        break;
      }

      // Mouse button pressed
      case SDL_MOUSEBUTTONDOWN:
      {
        auto button = SdlToOurButton(aEvent.button.button);
        glm::i32vec2 position{ aEvent.button.x, aEvent.button.y };
        aMouse->UpdateButton(button, true, position);
        break;
      }

      // Mouse button released
      case SDL_MOUSEBUTTONUP:
      {
        auto button = SdlToOurButton(aEvent.button.button);
        glm::i32vec2 position{ aEvent.button.x, aEvent.button.y };
        aMouse->UpdateButton(button, false, position);
        break;
      }

      // Mouse wheel motion
      case SDL_MOUSEWHEEL:
      {
        glm::vec2 scrollMove;
        scrollMove.x = aEvent.wheel.x;
        scrollMove.y = aEvent.wheel.y;

        aMouse->UpdateWheel(scrollMove);

        break;
      }
    }
  }


  glm::i32vec2 GetMousePosition()
  {
    glm::i32vec2 toReturn;
    SDL_GetGlobalMouseState(&toReturn.x, &toReturn.y);
    return toReturn;
  }
  
  void Mouse::SetCursorPosition(glm::i32vec2 aPosition)
  {
    auto self = mWindow->mData.Get<WindowData>();
    int x;
    int y;

    SDL_GetWindowPosition(self->mWindow, &x, &y);
    SDL_WarpMouseGlobal(aPosition.x + x, aPosition.y + y);
  }


  void Mouse::UpdateAllButtons(glm::i32vec2 aRelativePosition)
  {
    glm::i32vec2 position;
    auto buttonState = SDL_GetGlobalMouseState(&position.x, &position.y);

    UpdateButton(MouseButtons::Left, SDL_BUTTON_LMASK & buttonState, aRelativePosition);
    UpdateButton(MouseButtons::Right, SDL_BUTTON_RMASK & buttonState , aRelativePosition);
    UpdateButton(MouseButtons::Middle, SDL_BUTTON_MMASK & buttonState, aRelativePosition);
    UpdateButton(MouseButtons::Back, SDL_BUTTON_X1MASK & buttonState, aRelativePosition);
    UpdateButton(MouseButtons::Forward, SDL_BUTTON_X2MASK & buttonState, aRelativePosition);
  }
}
