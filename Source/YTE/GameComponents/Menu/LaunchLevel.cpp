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
    YTERegisterType(LaunchLevel);

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
    mOwner->YTERegister(Events::MenuElementTrigger, this, &LaunchLevel::OnElementTrigger);
  }

  void LaunchLevel::OnElementTrigger(MenuElementTrigger *)
  {
    String levelToLoad{ mLevelToLaunch.c_str() };
    static_cast<Space*>(mSpace->GetOwner())->LoadLevel(levelToLoad);
  }
}
