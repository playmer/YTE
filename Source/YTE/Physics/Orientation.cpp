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
    YTERegisterType(OrientationChanged);
    YTEBindField(&OrientationChanged::Orientation, "Orientation", PropertyBinding::Get);
    YTEBindField(&OrientationChanged::ForwardVector, "ForwardVector", PropertyBinding::Get);
    YTEBindField(&OrientationChanged::RightVector, "RightVector", PropertyBinding::Get);
    YTEBindField(&OrientationChanged::UpVector, "UpVector", PropertyBinding::Get);
  }

  YTEDefineType(Orientation)
  {
    YTERegisterType(Orientation);
    GetStaticType()->AddAttribute<RunInEditor>();

    std::vector<std::vector<Type*>> deps = { { Transform::GetStaticType() } };

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

    auto transform = mOwner->GetComponent<Transform>();

    glm::quat rotation = transform->GetWorldRotation();

    mForwardVector = rotation * forwardReset;
    mRightVector = rotation * rightReset;
    mUpVector = rotation * upReset;
  }

  void Orientation::OnRotationChanged(TransformChanged *aEvent)
  {
    //std::cout << "Orientation Rotation Change\n";

    const glm::vec3 forwardReset{ 0, 0, 1 };
    const glm::vec3 rightReset{-1, 0, 0};
    const glm::vec3 upReset{0, 1, 0};

    glm::quat rotation = aEvent->WorldRotation;

    mForwardVector = rotation * forwardReset;
    mRightVector = rotation * rightReset;
    mUpVector = rotation * upReset;

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
}