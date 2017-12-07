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
    if (!aObj)
    {
      return;
    }

    YTE::Transform *objTrans = aObj->GetComponent<YTE::Transform>();
    
    // get vector from object pos to mouse pos
    glm::vec3 toMPos = aFirstMousePos - objTrans->GetWorldTranslation();

    glm::vec3 objRot = glm::radians(objTrans->GetRotationAsEuler());

    YTE::Transform *gizTrans = mOwner->GetOwner()->GetComponent<YTE::Transform>();
    auto gizRot = gizTrans->GetRotationAsEuler();

    //glm::vec3 local;
    //
    //switch (mDir)
    //{
    //  case Axis::X:
    //  {
    //    // get the local x axis
    //    local = {1, 0, 0};
    //  }
    //
    //  case Axis::Y:
    //  {
    //    local = {0, 1, 0};
    //    break;
    //  }
    //
    //  case Axis::Z:
    //  {
    //    local = {0, 0, 1};
    //    break;
    //  }
    //}
    //
    //local = glm::rotateX(local, objRot.x);
    //local = glm::rotateY(local, objRot.y);
    //local = glm::rotateZ(local, objRot.z);
    //
    //// project vector onto the yz plane
    //glm::vec3 proj = toMPos - glm::proj(toMPos, local);
    //
    //proj = -proj;
    //
    //// rotate it 90 degrees so it's tangential to the point on the circle where the user clicked
    //glm::vec3 rotated = glm::rotate(proj, glm::radians(90.0f), local);
    //
    //// project the delta on to the rotated vector
    //glm::vec3 projected = glm::proj(aDelta, rotated);
    //
    //// apply the length of the projected vector
    //float projLength = glm::length(projected);
    //
    //if (glm::dot(aDelta, rotated) < 0)
    //{
    //  projLength = -projLength;
    //}
    //
    //float delta = projLength / glm::length(proj);


    float delta{0.0f};
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
    
        float rotChange = projLength / glm::length(proj);
    
        delta = -rotChange;
        break;
      }
    
      case Axis::Y:
      {
        // get the local y axis
        glm::vec3 localY(0, 1, 0);
    
        localY = glm::rotateX(localY, objRot.x);
        localY = glm::rotateY(localY, objRot.y);
        localY = glm::rotateZ(localY, objRot.z);
    
        // project vector onto the yz plane
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
    
        float rotChange = projLength / glm::length(proj);
    
        delta = -rotChange;
        break;
      }
    
      case Axis::Z:
      {
        // get the local z axis
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
    
        float rotChange = projLength / glm::length(proj);
    
        delta = -rotChange;
        break;
      }
    }



    glm::quat localQ;

    switch (mDir)
    {
      case Axis::X:
      {
        localQ = YTE::AroundAxis({1, 0, 0}, delta);
        break;
      }

      case Axis::Y:
      {
        localQ = YTE::AroundAxis({0, 1, 0}, delta);
        break;
      }

      case Axis::Z:
      {
        localQ = YTE::AroundAxis({0, 0, 1}, delta);
        break;
      }
    }


    //gizTrans->SetRotation(gizRot + change);
    //objTrans->SetRotation(objRot + change);

    //auto gizmoRotation = gizTrans->GetRotation();
    auto objectRotation = objTrans->GetRotation();

    //gizTrans->SetRotation();
    objTrans->SetRotation(objectRotation * localQ);
  }

}