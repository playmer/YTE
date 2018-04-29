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

#include "YTE/Physics/Orientation.hpp"
#include "YTE/Physics/Transform.hpp"

namespace YTE
{
  YTEDefineEvent(OrientationChanged);

  YTEDefineType(OrientationChanged)
  {
    RegisterType<OrientationChanged>();
    YTEBindField(&OrientationChanged::Orientation, "Orientation", PropertyBinding::Get);
    YTEBindField(&OrientationChanged::ForwardVector, "ForwardVector", PropertyBinding::Get);
    YTEBindField(&OrientationChanged::RightVector, "RightVector", PropertyBinding::Get);
    YTEBindField(&OrientationChanged::UpVector, "UpVector", PropertyBinding::Get);
  }

  YTEDefineType(Orientation)
  {
    RegisterType<Orientation>();
    GetStaticType()->AddAttribute<RunInEditor>();

    std::vector<std::vector<Type*>> deps = { { TypeId<Transform>() } };

    GetStaticType()->AddAttribute<ComponentDependencies>(deps);

    YTEBindProperty(&Orientation::GetForwardVector, nullptr, "ForwardVector");
    YTEBindProperty(&Orientation::GetRightVector, nullptr, "RightVector");
    YTEBindProperty(&Orientation::GetUpVector, nullptr, "UpVector");
  }

  Orientation::Orientation(Composition *aOwner, Space *aSpace, RSValue *aProperties)
    : Component(aOwner, aSpace)
  {
    DeserializeByType(aProperties, this, GetStaticType());
  };

  void Orientation::Initialize()
  {
    mOwner->YTERegister(Events::RotationChanged, this, &Orientation::OnRotationChanged);

    const glm::vec3 forwardReset(0, 0, 1);
    const glm::vec3 rightReset(-1, 0, 0);
    const glm::vec3 upReset(0, 1, 0);

    mTransform = mOwner->GetComponent<Transform>();

    glm::quat rotation = mTransform->GetWorldRotation();

    mForwardVector = glm::normalize(rotation * forwardReset);
    mRightVector   = glm::normalize(rotation * rightReset);
    mUpVector      = glm::normalize(rotation * upReset);
  }

  void Orientation::OnRotationChanged(TransformChanged *aEvent)
  {
    //std::cout << "Orientation Rotation Change\n";

    const glm::vec3 forwardReset{ 0, 0, 1 };
    const glm::vec3 rightReset{-1, 0, 0};
    const glm::vec3 upReset{0, 1, 0};

    glm::quat rotation = aEvent->WorldRotation;

    mForwardVector = glm::normalize(rotation * forwardReset);
    mRightVector   = glm::normalize(rotation * rightReset);
    mUpVector      = glm::normalize(rotation * upReset);

    OrientationChanged newOrientation;
    newOrientation.Orientation = this;
    newOrientation.ForwardVector = mForwardVector;
    newOrientation.RightVector = mRightVector;
    newOrientation.UpVector = mUpVector;
    mOwner->SendEvent(Events::OrientationChanged, &newOrientation);
  }

  glm::vec3 Orientation::GetForwardVector() const
  {
    return mForwardVector;
  }

  glm::vec3 Orientation::GetRightVector() const
  {
    return mRightVector;
  }

  glm::vec3 Orientation::GetUpVector() const
  {
    return mUpVector;
  }

  void Orientation::LookAt(glm::vec3 const &aDirection)
  {
    //YTEUnusedArgument(aUp);
    //
    /////Derived from pseudo code found here:
    /////http://stackoverflow.com/questions/13014973/quaternion-rotate-to
    ///// and actual code from :
    /////http://stackoverflow.com/questions/14337441/looking-at-an-object-with-a-quaternion
    
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


    //Alternative
    //auto direction = glm::normalize(aDirection);
    //
    //auto axis = glm::cross(aDirection, mForwardVector);
    //auto angle = glm::dot(aDirection, mForwardVector);
    //
    //mTransform->Rotate(axis, angle);
  }

  void Orientation::LookAtPoint(glm::vec3 const &aPoint)
  {
    LookAt(aPoint - mTransform->GetWorldTranslation());
  }
}