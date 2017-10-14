#include "../../YTE/Core/Composition.hpp"
#include "../../YTE/Physics/Transform.hpp"

#include "Axis.hpp"
#include "Scale.hpp"

namespace YTE
{

  YTEDefineType(Scale)
  {
    YTERegisterType(Scale);
  }

  Scale::Scale(int aDir, Composition *aOwner, Space *aSpace, RSValue *aProperties) 
    : YTE::Component(aOwner, aSpace), mDir(aDir)
  {

  }

  void Scale::ScaleObject(glm::vec3 aDelta)
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

    glm::vec3 scale = transform->GetWorldScale();

    transform->SetWorldScale(scale + change);
  }

}