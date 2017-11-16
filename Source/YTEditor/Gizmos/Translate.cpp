#include "YTE/Core/Composition.hpp"
#include "YTE/Physics/Transform.hpp"

#include "YTEditor/Gizmos/Axis.hpp"
#include "YTEditor/Gizmos/Translate.hpp"
#include "YTEditor/MainWindow/MainWindow.hpp"

namespace YTEditor
{

  YTEDefineType(Translate)
  {
    YTERegisterType(Translate);
    YTEBindProperty(&Translate::GetDirection, &Translate::SetDirection, "Direction")
      .AddAttribute<YTE::Serializable>();
  }

  Translate::Translate(YTE::Composition *aOwner, YTE::Space *aSpace, YTE::RSValue *aProperties) 
    : Component(aOwner, aSpace)
    , mDir(Axis::X)
  {
    DeserializeByType<Translate*>(aProperties, this, Translate::GetStaticType());
  }

  void Translate::MoveObject(YTE::Composition *aObj, glm::vec3 aDelta)
  {
    glm::vec3 change = glm::vec3(0.0f, 0.0f, 0.0f);

    switch (mDir)
    {
    case Axis::X:
    {
      change.x = aDelta.x;
      break;
    }

    case Axis::Y:
    {
      change.y = aDelta.y;
      break;
    }

    case Axis::Z:
    {
      change.z = aDelta.z;
      break;
    }
    }

    // get the transform of the currently selected object
    YTE::Transform *transform = aObj->GetComponent<YTE::Transform>();

    glm::vec3 pos = transform->GetWorldTranslation();
    glm::vec3 newPos = pos + change;

    transform->SetWorldTranslation(newPos);

    YTE::Transform *gizmoTransform = mOwner->GetOwner()->GetComponent<YTE::Transform>();
    gizmoTransform->SetWorldTranslation(newPos);
  }
}