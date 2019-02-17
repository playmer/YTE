#include "SDL.h"

#include "YTE/Core/Engine.hpp"

#include "YTE/Platform/SDL/Keyboard_SDL.hpp"
#include "YTE/Platform/SDL/Mouse_SDL.hpp"
#include "YTE/Platform/SDL/Window_SDL.hpp"

#include "YTE/Platform/PlatformManager.hpp"
#include "YTE/Platform/Window.hpp"


namespace YTE
{
  void PlatformManager::Update()
  {
    SDL_Event event;

    while( SDL_PollEvent( &event) != 0 )
    {
      switch (event.type)
      {
        case SDL_KEYDOWN: /**< Key pressed */
        case SDL_KEYUP:                  /**< Key released */
        case SDL_TEXTEDITING:            /**< Keyboard text editing (composition) */
        case SDL_TEXTINPUT:              /**< Keyboard text input */
        case SDL_KEYMAPCHANGED:          /**< Keymap changed due to a system event such as an input language or keyboard layout change. */
        {
          if (nullptr != mKeyboardFocusedWindow)
          {
            KeyboardEventHandler(event, mKeyboardFocusedWindow, &mKeyboardFocusedWindow->mKeyboard);
          }

          break;
        }

        case SDL_MOUSEMOTION: /**< Mouse moved */
        case SDL_MOUSEBUTTONDOWN:        /**< Mouse button pressed */
        case SDL_MOUSEBUTTONUP:          /**< Mouse button released */
        case SDL_MOUSEWHEEL:             /**< Mouse wheel motion */
        {
          if (nullptr != mMouseFocusedWindow)
          {
            MouseEventHandler(event, mMouseFocusedWindow, &mMouseFocusedWindow->mMouse);
          }

          break;
        }

        case SDL_WINDOWEVENT:           /**< Never used */
        {
          SDL_Window* sdlWindow = SDL_GetWindowFromID(event.window.windowID);

          auto window = static_cast<Window*>(SDL_GetWindowData(sdlWindow, "YTE_Window"));

          WindowEventHandler(event.window.event, this, window);

          break;
        }
      }
    }

    for (auto& [name, window] : mWindows)
    {
      window->Update();
    }
  }
}