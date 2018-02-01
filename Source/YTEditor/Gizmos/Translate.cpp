#include "YTE/Core/Composition.hpp"
#include "YTE/Physics/Transform.hpp"

#include "YTEditor/Gizmos/Gizmo.hpp"
#include "YTEditor/Gizmos/Translate.hpp"
#include "YTEditor/MainWindow/MainWindow.hpp"

namespace YTEditor
{

  YTEDefineType(Translate)
  {
    YTERegisterType(Translate);
    YTEBindField(&Translate::mDir, "Direction", YTE::PropertyBinding::GetSet)
      .AddAttribute<YTE::Serializable>();
  }

  Translate::Translate(YTE::Composition *aOwner, YTE::Space *aSpace, YTE::RSValue *aProperties) 
    : Component(aOwner, aSpace)
    , mDir(Gizmo::Dir::X)
  {
    DeserializeByType(aProperties, this, GetStaticType());
  }

  void Translate::MoveObject(YTE::Composition *aObj, glm::vec3 aDelta)
  {
    if (!aObj)
    {
      return;
    }

    glm::vec3 change = glm::vec3(0.0f, 0.0f, 0.0f);

    switch (mDir)
    {
    case Gizmo::Dir::X:
    {
      change.x = aDelta.x;
      break;
    }

    case Gizmo::Dir::Y:
    {
      change.y = aDelta.y;
      break;
    }

    case Gizmo::Dir::Z:
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