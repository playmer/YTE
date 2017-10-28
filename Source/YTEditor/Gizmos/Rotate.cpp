#include "YTE/Core/Composition.hpp"
#include "YTE/Physics/Transform.hpp"

#include "YTEditor/Gizmos/Axis.hpp"
#include "YTEditor/Gizmos/Rotate.hpp"

namespace YTEditor
{

  YTEDefineType(Rotate)
  {
    YTERegisterType(Rotate);
    YTEBindProperty(&Rotate::GetDirection, &Rotate::SetDirection, "Direction")
      .AddAttribute<YTE::Serializable>();
  }

  Rotate::Rotate(YTE::Composition *aOwner, YTE::Space *aSpace, YTE::RSValue *aProperties)
    : YTE::Component(aOwner, aSpace), mDir(Axis::X)
  {
    DeserializeByType<Rotate*>(aProperties, this, Rotate::GetStaticType());
  }

  void Rotate::RotateObject(glm::vec3 aDelta)
  {
    glm::vec3 change = glm::vec3(0.0f, 0.0f, 0.0f);

    switch (mDir)
    {
    case Axis::X:
    {
      change.x = aDelta.y;
      break;
    }

    case Axis::Y:
    {
      change.y = aDelta.x;
      break;
    }

    case Axis::Z:
    {
      change.z = aDelta.x;
      break;
    }
    }

    YTE::Transform *transform = mOwner->GetComponent<YTE::Transform>();

    glm::vec3 rot = transform->GetWorldRotationAsEuler();

    transform->SetWorldRotation(rot + change);
  }

}