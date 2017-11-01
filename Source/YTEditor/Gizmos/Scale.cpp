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

  void Scale::ScaleObject(YTE::Composition *aObj, glm::vec3 aDelta)
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

    // get the transform of the currently selected object
    YTE::Transform *transform = aObj->GetComponent<YTE::Transform>();

    glm::vec3 size = transform->GetScale();
    glm::vec3 newSize = size + change;

    //std::cout << "------------------------------------------------------------------------------------\n";
    //std::cout << "Size:     " << size.x << ", " << size.y << ", " << size.z << std::endl;
    //std::cout << "Change:   " << change.x << ", " << change.y << ", " << change.z << std::endl;
    //std::cout << "New Size: " << newSize.x << ", " << newSize.y << ", " << newSize.z << std::endl;


    transform->SetScale(newSize);
  }

}