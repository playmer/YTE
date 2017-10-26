#include "YTE/Core/Composition.hpp"
#include "YTE/Physics/Transform.hpp"

#include "YTEditor/Gizmos/Axis.hpp"
#include "YTEditor/Gizmos/Scale.hpp"

namespace YTEditor
{

  YTEDefineType(Scale)
  {
    YTERegisterType(Scale);
    YTEBindProperty(&Scale::GetDirection, &Scale::SetDirection, "Direction")
      .AddAttribute<YTE::Serializable>();
  }

  Scale::Scale(YTE::Composition *aOwner, YTE::Space *aSpace, YTE::RSValue *aProperties) 
    : YTE::Component(aOwner, aSpace), mDir(Axis::X)
  {
    DeserializeByType<Scale*>(aProperties, this, Scale::GetStaticType());
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