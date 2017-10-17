#include "../../YTE/Core/Composition.hpp"
#include "../../YTE/Physics/Transform.hpp"

#include "Axis.hpp"
#include "Rotate.hpp"

namespace YTE
{

  YTEDefineType(Rotate)
  {
    YTERegisterType(Rotate);
    YTEBindProperty(&Rotate::GetDirection, &Rotate::SetDirection, "Direction")
      .AddAttribute<Serializable>();
  }

  Rotate::Rotate(Composition *aOwner, Space *aSpace, RSValue *aProperties)
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

    YTE::Transform *transform = mOwner->GetComponent<YTE::Transform>();

    glm::vec3 rot = transform->GetWorldRotationAsEuler();

    transform->SetWorldRotation(rot + change);
  }

}