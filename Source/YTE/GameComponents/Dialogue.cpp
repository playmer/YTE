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
  {
    YTEUnusedArgument(aProperties);
  }


  void Dialogue::Initialize()
  {
    mOwner->YTERegister(Events::RequestDialogueStart, this, &Dialogue::OnDialogueStart);
    mOwner->GetEngine()->YTERegister(Events::DialogueConfirm, this, &Dialogue::OnConfirm);
    mOwner->GetEngine()->YTERegister(Events::DialogueExit, this, &Dialogue::OnExit);

    mSprite = mOwner->AddComposition("DialogueSprite", "DialogueBoi");
    auto transform = mSprite->GetComponent<Transform>();
    transform->SetScale(glm::vec3());
  }

  void Dialogue::Update(LogicUpdate *aEvent)
  {

  }

  void Dialogue::OnDialogueStart(RequestDialogueStart *aEvent)
  {
    Composition *camera = aEvent->camera;
    
    auto orientation = camera->GetComponent<Orientation>();
    
    glm::vec3 forward = orientation->GetForwardVector();
    
    auto camTransform = camera->GetComponent<Transform>();
    
    glm::vec3 pos = camTransform->GetWorldTranslation();
    
    glm::vec3 spritePos = pos + 10.0f * -forward;

    auto spriteTransform = mSprite->GetComponent<Transform>();
    spriteTransform->SetWorldTranslation(spritePos);
    spriteTransform->SetScale(glm::vec3(10.0f, 10.0f, 10.0f));
    spriteTransform->SetWorldRotation(camTransform->GetWorldRotation());

    auto emitter = mOwner->GetComponent<WWiseEmitter>();

    if (emitter)
    {
      emitter->PlayEvent("UI_Dia_Start");
    }

    mOwner->GetEngine()->GetComponent<InputInterpreter>()->SetInputContext(InputInterpreter::InputContext::Dialogue);
  }

  void Dialogue::OnConfirm(DialogueConfirm *aEvent)
  {
    auto emitter = mOwner->GetComponent<WWiseEmitter>();

    if (emitter)
    {
      emitter->PlayEvent("UI_Dia_Next");
    }
  }

  void Dialogue::OnExit(DialogueExit *aEvent)
  {
    auto emitter = mOwner->GetComponent<WWiseEmitter>();

    if (emitter)
    {
      emitter->PlayEvent("UI_Dia_End");
    }

    mOwner->GetEngine()->GetComponent<InputInterpreter>()->SetInputContext(InputInterpreter::InputContext::Sailing);
  }

}