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

    mOwner->YTERegister(Events::MenuElementTrigger, this, &ToggleFullscreen::OnElementTrigger);
    mOwner->YTERegister(Events::UpdateVisibilityEvent, this, &ToggleFullscreen::OnParentVisibilityUpdated);
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
