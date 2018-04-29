/******************************************************************************/
/*!
* \author Isaac Dayton
* \date   2018/04/17
*
* \copyright All content 2018 DigiPen (USA) Corporation, all rights reserved.
*/
/******************************************************************************/

#include "YTE/Core/Engine.hpp"
#include "YTE/Core/Space.hpp"

#include "YTE/Graphics/Model.hpp"
#include "YTE/Graphics/Sprite.hpp"
#include "YTE/Graphics/SpriteText.hpp"

#include "YTE/GameComponents/InheritVisibility.hpp"

namespace YTE
{
  YTEDefineType(InheritVisibility)
  {
    RegisterType<InheritVisibility>();

    std::vector<std::vector<Type*>> deps = {
      { TypeId<Model>(),
        TypeId<Sprite>(),
        TypeId<SpriteText>()
      }
    };

    GetStaticType()->AddAttribute<ComponentDependencies>(deps);
  }

  InheritVisibility::InheritVisibility(Composition* aOwner, Space* aSpace, RSValue* aProperties)
    : Component(aOwner, aSpace)
  {
    DeserializeByType(aProperties, this, GetStaticType());
  }

  void InheritVisibility::Initialize()
  {
    mOwner->YTERegister(Events::UpdateVisibilityEvent, this, &InheritVisibility::OnParentVisibilityUpdate);
  }

  void InheritVisibility::OnParentVisibilityUpdate(UpdateVisibilityEvent *aEvent)
  {
    if (Sprite *sprite = mOwner->GetComponent<Sprite>())
    {
      sprite->SetVisibility(aEvent->Visibility);
    }
    else if (SpriteText *spriteText = mOwner->GetComponent<SpriteText>())
    {
      spriteText->SetVisibility(aEvent->Visibility);
    }
    else if (Model *model = mOwner->GetComponent<Model>())
    {
      model->SetVisibility(aEvent->Visibility);
    }

    auto children = mOwner->GetCompositions()->All();
    for (auto &child : children)
    {
      UpdateVisibilityEvent visibilityUpdated(aEvent->Visibility);
      child.second->SendEvent(Events::UpdateVisibilityEvent, &visibilityUpdated);
    }
  }
}
