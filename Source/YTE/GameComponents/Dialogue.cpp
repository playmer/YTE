#include "YTE/Graphics/Camera.hpp"

#include "YTE/Physics/Orientation.hpp"

#include "YTE/Physics/Transform.hpp"


#include "YTE/GameComponents/Dialogue.hpp"

namespace YTE
{
  YTEDefineType(Dialogue)
  {
    RegisterType<Dialogue>();

    //std::vector<std::vector<Type*>> deps =
    //{
    //};
    //GetStaticType()->AddAttribute<ComponentDependencies>(deps);
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
    //mOwner->RegisterEvent<&Dialogue::OnDialogueStart>(Events::RequestDialogueStart, this);
    //mSpace->RegisterEvent<&Dialogue::OnConfirm>(Events::DialogueConfirm, this);
    //mSpace->RegisterEvent<&Dialogue::OnExit>(Events::DialogueExit, this);

    //mOwner->RegisterEvent<&Dialogue::OnCollisionPersist>(Events::CollisionPersisted, this);
    //mOwner->RegisterEvent<&Dialogue::OnCollisionStart>(Events::CollisionStarted, this);
    //mOwner->RegisterEvent<&Dialogue::OnCollisionEnd>(Events::CollisionEnded, this);

    auto soundSystem = mSpace->GetEngine()->GetComponent<WWiseSystem>();

    if (soundSystem)
    {
      mDiaStart = soundSystem->GetSoundIDFromString("UI_Dia_Start");
      mDiaNext = soundSystem->GetSoundIDFromString("UI_Dia_Next");
      mDiaEnd = soundSystem->GetSoundIDFromString("UI_Dia_End");
    }


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
    YTEUnusedArgument(aEvent);

    if (!mActive)
    {
      return;
    }

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

    auto emitter = mOwner->GetComponent<WWiseEmitter>();

    if (emitter)
    {
      emitter->PlayEvent(mDiaStart);
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
      emitter->PlayEvent(mDiaNext);
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
      emitter->PlayEvent(mDiaEnd);
    }

    mSpace->GetComponent<InputInterpreter>()->SetInputContext(InputInterpreter::InputContext::Sailing);

    auto transform = mSprite->GetComponent<Transform>();
    transform->SetTranslation(glm::vec3(0.0f, -100.0f, 0.0f));
  }

}