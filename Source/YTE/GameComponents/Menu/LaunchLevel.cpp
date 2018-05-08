/******************************************************************************/
/*!
* \author Isaac Dayton
* \date   2018/04/11
*
* \copyright All content 2018 DigiPen (USA) Corporation, all rights reserved.
*/
/******************************************************************************/

#include "YTE/Core/Engine.hpp"
#include "YTE/Core/Space.hpp"

#include "YTE/GameComponents/Menu/LaunchLevel.hpp"

namespace YTE
{
  YTEDefineType(LaunchLevel)
  {
    RegisterType<LaunchLevel>();

    YTEBindProperty(&GetLevelToLaunch, &SetLevelToLaunch, "LevelToLaunch")
      .AddAttribute<Serializable>()
      .AddAttribute<EditorProperty>();
  }

  LaunchLevel::LaunchLevel(Composition* aOwner, Space* aSpace, RSValue* aProperties)
    : Component(aOwner, aSpace)
    , mLevelToLaunch("")
  {
    DeserializeByType(aProperties, this, GetStaticType());
  }

  void LaunchLevel::Initialize()
  {
    mOwner->RegisterEvent<&LaunchLevel::OnElementTrigger>(Events::MenuElementTrigger, this);
  }

  void LaunchLevel::OnElementTrigger(MenuElementTrigger *)
  {
    if (!mSpace->GetEngine()->IsEditor())
    {
      String levelToLoad{ mLevelToLaunch.c_str() };
      static_cast<Space*>(mSpace->GetOwner())->LoadLevel(levelToLoad);

      auto emitter = mOwner->GetComponent<WWiseEmitter>();

      if (emitter)
      {
        emitter->PlayEvent("M_Dock_Leave");
        emitter->PlayEvent("Islands_Leave");
      }
    }
  }
}
