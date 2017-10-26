#include "YTE/Core/Composition.hpp"
#include "YTE/Physics/Transform.hpp"

#include "YTEditor/Gizmos/Axis.hpp"
#include "YTEditor/Gizmos/Translate.hpp"

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