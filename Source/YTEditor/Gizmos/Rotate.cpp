#include <glm/gtx/rotate_vector.hpp>
#include <glm/gtx/projection.hpp>

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

  void Rotate::RotateObject(YTE::Composition *aObj, glm::vec3 aFirstMousePos, glm::vec3 aDelta)
  {
    YTE::Transform *objTrans = aObj->GetComponent<YTE::Transform>();
    
    glm::vec3 change = glm::vec3();

    // get vector from object pos to mouse pos
    glm::vec3 toMPos = aFirstMousePos - objTrans->GetWorldTranslation();

    glm::vec3 objRot = objTrans->GetWorldRotationAsEuler();

    switch (mDir)
    {
    case Axis::X:
    {
      // get the local x axis
      glm::vec3 localX(1, 0, 0);

      localX = glm::rotateX(localX, objRot.x);
      localX = glm::rotateY(localX, objRot.y);
      localX = glm::rotateZ(localX, objRot.z);

      // project vector onto the yz plane
      glm::vec3 proj = toMPos - glm::proj(toMPos, localX);

      proj = -proj;

      // rotate it 90 degrees so it's tangential to the point on the circle where the user clicked
      glm::vec3 rotated = glm::rotate(proj, glm::radians(90.0f), localX);

      // project the delta on to the rotated vector
      glm::vec3 projected = glm::proj(aDelta, rotated);

      // apply the length of the projected vector
      float projLength = glm::length(projected);

      if (glm::dot(aDelta, rotated) < 0)
      {
        projLength = -projLength;
      }

      change.x = projLength;
      break;
    }

    case Axis::Y:
    {
      // get the local Y axis
      glm::vec3 localY(0, 1, 0);

      localY = glm::rotateX(localY, objRot.x);
      localY = glm::rotateY(localY, objRot.y);
      localY = glm::rotateZ(localY, objRot.z);

      //// project vector onto the yz plane
      glm::vec3 proj = toMPos - glm::proj(toMPos, localY);

      proj = -proj;

      // rotate it 90 degrees so it's tangential to the point on the circle where the user clicked
      glm::vec3 rotated = glm::rotate(proj, glm::radians(90.0f), localY);

      // project the delta on to the rotated vector
      glm::vec3 projected = glm::proj(aDelta, rotated);

      // apply the length of the projected vector
      float projLength = glm::length(projected);

      if (glm::dot(aDelta, rotated) < 0)
      {
        projLength = -projLength;
      }

      change.y = projLength;
      break;
    }

    case Axis::Z:
    {
      // get the local Z axis
      glm::vec3 localZ(0, 0, 1);

      localZ = glm::rotateX(localZ, objRot.x);
      localZ = glm::rotateY(localZ, objRot.y);
      localZ = glm::rotateZ(localZ, objRot.z);

      // project vector onto the yz plane
      glm::vec3 proj = toMPos - glm::proj(toMPos, localZ);

      proj = -proj;

      // rotate it 90 degrees so it's tangential to the point on the circle where the user clicked
      glm::vec3 rotated = glm::rotate(proj, glm::radians(90.0f), localZ);

      // project the delta on to the rotated vector
      glm::vec3 projected = glm::proj(aDelta, rotated);

      // apply the length of the projected vector
      float projLength = glm::length(projected);

      if (glm::dot(aDelta, rotated) < 0)
      {
        projLength = -projLength;
      }

      change.z = projLength;
      break;
    }
    }

    YTE::Transform *gizTrans = mOwner->GetOwner()->GetComponent<YTE::Transform>();
    glm::vec3 gizRot = gizTrans->GetWorldRotationAsEuler();
    gizTrans->SetWorldRotation(gizRot + change);

    objTrans->SetWorldRotation(objRot + change);
  }

}