#include <glm/gtx/rotate_vector.hpp>
#include <glm/gtx/projection.hpp>

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
    glm::vec3 change = glm::vec3(0.0f, 0.0f, 0.0f);

    // get the transform of the currently selected object
    YTE::Transform *transform = aObj->GetComponent<YTE::Transform>();
    glm::vec3 rot = transform->GetWorldRotationAsEuler();

    switch (mDir)
    {
    case Axis::X:
    {
      // get the local x axis
      glm::vec3 localX(1, 0, 0);
      
      localX = glm::rotateX(localX, rot.x);
      localX = glm::rotateY(localX, rot.y);
      localX = glm::rotateZ(localX, rot.z);
      
      // project delta vector onto the object local x axis
      glm::vec3 projected = glm::proj(aDelta, localX);
      float diff = glm::length(projected);

      if (glm::dot(aDelta, localX) < 0)
      {
        diff = -diff;
      }


      change.x = diff;
      break;
    }

    case Axis::Y:
    {
      // get the local Y axis
      glm::vec3 localY(0, 1, 0);

      localY = glm::rotateX(localY, rot.x);
      localY = glm::rotateY(localY, rot.y);
      localY = glm::rotateZ(localY, rot.z);

      // project delta vector onto the object local x axis
      glm::vec3 projected = glm::proj(aDelta, localY);
      float diff = glm::length(projected);

      if (glm::dot(aDelta, localY) < 0)
      {
        diff = -diff;
      }


      change.y = diff;
      break;
    }

    case Axis::Z:
    {
      // get the local Z axis
      glm::vec3 localZ(0, 0, 1);

      localZ = glm::rotateX(localZ, rot.x);
      localZ = glm::rotateY(localZ, rot.y);
      localZ = glm::rotateZ(localZ, rot.z);

      // project delta vector onto the object local x axis
      glm::vec3 projected = glm::proj(aDelta, localZ);
      float diff = glm::length(projected);

      if (glm::dot(aDelta, localZ) < 0)
      {
        diff = -diff;
      }

      change.z = diff;
      break;
    }
    }


    glm::vec3 size = transform->GetScale();
    glm::vec3 newSize = size + change;

    //std::cout << "------------------------------------------------------------------------------------\n";
    //std::cout << "Size:     " << size.x << ", " << size.y << ", " << size.z << std::endl;
    //std::cout << "Change:   " << change.x << ", " << change.y << ", " << change.z << std::endl;
    //std::cout << "New Size: " << newSize.x << ", " << newSize.y << ", " << newSize.z << std::endl;

    transform->SetScale(newSize);
  }

}