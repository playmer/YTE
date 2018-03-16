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
  YTEDefineType(DialogueElement)
  {
    YTERegisterType(DialogueElement);

    YTEBindProperty(&GetCanDeselect, &SetCanDeselect, "CanBeDeselected")
      .AddAttribute<Serializable>()
      .AddAttribute<EditorProperty>();
  }


  DialogueElement::DialogueElement(Composition *aOwner, Space *aSpace, RSValue *aProperties)
    : Component(aOwner, aSpace)
  {
    mCanDeselect = false;
    DeserializeByType(aProperties, this, GetStaticType());
  }

  void DialogueElement::Initialize()
  {
    mTransform = mOwner->GetComponent<Transform>();
    mText = mOwner->FindFirstCompositionByName("Text")->GetComponent<SpriteText>();

    mInitialPos = mTransform->GetWorldTranslation();
    mSelectString = mText->GetText();

      // Initialize to not being displayed;
    //mTransform->SetWorldTranslation(0.f, -60.f, 0.f);

    mOwner->YTERegister(Events::UIDisplayEvent, this, &DialogueElement::OnDisplayEvent);
    mOwner->YTERegister(Events::UIUpdateContent, this, &DialogueElement::OnContentUpdate);
    mOwner->YTERegister(Events::UISelectEvent, this, &DialogueElement::OnSelectEvent);
    mOwner->YTERegister(Events::UIConfirmEvent, this, &DialogueElement::OnConfirmEvent);
  }

  void DialogueElement::OnDisplayEvent(UIDisplayEvent *aEvent)
  {
    if (aEvent->ShouldDisplay)
    {
      if (mCanDeselect)
      {
        mText->SetText(mDeselectString);
      }
      else
      {
        mText->SetText(mSelectString);
      }

     // mTransform->SetWorldTranslation(mInitialPos);
    }
    else
    {
      //mTransform->SetWorldTranslation(glm::vec3(0.f, -60.f, 0.f));
    }
  }

  void DialogueElement::OnContentUpdate(UIUpdateContent *aEvent)
  {
    mSelectString = aEvent->ContentMessage;
    mText->SetText(mSelectString);
  }

  void DialogueElement::OnSelectEvent(UISelectEvent *aEvent)
  {
    if (aEvent->SelectOrDeselect)
    {
      mText->SetText(mSelectString);
    }
    else if (mCanDeselect)
    {
      mText->SetText(mDeselectString);
    }
  }

  void DialogueElement::OnConfirmEvent(UIConfirmEvent *aEvent)
  {

  }
}
