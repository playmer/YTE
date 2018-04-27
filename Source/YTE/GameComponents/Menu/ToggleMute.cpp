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

#include "YTE/WWise/WWiseEmitter.hpp"

#include "YTE/GameComponents/Menu/ToggleMute.hpp"

namespace YTE
{
  YTEDefineType(ToggleMute)
  {
    YTERegisterType(ToggleMute);

    std::vector<std::vector<Type*>> deps = { { TypeId<WWiseEmitter>() } };

    GetStaticType()->AddAttribute<ComponentDependencies>(deps);
  }

  ToggleMute::ToggleMute(Composition* aOwner, Space* aSpace, RSValue* aProperties)
    : Component(aOwner, aSpace)
    , mCheckSprite(nullptr)
    , mIsMuted(false)
  {
    DeserializeByType(aProperties, this, GetStaticType());
  }

  void ToggleMute::Initialize()
  {
    if (Composition *childCheck = mOwner->FindFirstCompositionByName("Checkmark"))
    {
      mCheckSprite = childCheck->GetComponent<Sprite>();
    }

    mOwner->YTERegister(Events::MenuElementTrigger, this, &ToggleMute::OnElementTrigger);
    mOwner->YTERegister(Events::UpdateVisibilityEvent, this, &ToggleMute::OnParentVisibilityUpdated);
    mSpace->YTERegister(Events::MuteBypass, this, &ToggleMute::OnMuteBypass);
  }

  void ToggleMute::OnElementTrigger(MenuElementTrigger *)
  {
    if (!mIsMuted)
    {
      mOwner->GetComponent<WWiseEmitter>()->PlayEvent("Music_Mute");
    }
    else
    {
      mOwner->GetComponent<WWiseEmitter>()->PlayEvent("Music_Unmute");
    }

    mIsMuted = !mIsMuted;

    if (mCheckSprite)
    {
      mCheckSprite->SetVisibility(mIsMuted);
    }
  }

  void ToggleMute::OnParentVisibilityUpdated(UpdateVisibilityEvent *aEvent)
  {
    if (aEvent->Visibility)
    {
      mCheckSprite->SetVisibility(mIsMuted);
    }
    else
    {
      mCheckSprite->SetVisibility(false);
    }
  }

  void ToggleMute::OnMuteBypass(MuteBypass *)
  {
    MenuElementTrigger lmao;
    this->OnElementTrigger(&lmao);

    mCheckSprite->SetVisibility(false);
  }
}