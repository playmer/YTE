#include "YTE/Core/ScriptBind.hpp"

#include "YTE/Platform/GamepadSystem.hpp"

namespace YTE
{
  YTEDefineType(GamepadSystem)
  {
    RegisterType<GamepadSystem>();
    TypeBuilder<GamepadSystem> builder;

    //builder.Function<&GamepadSystem::GetXboxController>("GetXboxController")
    //  .SetParameterNames("aController")
    //  .SetDocumentation("Gets you the given controller. Warning: Controller may not be plugged in/active.");
  }

  GamepadSystem::GamepadSystem()
  {
    PlatformInitialize();
  }

  Gamepad* GamepadSystem::GetGamepad(i64 aId)
  {
    auto it = mGamepads.find(aId);

    if (it != mGamepads.end())
    {
      return &it->second;
    }

    return nullptr;
  }

  void GamepadSystem::UpdateGamepads(double aDt)
  {
    for (auto& [instanceId, gamepad] : mGamepads)
    {
      gamepad.Update(aDt);
    }
  }

  void GamepadSystem::PreUpdateGamepads()
  {
    for (auto& [instanceId, gamepad] : mGamepads)
    {
      gamepad.PreUpdate();
    }
  }
}
