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
    YTEBindField(&OrientationChanged::Forward, "Forward", PropertyBinding::Get);
    YTEBindField(&OrientationChanged::Right, "Right", PropertyBinding::Get);
    YTEBindField(&OrientationChanged::Up, "Up", PropertyBinding::Get);
  }

  YTEDefineType(Orientation)
  {
    YTERegisterType(Orientation);

    std::vector<std::vector<Type*>> deps = { { Transform::GetStaticType() } };

    GetStaticType()->AddAttribute<ComponentDependencies>(deps);

    YTEBindProperty(&Orientation::GetForwardVector, nullptr, "ForwardVector");
    YTEBindProperty(&Orientation::GetRightVector, nullptr, "RightVector");
    YTEBindProperty(&Orientation::GetUpVector, nullptr, "UpVector");
    YTEBindProperty(&Orientation::GetForward, nullptr, "Forward");
    YTEBindProperty(&Orientation::GetRight, nullptr, "Right");
    YTEBindProperty(&Orientation::GetUp, nullptr, "Up");
  }

  Orientation::Orientation(Composition *aOwner, Space *aSpace, RSValue *aProperties)
    : Component(aOwner, aSpace)
  {
    DeserializeByType<Orientation*>(aProperties, this, Orientation::GetStaticType());

    const glm::vec3 forwardReset(0, 0, -1);
    const glm::vec3 rightReset(1, 0, 0);
    const glm::vec3 upReset(0, 1, 0);

    mForward = AroundAxis(forwardReset, 0.0f);
    mRight = AroundAxis(rightReset, 0.0f);
    mUp = AroundAxis(upReset, 0.0f);
  };

  void Orientation::Initialize()
  {
    mOwner->YTERegister(Events::RotationChanged, this, &Orientation::OnRotationChanged);
  }

  void Orientation::OnRotationChanged(TransformChanged *aEvent)
  {
    std::cout << "Orientation Rotation Change\n";

    const glm::vec3 forwardReset(0, 0, -1);
    const glm::vec3 rightReset(1, 0, 0);
    const glm::vec3 upReset(0, 1, 0);


    auto forwardVector = AroundAxis(forwardReset, 0.0f);
    auto rightVector = AroundAxis(rightReset, 0.0f);
    auto upVector = AroundAxis(upReset, 0.0f);

    glm::quat rotation = aEvent->WorldRotation;

    mForwardVector = glm::rotate(rotation, forwardReset);
    mRightVector = glm::rotate(rotation, rightReset);
    mUpVector = glm::rotate(rotation, upReset);

    mForward = rotation * forwardVector;
    mRight = rotation * rightVector;
    mUp = rotation * upVector;

    OrientationChanged newOrientation;
    newOrientation.Orientation = this;
    newOrientation.ForwardVector = GetForwardVector();
    newOrientation.RightVector = GetRightVector();
    newOrientation.UpVector = GetUpVector();
    newOrientation.Forward = mForward;
    newOrientation.Right = mRight;
    newOrientation.Up = mUp;
    mOwner->SendEvent(Events::OrientationChanged, &newOrientation);
  }

  glm::quat Orientation::GetForward() const
  {
    return mForward;
  }

  glm::quat Orientation::GetRight() const
  {
    return mRight;
  }

  glm::quat Orientation::GetUp() const
  {
    return mUp;
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