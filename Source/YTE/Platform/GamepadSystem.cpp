/******************************************************************************/
/*!
 * \author Joshua T. Fisher
 * \date   2015-6-7
 *
 * \copyright All content 2016 DigiPen (USA) Corporation, all rights reserved.
 */
/******************************************************************************/
#include "YTE/Core/ScriptBind.hpp"

#include "YTE/Platform/GamepadSystem.hpp"

namespace YTE
{
  YTEDefineType(GamepadSystem)
  {
    RegisterType<GamepadSystem>();
    TypeBuilder<GamepadSystem> builder;

    builder.Function<&GamepadSystem::GetXboxController>("GetXboxController")
      .SetParameterNames("aController")
      .SetDocumentation("Gets you the given controller. Warning: Controller may not be plugged in/active.");
  }

  GamepadSystem::GamepadSystem()
    : mChecking{ true }
  {
    for (uint8_t i = 0; i < 4; ++i)
    {
      mXboxControllers[i].first.mGamepadIndex = i;

      // Make the second higher than 4 so that it updates the controllers immediately.
      mXboxControllers[i].second = 5.0f;
    }
  }

  void GamepadSystem::Update(double aDt)
  {
    UpdateXboxControllers(aDt);
  }
    
  XboxController* GamepadSystem::GetXboxController(ControllerId aId)
  {
    if (aId >= ControllerId::Xbox_Controllers_Number)
    {
      return nullptr;
    }

    return &mXboxControllers.at(static_cast<size_t>(aId)).first;
  }
}
