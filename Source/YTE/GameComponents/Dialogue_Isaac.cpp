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


#include "YTE/GameComponents/Dialogue_Isaac.hpp"

namespace YTE
{
  YTEDefineEvent(DialogueStart);
  YTEDefineType(DialogueStart) { YTERegisterType(DialogueStart); }

  YTEDefineType(DialogueIsaac)
  {
    YTERegisterType(DialogueIsaac);

    //std::vector<std::vector<Type*>> deps =
    //{
    //};
    //Dialogue::GetStaticType()->AddAttribute<ComponentDependencies>(deps);
  }


  DialogueIsaac::DialogueIsaac(Composition *aOwner, Space *aSpace, RSValue *aProperties)
    : Component(aOwner, aSpace)
    //, mSprite(nullptr)
    , mActive(false)
  {
    YTEUnusedArgument(aProperties);
  }


  void DialogueIsaac::Initialize()
  {
    mDockAnchorPosition = mOwner->GetComponent<Transform>()->GetWorldTranslation();

    mSpace->YTERegister(Events::RequestDialogueStart, this, &DialogueIsaac::OnRequestDialogueStart);
    mSpace->YTERegister(Events::DialogueConfirm, this, &DialogueIsaac::OnDialogueConfirm);
    mSpace->YTERegister(Events::DialogueExit, this, &DialogueIsaac::OnDialogueExit);

    mOwner->YTERegister(Events::CollisionPersisted, this, &DialogueIsaac::OnCollisionPersist);
    mOwner->YTERegister(Events::CollisionStarted, this, &DialogueIsaac::OnCollisionStart);
    mOwner->YTERegister(Events::CollisionEnded, this, &DialogueIsaac::OnCollisionEnd);


    //mSprite = mOwner->FindFirstCompositionByName("DialogueBoi");
    //auto transform = mSprite->GetComponent<Transform>();
    
    //transform->SetWorldScale(glm::vec3(1.0f, 1.0f, 1.0f));
  }

  void DialogueIsaac::Update(LogicUpdate *aEvent)
  {
    YTEUnusedArgument(aEvent);
  }

  void DialogueIsaac::OnCollisionPersist(CollisionPersisted * aEvent)
  {
    YTEUnusedArgument(aEvent);
  }

  void DialogueIsaac::OnCollisionStart(CollisionStarted * aEvent)
  {
    YTEUnusedArgument(aEvent);
    mActive = true;
  }

  void DialogueIsaac::OnCollisionEnd(CollisionEnded * aEvent)
  {
    YTEUnusedArgument(aEvent);
    mActive = false;
  }

  void DialogueIsaac::OnRequestDialogueStart(RequestDialogueStart *aEvent)
  {
    if (mActive && !aEvent->EventHandled)
    {
      aEvent->EventHandled = true;

      BoatDockEvent dockEvent(mDockAnchorPosition);
      mSpace->SendEvent(Events::BoatDockEvent, &dockEvent);

      /*Transform *dialogCamTrans = mOwner->FindFirstCompositionByName("DialogueCamera")->GetComponent<Transform>();

      Composition *camera = aEvent->camera;

      auto orientation = camera->GetComponent<Orientation>();

      glm::vec3 forward = orientation->GetForwardVector();

      auto camTransform = camera->GetComponent<Transform>();

      camTransform->SetWorldTranslation(dialogCamTrans->GetWorldTranslation());
      camTransform->SetWorldRotation(dialogCamTrans->GetWorldRotation());
      camTransform->SetWorldScale(dialogCamTrans->GetWorldScale());*/

      /*glm::vec3 pos = camTransform->GetWorldTranslation();

      glm::vec3 spritePos = pos - 3.0f * forward;

      auto spriteTransform = mSprite->GetComponent<Transform>();
      spriteTransform->SetWorldTranslation(spritePos);
      spriteTransform->SetWorldRotation(camTransform->GetWorldRotation());*/

      /*auto emitter = mOwner->GetComponent<WWiseEmitter>();

      if (emitter)
      {
        emitter->PlayEvent("UI_Dia_Start");
      }*/

      mSpace->GetComponent<InputInterpreter>()->SetInputContext(InputInterpreter::InputContext::Dialogue);
    }
  }

  void DialogueIsaac::OnDialogueConfirm(DialogueConfirm *aEvent)
  {
    YTEUnusedArgument(aEvent);
    if (!mActive)
    {
      return;
    }

    auto emitter = mOwner->GetComponent<WWiseEmitter>();

    if (emitter)
    {
      emitter->PlayEvent("UI_Dia_Next");
    }
  }

  void DialogueIsaac::OnDialogueExit(DialogueExit *aEvent)
  {
    YTEUnusedArgument(aEvent);
    if (!mActive)
    {
      return;
    }

    auto emitter = mOwner->GetComponent<WWiseEmitter>();

    if (emitter)
    {
      emitter->PlayEvent("UI_Dia_End");
    }

    mSpace->GetComponent<InputInterpreter>()->SetInputContext(InputInterpreter::InputContext::Sailing);

    //auto transform = mSprite->GetComponent<Transform>();
    //transform->SetTranslation(glm::vec3(0.0f, -100.0f, 0.0f));
  }
}
