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


#include "YTE/GameComponents/Dialogue.hpp"

namespace YTE
{
  YTEDefineType(Dialogue)
  {
    YTERegisterType(Dialogue);

    //std::vector<std::vector<Type*>> deps =
    //{
    //};
    //Dialogue::GetStaticType()->AddAttribute<ComponentDependencies>(deps);
  }


  Dialogue::Dialogue(Composition *aOwner, Space *aSpace, RSValue *aProperties)
    : Component(aOwner, aSpace)
    , mSprite(nullptr)
    , mActive(false)
  {
    YTEUnusedArgument(aProperties);
  }


  void Dialogue::Initialize()
  {
    mOwner->YTERegister(Events::RequestDialogueStart, this, &Dialogue::OnDialogueStart);
    mSpace->YTERegister(Events::DialogueConfirm, this, &Dialogue::OnConfirm);
    mSpace->YTERegister(Events::DialogueExit, this, &Dialogue::OnExit);

    mOwner->YTERegister(Events::CollisionPersisted, this, &Dialogue::OnCollisionPersist);
    mOwner->YTERegister(Events::CollisionStarted, this, &Dialogue::OnCollisionStart);
    mOwner->YTERegister(Events::CollisionEnded, this, &Dialogue::OnCollisionEnd);


    mSprite = mOwner->FindFirstCompositionByName("DialogueBoi");
    auto transform = mSprite->GetComponent<Transform>();
    
    transform->SetWorldScale(glm::vec3(1.0f, 1.0f, 1.0f));

  }

  void Dialogue::Update(LogicUpdate *aEvent)
  {
    YTEUnusedArgument(aEvent);
  }

  void Dialogue::OnCollisionPersist(CollisionPersisted * aEvent)
  {
    YTEUnusedArgument(aEvent);
  }

  void Dialogue::OnCollisionStart(CollisionStarted * aEvent)
  {
    YTEUnusedArgument(aEvent);
    mActive = true;
  }

  void Dialogue::OnCollisionEnd(CollisionEnded * aEvent)
  {
    YTEUnusedArgument(aEvent);
    mActive = false;
  }

  void Dialogue::OnDialogueStart(RequestDialogueStart *aEvent)
  {
    if (!mActive)
    {
      return;
    }

    Transform *dialogCamTrans = mOwner->FindFirstCompositionByName("DialogueCamera")->GetComponent<Transform>();

    Composition *camera = aEvent->camera;
    
    auto orientation = camera->GetComponent<Orientation>();
    
    glm::vec3 forward = orientation->GetForwardVector();
    
    auto camTransform = camera->GetComponent<Transform>();
   
    camTransform->SetWorldTranslation(dialogCamTrans->GetWorldTranslation());
    camTransform->SetWorldRotation(dialogCamTrans->GetWorldRotation());
    camTransform->SetWorldScale(dialogCamTrans->GetWorldScale());

    /*glm::vec3 pos = camTransform->GetWorldTranslation();
    
    glm::vec3 spritePos = pos - 3.0f * forward;

    auto spriteTransform = mSprite->GetComponent<Transform>();
    spriteTransform->SetWorldTranslation(spritePos);
    spriteTransform->SetWorldRotation(camTransform->GetWorldRotation());*/

    auto emitter = mOwner->GetComponent<WWiseEmitter>();

    if (emitter)
    {
      emitter->PlayEvent("UI_Dia_Start");
    }

    mSpace->GetComponent<InputInterpreter>()->SetInputContext(InputInterpreter::InputContext::Dialogue);
  }

  void Dialogue::OnConfirm(DialogueConfirm *aEvent)
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

  void Dialogue::OnExit(DialogueExit *aEvent)
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

    auto transform = mSprite->GetComponent<Transform>();
    transform->SetTranslation(glm::vec3(0.0f, -100.0f, 0.0f));
  }

}