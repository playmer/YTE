#pragma once

#include <optional>

#include "YTE/Core/ForwardDeclarations.hpp"
#include "YTE/Platform/Window.hpp"

namespace YTE
{
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

  private:
    PrivateImplementationDynamic mData;
    std::unordered_map<std::string, std::unique_ptr<Window>> mWindows;
    Engine* mEngine;
    Window* mMouseFocusedWindow;
    Window* mKeyboardFocusedWindow;
  };
}