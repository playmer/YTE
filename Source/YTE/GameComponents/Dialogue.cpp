#include "YTE/Physics/Orientation.hpp"

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
  {
    YTEUnusedArgument(aProperties);
  }


  void Dialogue::Initialize()
  {
    mOwner->YTERegister(Events::DialogueStart, this, &Dialogue::OnStart);
    mOwner->GetEngine()->YTERegister(Events::DialogueConfirm, this, &Dialogue::OnConfirm);
    mOwner->GetEngine()->YTERegister(Events::DialogueExit, this, &Dialogue::OnExit);
  }

  void Dialogue::Update(LogicUpdate *aEvent)
  {

  }

  void Dialogue::OnStart(DialogueStart *aEvent)
  {
    Composition *camera = aEvent->cameraObj;

    auto orientation = camera->GetComponent<Orientation>();

    glm::vec3 forward = orientation->GetForwardVector();

    auto transform = camera->GetComponent<Transform>();

    glm::vec3 pos = transform->GetWorldTranslation();

    glm::vec3 spritePos = pos + 5.0f * forward;

    



  }

  void Dialogue::OnConfirm(DialogueConfirm *aEvent)
  {

  }

  void Dialogue::OnExit(DialogueExit *aEvent)
  {

  }

}