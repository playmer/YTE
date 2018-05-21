/******************************************************************************/
/*!
* \author Isaac Dayton
* \date   2018/03/15
*
* \copyright All content 2018 DigiPen (USA) Corporation, all rights reserved.
*/
/******************************************************************************/

#include "YTE/Physics/Transform.hpp"

#include "YTE/GameComponents/DialogueElement.hpp"

namespace YTE
{
  static std::vector<std::string> PopulateDropDownList(Component *)
  {
    return ContentTypeStrings;
  }

  YTEDefineType(DialogueElement)
  {
    RegisterType<DialogueElement>();
    TypeBuilder<DialogueElement> builder;

    builder.Property<&GetContentType, &SetContentType>( "ContentType")
      .AddAttribute<EditorProperty>()
      .AddAttribute<Serializable>()
      .AddAttribute<DropDownStrings>(PopulateDropDownList);

    builder.Property<&GetSelectionIndex, &SetSelectionIndex>( "SelectionIndex")
      .AddAttribute<Serializable>()
      .AddAttribute<EditorProperty>();
  }

  DialogueElement::DialogueElement(Composition *aOwner, Space *aSpace, RSValue *aProperties)
    : Component(aOwner, aSpace)
    , mMySprite(nullptr)
    , mAcceptSprite(nullptr)
    , mNextSprite(nullptr)
    , mPrevSprite(nullptr)
    , mTextContent(nullptr)
    , mContentType(ContentType::Passive)
    , mSelectionIndex(0)
    , mIsDisplayed(false)
  {
    DeserializeByType(aProperties, this, GetStaticType());
  }

  void DialogueElement::Initialize()
  {
    //mTransform = mOwner->GetComponent<Transform>();
    //mText = mOwner->FindFirstCompositionByName("Text")->GetComponent<SpriteText>();

    //mInitialPos = mTransform->GetWorldTranslation();
    //mSelectString = mText->GetText();

      // Initialize to not being displayed;
    //mTransform->SetWorldTranslation(0.f, -60.f, 0.f);

    mMySprite = mOwner->GetComponent<Sprite>();

    if (Composition *accept = mOwner->FindFirstCompositionByName("Accept"))
    {
      mAcceptSprite = accept->GetComponent<Sprite>();
    }
    if (Composition *next = mOwner->FindFirstCompositionByName("Next"))
    {
      mNextSprite = next->GetComponent<Sprite>();
    }
    if (Composition *prev = mOwner->FindFirstCompositionByName("Prev"))
    {
      mPrevSprite = prev->GetComponent<Sprite>();
    }
    if (Composition *text = mOwner->FindFirstCompositionByName("Text"))
    {
      mTextContent = text->GetComponent<SpriteText>();
    }

    /*
    for (auto const& [name, child] : mOwner->GetCompositions())
    {
      auto spriteTextComponent = child->GetComponent<SpriteText>();
      if (spriteTextComponent != nullptr)
      {
        mTextContent = spriteTextComponent;
      }

      auto spriteComponent = child->GetComponent<Sprite>();
      if (spriteComponent != nullptr)
      {
        mChildSprite = spriteComponent;
      }
    }*/

    mSpace->RegisterEvent<&DialogueElement::OnStart>(Events::LogicUpdate, this);
    mSpace->RegisterEvent<&DialogueElement::OnContentUpdate>(Events::UIUpdateContent, this);

    if (mContentType == ContentType::Passive)
    {
      mSelectionIndex = 0;
      mSpace->RegisterEvent<&DialogueElement::OnDisplayEvent>(Events::UIDisplayEvent, this);
      mSpace->RegisterEvent<&DialogueElement::OnFocusSwitch>(Events::UIFocusSwitchEvent, this);
    }
    else if (mContentType == ContentType::Active)
    {
      mSpace->RegisterEvent<&DialogueElement::OnSelectEvent>(Events::UISelectEvent, this);
    }
  }

  void DialogueElement::OnStart(LogicUpdate*)
  {
    UpdateVisibility(false);
    mSpace->DeregisterEvent<&DialogueElement::OnStart>(Events::LogicUpdate,  this);
  }

  void DialogueElement::OnContentUpdate(UIUpdateContent *aEvent)
  {
    if (aEvent->IsPassive && mContentType == ContentType::Passive)
    {
      mTextContent->SetText(aEvent->ContentMessage);
    }
    else if (!aEvent->IsPassive && mContentType == ContentType::Active)
    {
      if (aEvent->SelectionIndex == mSelectionIndex)
      {
        mTextContent->SetText(aEvent->ContentMessage);
      }
    }
  }

  void DialogueElement::OnFocusSwitch(UIFocusSwitchEvent *aEvent)
  {
    if (mAcceptSprite != nullptr)
    {
      if (aEvent->IsPassiveFocused)
      {
        mAcceptSprite->SetVisibility(true);
      }
      else
      {
        mAcceptSprite->SetVisibility(false);
      }
    }
  }

  void DialogueElement::OnDisplayEvent(UIDisplayEvent *aEvent)
  {
    if (aEvent->ShouldDisplay != mIsDisplayed)
    {
      UpdateVisibility(aEvent->ShouldDisplay);
    }
  }

  void DialogueElement::OnSelectEvent(UISelectEvent *aEvent)
  {
    if (aEvent->SelectionIndex == mSelectionIndex)
    {
      if (!mIsDisplayed)
      {
        UpdateVisibility(true);

        if (aEvent->NumOptions == 1)
        {
          mNextSprite->SetVisibility(false);
          mPrevSprite->SetVisibility(false);
        }
      }
    }
    else
    {
      if (mIsDisplayed)
      {
        UpdateVisibility(false);
      }
    }
  }

  void DialogueElement::UpdateVisibility(bool aBecomeVisible)
  {
    mIsDisplayed = aBecomeVisible;

    if (mMySprite)
    {
      mMySprite->SetVisibility(aBecomeVisible);
    }

    if (mAcceptSprite)
    {
      mAcceptSprite->SetVisibility(aBecomeVisible);
    }

    if (mNextSprite)
    {
      mNextSprite->SetVisibility(aBecomeVisible);
    }

    if (mPrevSprite)
    {
      mPrevSprite->SetVisibility(aBecomeVisible);
    }

    if (mTextContent)
    {
      mTextContent->SetVisibility(aBecomeVisible);
    }
  }
}
