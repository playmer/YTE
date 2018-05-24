/******************************************************************************/
/*!
* \author Isaac Dayton
* \date   2018/03/25
*
* \copyright All content 2018 DigiPen (USA) Corporation, all rights reserved.
*/
/******************************************************************************/

#include "YTE/Physics/Transform.hpp"

#include "YTE/GameComponents/DialogueElementDefault.hpp"

namespace YTE
{
  YTEDefineType(DialogueElementDefault)
  {
    RegisterType<DialogueElementDefault>();
    TypeBuilder<DialogueElementDefault> builder;

    builder.Property<&GetSelectionIndex, &SetSelectionIndex>( "SelectinIndex")
      .AddAttribute<Serializable>()
      .AddAttribute<EditorProperty>();
  }

  DialogueElementDefault::DialogueElementDefault(Composition *aOwner, Space *aSpace, RSValue *aProperties)
    : Component(aOwner, aSpace)
    , mMySprite(nullptr)
    , mTextContent(nullptr)
    , mSelectionIndex(0)
    , mIsDisplayed(false)
  {
    DeserializeByType(aProperties, this, GetStaticType());
  }

  void DialogueElementDefault::Initialize()
  {
    mMySprite = mOwner->GetComponent<Sprite>();

    for (auto const& [name, child] : mOwner->GetCompositions())
    {
      auto spriteTextComponent = child->GetComponent<SpriteText>();
      if (spriteTextComponent != nullptr)
      {
        mTextContent = spriteTextComponent;
        break;
      }
    }

    mSpace->RegisterEvent<&DialogueElementDefault::OnStart>(Events::LogicUpdate, this);

    mSpace->RegisterEvent<&DialogueElementDefault::OnDisplayEvent>(Events::depr_UIDisplayEvent, this);
  }

  void DialogueElementDefault::OnStart(LogicUpdate*)
  {
    UpdateVisibility(false);
    mSpace->DeregisterEvent<&DialogueElementDefault::OnStart>(Events::LogicUpdate,  this);
  }

  void DialogueElementDefault::OnDisplayEvent(depr_UIDisplayEvent *aEvent)
  {
    if (!aEvent->ShouldDisplay && mIsDisplayed)
    {
      UpdateVisibility(false);
    }
    else if (aEvent->ShouldDisplay && !mIsDisplayed)
    {
      if (aEvent->DisplayIndex == mSelectionIndex)
      {
        UpdateVisibility(true);
        mSpace->RegisterEvent<&DialogueElementDefault::OnSelectEvent>(Events::UISelectEvent, this);
      }

      else
      {
        UpdateVisibility(false);
        mSpace->DeregisterEvent<&DialogueElementDefault::OnSelectEvent>(Events::UISelectEvent,  this);
      }
    }
  }

  void DialogueElementDefault::OnSelectEvent(UISelectEvent *aEvent)
  {
    if (aEvent->SelectionIndex == mSelectionIndex)
    {
      if (mIsDisplayed)
      {
        UpdateVisibility(false);
      }
    }
    else
    {
      if (!mIsDisplayed)
      {
        UpdateVisibility(true);
      }
    }
  }

  void DialogueElementDefault::UpdateVisibility(bool aBecomeVisible)
  {
    mIsDisplayed = aBecomeVisible;

    if (mMySprite)
    {
      mMySprite->SetVisibility(aBecomeVisible);
    }

    if (mTextContent)
    {
      mTextContent->SetVisibility(aBecomeVisible);
    }
  }
}
