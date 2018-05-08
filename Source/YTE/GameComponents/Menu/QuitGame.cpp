/******************************************************************************/
/*!
* \author Isaac Dayton
* \date   2018/04/13
*
* \copyright All content 2018 DigiPen (USA) Corporation, all rights reserved.
*/
/******************************************************************************/

#include "YTE/Core/Engine.hpp"
#include "YTE/Core/Space.hpp"

#include "YTE/GameComponents/Menu/QuitGame.hpp"

namespace YTE
{
  YTEDefineType(QuitGame)
  {
    RegisterType<QuitGame>();
    TypeBuilder<QuitGame> builder;
  }

  QuitGame::QuitGame(Composition* aOwner, Space* aSpace, RSValue* aProperties)
    : Component(aOwner, aSpace)
  {
    DeserializeByType(aProperties, this, GetStaticType());
  }

  void QuitGame::Initialize()
  {
    mOwner->RegisterEvent<&QuitGame::OnElementTrigger>(Events::MenuElementTrigger, this);
  }

  void QuitGame::OnElementTrigger(MenuElementTrigger *)
  {
    auto engine = mOwner->GetEngine();

    if (!engine->IsEditor())
    {
      engine->EndExecution();
    }
  }
}
