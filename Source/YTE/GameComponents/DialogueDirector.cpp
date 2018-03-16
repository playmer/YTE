/******************************************************************************/
/*!
* \author Isaac Dayton
* \date   2018/03/15
*
* \copyright All content 2018 DigiPen (USA) Corporation, all rights reserved.
*/
/******************************************************************************/

#include "YTE/Graphics/Camera.hpp"

#include "YTE/Physics/Orientation.hpp"

#include "YTE/Physics/Transform.hpp"


#include "YTE/GameComponents/DialogueDirector.hpp"

namespace YTE
{
  /*YTEDefineEvent(DialogueStart);
  YTEDefineType(DialogueStart) { YTERegisterType(DialogueStart); }*/

  YTEDefineEvent(UISelectEvent);
  YTEDefineEvent(UIConfirmEvent);
  YTEDefineEvent(UIDisplayEvent);
  YTEDefineEvent(UIUpdateContent);

  YTEDefineType(UISelectEvent) { YTERegisterType(UISelectEvent); }
  YTEDefineType(UIConfirmEvent) { YTERegisterType(UIConfirmEvent); }
  YTEDefineType(UIDisplayEvent) { YTERegisterType(UIDisplayEvent); }
  YTEDefineType(UIUpdateContent) { YTERegisterType(UIUpdateContent); }

  YTEDefineType(DialogueDirector)
  {
    YTERegisterType(DialogueDirector);

    YTEBindProperty(&GetCamAnchor, &SetCamAnchor, "CameraPos")
      .AddAttribute<Serializable>()
      .AddAttribute<EditorProperty>();

    YTEBindProperty(&GetPlayerMark, &SetPlayerMark, "PlayerMark")
      .AddAttribute<Serializable>()
      .AddAttribute<EditorProperty>();

    YTEBindProperty(&GetCharMark, &SetCharMark, "CharacterMark")
      .AddAttribute<Serializable>()
      .AddAttribute<EditorProperty>();

    //std::vector<std::vector<Type*>> deps =
    //{
    //};
    //Dialogue::GetStaticType()->AddAttribute<ComponentDependencies>(deps);
  }


  DialogueDirector::DialogueDirector(Composition *aOwner, Space *aSpace, RSValue *aProperties)
    : Component(aOwner, aSpace), mDockAnchorPosition(0.f), mCameraAnchorPosition(0.f), mActive(false)
  {
    mLastSelected = nullptr;

    DeserializeByType(aProperties, this, GetStaticType());
  }

  void DialogueDirector::Initialize()
  {
    mDockAnchorPosition = mOwner->GetComponent<Transform>()->GetWorldTranslation();
    mCameraAnchorPosition = mOwner->FindFirstCompositionByName("CameraAnchor")->GetComponent<Transform>()->GetWorldTranslation();

    mCharacterDialogue = mOwner->FindFirstCompositionByName("DiaCharacter");
    mDialogueOption1 = mOwner->FindFirstCompositionByName("DiaPlayer1");
    mDialogueOption2 = mOwner->FindFirstCompositionByName("DiaPlayer2");
    mDialogueOption3 = mOwner->FindFirstCompositionByName("DiaPlayer3");
    
      // @@@JAY
      // @@@NICK: Uncomment these if you have a child object representing the character mark points
    //mCameraAnchorPosition = mOwner->FindFirstCompositionByName("PlayerMark")->GetComponent<Transform>()->GetWorldTranslation();
    //mCameraAnchorPosition = mOwner->FindFirstCompositionByName("CharacterMark")->GetComponent<Transform>()->GetWorldTranslation();

    mSpace->YTERegister(Events::RequestDialogueStart, this, &DialogueDirector::OnRequestDialogueStart);
    mSpace->YTERegister(Events::DialogueSelect, this, &DialogueDirector::OnDialogueSelect);
    mSpace->YTERegister(Events::DialogueConfirm, this, &DialogueDirector::OnDialogueConfirm);
    mSpace->YTERegister(Events::DialogueExit, this, &DialogueDirector::OnDialogueExit);

    mOwner->YTERegister(Events::CollisionPersisted, this, &DialogueDirector::OnCollisionPersist);
    mOwner->YTERegister(Events::CollisionStarted, this, &DialogueDirector::OnCollisionStart);
    mOwner->YTERegister(Events::CollisionEnded, this, &DialogueDirector::OnCollisionEnd);
  }

  void DialogueDirector::Update(LogicUpdate *aEvent)
  {
    YTEUnusedArgument(aEvent);
  }

  void DialogueDirector::OnCollisionPersist(CollisionPersisted * aEvent)
  {
    YTEUnusedArgument(aEvent);
  }

  void DialogueDirector::OnCollisionStart(CollisionStarted * aEvent)
  {
    if (aEvent->OtherObject->GetComponent<BoatController>())
    {
      mActive = true;
    }
  }

  void DialogueDirector::OnCollisionEnd(CollisionEnded * aEvent)
  {
    if (aEvent->OtherObject->GetComponent<BoatController>())
    {
      mActive = false;
    }
  }

  void DialogueDirector::OnRequestDialogueStart(RequestDialogueStart *aEvent)
  {
    if (mActive && !aEvent->EventHandled)
    {
      aEvent->EventHandled = true;

      BoatDockEvent dockEvent(mDockAnchorPosition, mOwner->GetComponent<Orientation>()->GetForwardVector());
      mSpace->SendEvent(Events::BoatDockEvent, &dockEvent);

      DirectCameraEvent directCam(mCameraAnchorPosition, mOwner->GetParent()->GetComponent<Transform>()->GetWorldTranslation());
      mSpace->SendEvent(Events::DirectCameraEvent, &directCam);

      mSpace->GetComponent<InputInterpreter>()->SetInputContext(InputInterpreter::InputContext::Dialogue);

      // @@@JAY: Send your initial event here!!

        // Send an UIUpdateContent event for the opening line
      std::string message /* = your message from the event you sent */;
      UIUpdateContent content(message);
      mCharacterDialogue->SendEvent(Events::UIUpdateContent, &content);

        // Then I open up all the speech bubbles
      UIDisplayEvent display(true);

      auto children = mOwner->GetCompositions()->All();
      for (auto &child : children)
      {
        child.second->SendEvent(Events::UIDisplayEvent, &display);
      }
    }
  }

  void DialogueDirector::OnDialogueSelect(DialogueSelect *aEvent)
  {
    if (mActive && !aEvent->EventHandled)
    {
      aEvent->EventHandled = true;

      float stickAngle = glm::acos(glm::dot(glm::vec2(1.f, 0.f), aEvent->StickDirection));
      float pi = glm::pi<float>();

      if (aEvent->StickDirection.y > 0.0f)
      {
        if (stickAngle >= 0.f && stickAngle < (pi / 3.0f))
        {
          if (mLastSelected)
          {
            UISelectEvent select(false);
            mLastSelected->SendEvent(Events::UISelectEvent, &select);
          }

          UISelectEvent select(true);
          mDialogueOption2->SendEvent(Events::UISelectEvent, &select);
          mLastSelected = mDialogueOption2;
        }
        else if (stickAngle >= (pi / 3.0f) && stickAngle < (2.0f * pi / 3.0f))
        {
          if (mLastSelected)
          {
            UISelectEvent select(false);
            mLastSelected->SendEvent(Events::UISelectEvent, &select);
          }

          UISelectEvent select(true);
          mDialogueOption1->SendEvent(Events::UISelectEvent, &select);
          mLastSelected = mDialogueOption1;
        }
        else if (stickAngle >= (2.0f * pi / 3.0f) && stickAngle < pi)
        {
          if (mLastSelected)
          {
            UISelectEvent select(false);
            mLastSelected->SendEvent(Events::UISelectEvent, &select);
          }

          UISelectEvent select(true);
          mDialogueOption3->SendEvent(Events::UISelectEvent, &select);
          mLastSelected = mDialogueOption3;
        }
      }
      else if (stickAngle >= -(pi / 3.0f) && stickAngle < (2.0f * pi / 3.0f))
      {
          // If the user presses "down", deselect all options
        UISelectEvent select(false);
        mDialogueOption1->SendEvent(Events::UISelectEvent, &select);
        mDialogueOption2->SendEvent(Events::UISelectEvent, &select);
        mDialogueOption3->SendEvent(Events::UISelectEvent, &select);

        mLastSelected = nullptr;
      }
    }
  }

  void DialogueDirector::OnDialogueConfirm(DialogueConfirm *aEvent)
  {
    if (mActive && !aEvent->EventHandled)
    {
      aEvent->EventHandled = true;

      auto emitter = mOwner->GetComponent<WWiseEmitter>();

      if (emitter)
      {
        emitter->PlayEvent("UI_Dia_Next");
      }
    }
  }

  void DialogueDirector::OnDialogueExit(DialogueExit *aEvent)
  {
    if (mActive && !aEvent->EventHandled)
    {
      aEvent->EventHandled = true;

      auto emitter = mOwner->GetComponent<WWiseEmitter>();

      if (emitter)
      {
        emitter->PlayEvent("UI_Dia_End");
      }

      mSpace->GetComponent<InputInterpreter>()->SetInputContext(InputInterpreter::InputContext::Sailing);
    }
  }
}
