/******************************************************************************/
/*!
 * \author Isaac Dayton
 * \date   2015-10-28
 *
 * \copyright All content 2016 DigiPen (USA) Corporation, all rights reserved.
*/
/******************************************************************************/
#include "YTE/Core/Composition.hpp"
#include "YTE/Core/Engine.hpp"
#include "YTE/Core/Space.hpp"

#include "YTE/Physics/Orientation.h"
#include "YTE/Physics/Transform.h"
#include "YTE/Physics/Orientation.h"

namespace YTE
{
  DefineType(Orientation)
  {
    YTERegisterType(Orientation);
    YTEBindProperty(&Orientation::GetForwardVector, nullptr, "ForwardVector");
    YTEBindProperty(&Orientation::GetRightVector, nullptr, "RightVector");
    YTEBindProperty(&Orientation::GetUpVector, nullptr, "UpVector");

    YTEAddFunction(&Orientation::LookAtDirection, YTENoOverload, "LookAtDirection", YTEParameterNames("aDirection"));
    YTEAddFunction(&Orientation::LookAtDirectionWithUp, YTENoOverload, "LookAtDirectionWithUp", YTEParameterNames("aDirection", "aUp"));
    YTEAddFunction(&Orientation::LookAtPoint, YTENoOverload, "LookAtPoint", YTEParameterNames("aPoint"));
    YTEAddFunction(&Orientation::LookAtPointWithUp, YTENoOverload, "LookAtPointWithUp", YTEParameterNames("aPoint", "aUp"));
  }

  Orientation::Orientation(Composition *aOwner, Space *aSpace, RSValue *aProperties)
    : Component(aOwner, aSpace)
  {
    DeserializeByType<Orientation*>(aProperties, this, Orientation::GetStaticType());
      
    mForwardVector = { 0.0f, 0.0f, 1.0f };
    mRightVector = { 1.0f, 0.0f, 0.0f };
    mUpVector = { 0.0f, 1.0f, 0.0f};
  };

  void Orientation::Initialize()
  {
    mOwner->CONNECT(Events::RotationChanged, this, &Orientation::OnRotationChanged);
  }

  void Orientation::OnRotationChanged(RotationChanged *aEvent)
  {
    const glm::vec3 forwardReset(0, 0, 1);
    const glm::vec3 rightReset(1, 0, 0);
    const glm::vec3 upReset(0, 1, 0);

    glm::quat rotation = aEvent->Rotation;
    
    mForwardVector = glm::rotate(rotation, forwardReset);
    mRightVector = glm::rotate(rotation, rightReset);
    mUpVector = glm::rotate(rotation, upReset);

    OrientationChanged newOrientation;
    newOrientation.Forward = mForwardVector;
    newOrientation.Right = mRightVector;
    newOrientation.Up = mUpVector;
    mOwner->SendEvent(Events::OrientationChanged, &newOrientation);
  }

  const glm::vec3& Orientation::GetForwardVector() const
  {
    return mForwardVector;
  }

  const glm::vec3& Orientation::GetRightVector() const
  {
    return mRightVector;
  }

  const glm::vec3& Orientation::GetUpVector() const
  {
    return mUpVector;
  }

  void Orientation::LookAtPoint(glm::vec3 &aPoint)
  {
    auto direction = aPoint - mOwner->GetComponent<Transform>()->GetTranslation();
    LookAtDirectionWithUp(direction, mUpVector);
  }

  void Orientation::LookAtPointWithUp(glm::vec3 &aPoint, glm::vec3 &aUp)
  {
    auto direction = aPoint - mOwner->GetComponent<Transform>()->GetTranslation();
    LookAtDirectionWithUp(direction, aUp);
  }

  void Orientation::LookAtDirection(glm::vec3 &aDirection)
  {
    LookAtDirectionWithUp(aDirection, mUpVector);
  }

  void Orientation::LookAtDirectionWithUp(glm::vec3 &aDirection, glm::vec3 &aUp)
  {
    ///Derived from pseudocode found here:
    ///http://stackoverflow.com/questions/13014973/quaternion-rotate-to
    /// and actual code from :
    ///http://stackoverflow.com/questions/14337441/looking-at-an-object-with-a-quaternion
      
    auto transform = mOwner->GetComponent<Transform>();
    auto position = transform->GetTranslation();
      
    //Normalize VectorTo
    auto direction = glm::normalize(aDirection);
      
    //Get the dot product to find the angle
    float forwardAngle = std::atan2(mForwardVector.z, mForwardVector.x);
    float directionAngle = std::atan2(direction.z, direction.x);
      
    // create [0, 2pi] ranges instead of [-pi, pi]
    if (forwardAngle < 0.0f)
    {
      forwardAngle = 2.0f * glm::pi<float>() + forwardAngle;
    }
    if (directionAngle < 0.0f)
    {
      directionAngle = 2.0f * glm::pi<float>() + directionAngle;
    }

    glm::vec3 eulers = transform->GetRotationAsEuler();
    eulers.x = glm::radians(eulers.x);
    eulers.y = glm::radians(eulers.y);
    eulers.z = glm::radians(eulers.z);
      
    float yRotation = directionAngle - forwardAngle;
    eulers.y += yRotation;
      
    //Finally, create a quaternion
    glm::quat axisAngle(eulers);
      
    //And multiply it into the current orientation
    transform->SetRotation(axisAngle);
  }
}
