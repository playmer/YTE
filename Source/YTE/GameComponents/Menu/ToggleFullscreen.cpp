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

#include "YTE/Graphics/GraphicsView.hpp"

#include "YTE/GameComponents/Menu/ToggleFullscreen.hpp"

namespace YTE
{
  YTEDefineType(ToggleFullscreen)
  {
    RegisterType<ToggleFullscreen>();
    TypeBuilder<ToggleFullscreen> builder;
  }

  ToggleFullscreen::ToggleFullscreen(Composition* aOwner, Space* aSpace, RSValue* aProperties)
    : Component(aOwner, aSpace)
    , mCheckSprite(nullptr)
  {
    DeserializeByType(aProperties, this, GetStaticType());
  }

  void ToggleFullscreen::Initialize()
  {
    if (Composition *childCheck = mOwner->FindFirstCompositionByName("Checkmark"))
    {
      mCheckSprite = childCheck->GetComponent<Sprite>();
    }

    mOwner->RegisterEvent<&ToggleFullscreen::OnElementTrigger>(Events::MenuElementTrigger, this);
    mOwner->RegisterEvent<&ToggleFullscreen::OnParentVisibilityUpdated>(Events::UpdateVisibilityEvent, this);
  }

  void ToggleFullscreen::OnElementTrigger(MenuElementTrigger *)
  {
    auto fullscreen = mSpace->GetComponent<GraphicsView>()->GetWindow()->mFullscreen;
    mSpace->GetComponent<GraphicsView>()->GetWindow()->SetFullscreen(!fullscreen, false);

    if (mCheckSprite)
    {
      mCheckSprite->SetVisibility(!fullscreen);
    }
  }

  void ToggleFullscreen::OnParentVisibilityUpdated(UpdateVisibilityEvent *aEvent)
  {
    if (aEvent->Visibility)
    {
      mCheckSprite->SetVisibility(mSpace->GetComponent<GraphicsView>()->GetWindow()->mFullscreen);
    }
    else
    {
      mCheckSprite->SetVisibility(false);
    }
  }
}
