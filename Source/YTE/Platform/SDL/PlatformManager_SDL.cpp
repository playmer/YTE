#include "SDL.h"

#include "YTE/Core/Engine.hpp"

#include "YTE/Platform/SDL/Keyboard_SDL.hpp"
#include "YTE/Platform/SDL/Mouse_SDL.hpp"
#include "YTE/Platform/SDL/Window_SDL.hpp"

#include "YTE/Platform/PlatformManager.hpp"
#include "YTE/Platform/Window.hpp"


// GamePadManager.hpp
namespace YTE
{
  struct GamePadManager
  {
  public:
    GamePadManager();

    inline PrivateImplementationDynamic* GetData()
    {
      return &mData;
    }

  private:
    PrivateImplementationDynamic mData;
  };
}

// GamePadManager_SDL.hpp
namespace YTE
{
  void GamePadManagerEventHandler(SDL_JoyDeviceEvent aEvent);

  struct ControllerData
  {
    char const* mName;
    SDL_GameController* mController;
    int mIndex;
  };
}

// GamePadManager_SDL.cpp
#include <map>

namespace YTE
{
  struct GamePadManagerData
  {
    std::map<int, ControllerData> mControllers;
  };

  void GamePadEventHandler(SDL_JoyDeviceEvent aEvent, GamePadManager* aManager)
  {
    auto self = aManager->GetData()->Get<GamePadManagerData>();
    SDL_GameController * pad;
    switch (aEvent.type)
    {
      case SDL_JOYDEVICEADDED:
      {
        auto errorIsGameController = SDL_IsGameController(aEvent.which);

        if (errorIsGameController)
        {
          pad = SDL_GameControllerOpen(aEvent.which);

          if (pad)
          {
            printf("Device Added: %s\n", SDL_GameControllerName(pad));
          }
        }

        break;
      }
      case SDL_JOYDEVICEREMOVED:
      {
        pad = SDL_GameControllerOpen(aEvent.which);
        printf("Device Removed: %s\n", SDL_GameControllerName(pad));
        return;
      }
    }
  }

  GamePadManager::GamePadManager()
  {
    mData.ConstructAndGet<GamePadManager>();
  }
}


// GamePad.hpp
namespace YTE
{
  struct GamePad
  {
  public:
    GamePad();

  private:
  };
}

// GamePad_SDL.hpp
namespace YTE
{
  void GamePadEventHandler(SDL_JoyDeviceEvent aEvent);
}

// GamePad_SDL.cpp
#include <map>

namespace YTE
{

  void GamePadEventHandler(SDL_JoyDeviceEvent aEvent, GamePad* aGamePad)
  {
    auto self = aGamePad->GetData()->Get<ControllerData>();
    
  }
}







namespace YTE
{
  struct PlatformManagerData
  {
    std::vector<SDL_Event> mEvents;
    std::vector<SDL_GameController*> mControllers;
  };

  void SDLCALL PlatformManagerBlockedUpdate(void* aUserData, SDL_Window*)
  {
    auto self = reinterpret_cast<PlatformManager*>(aUserData);

    auto engine = self->GetEngine();

    if (engine->KeepRunning())
    {
      engine->Update();
    }
  }

  PlatformManager::PlatformManager(Engine* aEngine)
    : mEngine{ aEngine }
    , mMouseFocusedWindow{ nullptr }
    , mKeyboardFocusedWindow{ nullptr }
    , mIsUpdating{ false }
  {
    mData.ConstructAndGet<PlatformManagerData>();
  }

  void PlatformManager::Update()
  {
    mIsUpdating = true;

    //auto size = SDL_NumJoysticks();

    //printf("Controllers: %d\n", size);



    if (false == mEngine->IsEditor())
    {
      SDL_PumpEvents();
    }

    auto self = mData.Get<PlatformManagerData>();
    auto& eventQueue = self->mEvents;

    auto numberOfEvents = SDL_PeepEvents(nullptr, 0, SDL_PEEKEVENT, SDL_FIRSTEVENT, SDL_LASTEVENT);
    eventQueue.resize(numberOfEvents);
    auto error = SDL_PeepEvents(eventQueue.data(), static_cast<int>(eventQueue.size()), SDL_GETEVENT, SDL_FIRSTEVENT, SDL_LASTEVENT);

    UnusedArguments(error);
    DebugAssert(0 <= error, "Couldn't retreive SDL_Events");

    for(auto& event : eventQueue)
    {
      switch (event.type)
      {
        case SDL_KEYDOWN:       // Key pressed
        case SDL_KEYUP:         // Key released
        case SDL_TEXTEDITING:   // Keyboard text editing (composition)
        case SDL_TEXTINPUT:     // Keyboard text input
        case SDL_KEYMAPCHANGED: // Keymap changed due to a system event such as an input language or keyboard layout change.
        {
          if (nullptr != mKeyboardFocusedWindow)
          {
            KeyboardEventHandler(event, &mKeyboardFocusedWindow->mKeyboard);
          }

          break;
        }

        case SDL_MOUSEMOTION:     // Mouse moved
        case SDL_MOUSEBUTTONDOWN: // Mouse button pressed
        case SDL_MOUSEBUTTONUP:   // Mouse button released
        case SDL_MOUSEWHEEL:      // Mouse wheel motion
        {
          if (nullptr != mMouseFocusedWindow)
          {
            MouseEventHandler(event, &mMouseFocusedWindow->mMouse);
          }

          break;
        }

        case SDL_WINDOWEVENT:
        {
          SDL_Window* sdlWindow = SDL_GetWindowFromID(event.window.windowID);

          auto window = static_cast<Window*>(SDL_GetWindowData(sdlWindow, "YTE_Window"));

          WindowEventHandler(event.window, this, window);

          break;
        }
        case SDL_JOYDEVICEADDED:
        case SDL_JOYDEVICEREMOVED:
        {
          GamePadEventHandler(event.jdevice);
          break;
        }
      }
    }

    for (auto& [name, window] : mWindows)
    {
      window->Update();
    }

    eventQueue.clear();

    mIsUpdating = false;
  }
}