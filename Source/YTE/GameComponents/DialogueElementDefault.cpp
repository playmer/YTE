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
    YTERegisterType(DialogueElementDefault);

    YTEBindProperty(&GetSelectionIndex, &SetSelectionIndex, "SelectinIndex")
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

    auto children = mOwner->GetCompositions()->All();
    for (auto &child : children)
    {
      auto spriteTextComponent = child.second->GetComponent<SpriteText>();
      if (spriteTextComponent != nullptr)
      {
        mTextContent = spriteTextComponent;
        break;
      }
    }

    mSpace->YTERegister(Events::LogicUpdate, this, &DialogueElementDefault::OnStart);

    mSpace->YTERegister(Events::UIDisplayEvent, this, &DialogueElementDefault::OnDisplayEvent);
  }

  void DialogueElementDefault::OnStart(LogicUpdate*)
  {
    UpdateVisibility(false);
    mSpace->YTEDeregister(Events::LogicUpdate, this, &DialogueElementDefault::OnStart);
  }

  void DialogueElementDefault::OnDisplayEvent(UIDisplayEvent *aEvent)
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
        mSpace->YTERegister(Events::UISelectEvent, this, &DialogueElementDefault::OnSelectEvent);
      }

      else
      {
        UpdateVisibility(false);
        mSpace->YTEDeregister(Events::UISelectEvent, this, &DialogueElementDefault::OnSelectEvent);
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
