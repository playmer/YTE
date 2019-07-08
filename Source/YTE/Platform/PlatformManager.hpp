#pragma once

#include <optional>

#include "YTE/Core/ForwardDeclarations.hpp"
#include "YTE/Platform/Window.hpp"

namespace YTE
{
  struct PlatformEventHolder
  {
    PlatformEventHolder(std::string const& aEventType,
                        EventHandler* aObject,
                        Event* aEvent)
      : mEventType{aEventType}
      , mObject{aObject}
      , mEvent{aEvent}
    {

    }

    std::string const& mEventType;
    EventHandler* mObject;
    Event* mEvent;
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

  private:
    PrivateImplementationDynamic mData;
    std::unordered_map<std::string, std::unique_ptr<Window>> mWindows;
    std::vector<PlatformEventHolder> mEvents;
    Engine* mEngine;
    Window* mMouseFocusedWindow;
    Window* mKeyboardFocusedWindow;
    bool mIsUpdating;
  };
}