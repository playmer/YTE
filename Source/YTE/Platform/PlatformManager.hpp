#pragma once

#include <optional>
#include <variant>

#include "YTE/Core/ForwardDeclarations.hpp"

#include "YTE/Platform/GamepadSystem.hpp"
#include "YTE/Platform/Gamepad.hpp"
#include "YTE/Platform/Window.hpp"

namespace YTE
{
  struct PlatformEventHolder
  {

    //PlatformEventHolder(std::string const& aEventType,
    //                    EventHandler* aObject,
    //                    Event* aEvent)
    //  : mEventType{aEventType}
    //  , mObject{aObject}
    //  , mEvent{aEvent}
    //{
    //
    //}

    std::string const& mEventType;
    EventHandler* mObject;
    std::variant<
      GamepadFlickEvent,
      GamepadStickEvent,
      GamepadButtonEvent,
      MouseButtonEvent,
      MouseWheelEvent,
      MouseMoveEvent,
      KeyboardEvent> mEvent;
  };

  class PlatformManager
  {
  public:
    PlatformManager(Engine* aEngine);

    void Update();

    std::optional<Window*> GetPrimaryWindow()
    {
      if (mWindows.size())
      {
        return mWindows.begin()->second.get();
      }

      return std::nullopt;
    }


    std::unordered_map<std::string, std::unique_ptr<Window>>& GetWindows()
    {
      return mWindows;
    }

    void SetKeyboardFocusedWindow(Window* aWindow)
    {
      mKeyboardFocusedWindow = aWindow;
    }

    void SetMouseFocusedWindow(Window* aWindow)
    {
      mMouseFocusedWindow = aWindow;
    }

    Window* GetKeyboardFocusedWindow()
    {
      return mKeyboardFocusedWindow;
    }

    Window* GetMouseFocusedWindow()
    {
      return mMouseFocusedWindow;
    }

    Engine* GetEngine()
    {
      return mEngine;
    }

    GamepadSystem* GetGamepadSystem()
    {
      return &mGamepadSystem;
    }

  private:
    GamepadSystem mGamepadSystem;
    PrivateImplementationDynamic mData;
    std::unordered_map<std::string, std::unique_ptr<Window>> mWindows;
    std::vector<PlatformEventHolder> mEvents;
    Engine* mEngine;
    Window* mMouseFocusedWindow;
    Window* mKeyboardFocusedWindow;
    bool mIsUpdating;
  };
}