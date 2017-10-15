#include "../../YTE/Core/Composition.hpp"
#include "../../YTE/Physics/Transform.hpp"

#include "Axis.hpp"
#include "Translate.hpp"

namespace YTE
{

  YTEDefineType(Translate)
  {
    YTERegisterType(Translate);
  }

  Translate::Translate(Composition *aOwner, Space *aSpace, RSValue *aProperties) 
    : Component(aOwner, aSpace), mDir(Axis::X)
  {

  }

  void Translate::MoveObject(glm::vec3 aDelta)
  {
    if (aDelta.x > 1 || aDelta.y > 1 || aDelta.z > 1)
    {
      return;
    }

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

    YTE::Transform *transform = mOwner->GetOwner()->GetComponent<YTE::Transform>();

    glm::vec3 pos = transform->GetWorldTranslation();

    transform->SetWorldTranslation(pos + change);
  }

}