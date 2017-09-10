/******************************************************************************/
/*!
* \author Isaac Dayton
* \date   2015-10-16
*
* \copyright All content 2016 DigiPen (USA) Corporation, all rights reserved.
*/
/******************************************************************************/

#include "YTE/Core/Engine.hpp"
#include "YTE/Core/Space.hpp"

#include "YTE/Physics/RigidBody.h"
#include "YTE/Physics/Transform.h"

namespace YTE
{
  DefineEvent(PositionChanged);
  DefineEvent(RotationChanged);
  DefineEvent(ScaleChanged);
  DefineEvent(OrientationChanged);

  DefineType(PositionChanged)
  {
    YTERegisterType(PositionChanged);
    YTEBindField(&PositionChanged::Position, "Position", PropertyBinding::Get);
  }

  DefineType(RotationChanged)
  {
    YTERegisterType(RotationChanged);
    YTEBindField(&RotationChanged::Rotation, "Rotation", PropertyBinding::Get);
  }

  DefineType(ScaleChanged)
  {
    YTERegisterType(ScaleChanged);
    YTEBindField(&ScaleChanged::Scale, "Scale", PropertyBinding::Get);
  }

  DefineType(OrientationChanged)
  {
    YTERegisterType(OrientationChanged);
    YTEBindField(&OrientationChanged::Forward, "Forward", PropertyBinding::Get);
    YTEBindField(&OrientationChanged::Right, "Right", PropertyBinding::Get);
    YTEBindField(&OrientationChanged::Up, "Up", PropertyBinding::Get);
  }

  DefineType(Transform)
  {
    YTERegisterType(Transform);
    YTEBindProperty(&Transform::GetTranslation, &Transform::SetTranslationProperty, "Translation")->AddAttribute<EditorProperty>();
    YTEBindProperty(&Transform::GetScale, &Transform::SetScaleProperty, "Scale")->AddAttribute<EditorProperty>();
    YTEBindProperty(&Transform::GetRotationAsEuler, &Transform::SetRotationProperty, "Rotation")->AddAttribute<EditorProperty>();

    YTEAddFunction( &Transform::SetRotation, (void (Transform::*) (const glm::vec3&)), "SetRotation", YTEParameterNames("eulerAngles"))
      ->Description() = "Sets the local rotation relative to parent from a Real3 of Euler Angles";
    YTEAddFunction( &Transform::SetRotation, (void (Transform::*) (float, float, float)), "SetRotation", YTEParameterNames("aThetaX", "aThetaY", "aThetaZ"))
      ->Description() = "Sets the local rotation relative to parent from three individual Euler Angles X, Y, and Z (in degrees)";

    YTEBindProperty(&Transform::GetWorldTranslation, &Transform::SetWorldTranslationProperty, "WorldTranslation")->AddAttribute<EditorProperty>();
    YTEBindProperty(&Transform::GetWorldScale, &Transform::SetWorldScaleProperty, "WorldScale")->AddAttribute<EditorProperty>();
    YTEBindProperty(&Transform::GetWorldRotationAsEuler, &Transform::SetWorldRotationProperty, "WorldRotation")->AddAttribute<EditorProperty>();

    YTEAddFunction( &Transform::SetWorldRotation, (void (Transform::*) (const glm::vec3&)), "SetWorldRotation", YTEParameterNames("eulerAngles"))
      ->Description() = "SetWorlds the local rotation relative to parent from a Real3 of Euler Angles";
    YTEAddFunction( &Transform::SetWorldRotation, (void (Transform::*) (float, float, float)), "SetWorldRotation", YTEParameterNames("aThetaX", "aThetaY", "aThetaZ"))
      ->Description() = "SetWorlds the local rotation relative to parent from three individual Euler Angles X, Y, and Z (in degrees)";
  }


  Transform::Transform(Composition *aOwner, Space *aSpace, RSValue *aProperties)
    : Component(aOwner, aSpace),
      mScale(1.f, 1.f, 1.f),
      mWorldScale(1.f, 1.f, 1.f)
  {
    DeserializeByType<Transform*>(aProperties, 
      this, Transform::GetStaticType());
  };
    
  const glm::vec3& Transform::GetTranslation() const
  {
    return mTranslation;
  }

  void Transform::SetPhysicsTranslation(const glm::vec3& aTrans)
  {
    mTranslation = aTrans;

    PositionChanged newPosition;
    newPosition.Position = mTranslation;
    mOwner->SendEvent(Events::PositionChanged, &newPosition);
  }

  void Transform::SetTranslation(float aX, float aY, float aZ)
  {
    SetTranslation(glm::vec3(aX, aY, aZ));
  }

  void Transform::SetTranslation(const glm::vec3& aTrans)
  {
    mTranslation = aTrans;

    auto rigidBody = mOwner->GetComponent<RigidBody>();
    if (rigidBody != nullptr)
    {
      rigidBody->SetPhysicsTransform(mTranslation, mRotation);
    }

    PositionChanged newPosition;
    newPosition.Position = mTranslation;
    mOwner->SendEvent(Events::PositionChanged, &newPosition);
  }

  const glm::vec3& Transform::GetScale() const
  {
    return mScale;
  }

  void Transform::SetScale(const glm::vec3& aScale)
  {
    mScale = aScale;

    ScaleChanged newScale;
    newScale.Scale = mScale;
    mOwner->SendEvent(Events::ScaleChanged, &newScale);
  }

  void Transform::SetScale(float aX, float aY, float aZ)
  {
    mScale = glm::vec3(aX, aY, aZ);

    ScaleChanged newScale;
    newScale.Scale = mScale;
    mOwner->SendEvent(Events::ScaleChanged, &newScale);
  }

  const glm::quat& Transform::GetRotation() const
  {
    return mRotation;
  }

  glm::vec3 Transform::GetRotationAsEuler() const
  {
    return glm::eulerAngles(mRotation);
  }

  void Transform::SetRotation(const glm::quat& aRotation)
  {
    mRotation = aRotation;

    auto rigidBody = mOwner->GetComponent<RigidBody>();
    if (rigidBody != nullptr)
    {
      rigidBody->SetPhysicsTransform(mTranslation, mRotation);
    }

    RotationChanged newRotation;
    newRotation.Rotation = mRotation;
    mOwner->SendEvent(Events::RotationChanged, &newRotation);
  }

  void Transform::SetRotation(const glm::vec3& aEulerRot)
  {
    mRotation = glm::quat(aEulerRot);
    
    auto rigidBody = mOwner->GetComponent<RigidBody>();
    if (nullptr != rigidBody)
    {
      rigidBody->SetPhysicsTransform(mTranslation, mRotation);
    }
    
    RotationChanged newRotation;
    newRotation.Rotation = mRotation;
    mOwner->SendEvent(Events::RotationChanged, &newRotation);
  }

  void Transform::SetRotation(float aThetaX, float aThetaY, float aThetaZ)
  {
    mRotation = glm::quat(glm::vec3{ aThetaX, aThetaY, aThetaZ });

    auto rigidBody = mOwner->GetComponent<RigidBody>();
    if (nullptr != rigidBody)
    {
      rigidBody->SetPhysicsTransform(mTranslation, mRotation);
    }
    
    RotationChanged newRotation;
    newRotation.Rotation = mRotation;
    mOwner->SendEvent(Events::RotationChanged, &newRotation);
  }

  const glm::vec3& Transform::GetWorldTranslation() const
  {
    return mWorldTranslation;
  }

  void Transform::SetWorldTranslation(const glm::vec3& aTrans)
  {
    mWorldTranslation = aTrans;
  }

  void Transform::SetWorldTranslation(float aX, float aY, float aZ)
  {
    mWorldTranslation = glm::vec3(aX, aY, aZ);
  }

  const glm::vec3& Transform::GetWorldScale() const
  {
    return mWorldScale;
  }

  void Transform::SetWorldScale(const glm::vec3& aScale)
  {
    mWorldScale = aScale;
  }

  void Transform::SetWorldScale(float aX, float aY, float aZ)
  {
    mWorldScale = glm::vec3(aX, aY, aZ);
  }

  const glm::quat& Transform::GetWorldRotation() const
  {
    return mWorldRotation;
  }

  glm::vec3 Transform::GetWorldRotationAsEuler() const
  {
    return glm::eulerAngles(mWorldRotation);
  }

  void Transform::SetWorldRotation(const glm::quat& aRotation)
  {
    mWorldRotation = aRotation;
  }

  void Transform::SetWorldRotation(const glm::vec3& aEulerRot)
  {
    mWorldRotation = glm::quat(aEulerRot);
  }

  void Transform::SetWorldRotation(float aThetaX, float aThetaY, float aThetaZ)
  {
    mWorldRotation = glm::quat(glm::vec3{ aThetaX, aThetaY, aThetaZ });
  }
}
