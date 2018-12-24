/******************************************************************************/
/*!
* \author Isaac Dayton
* \date   2015-10-16
*
* \copyright All content 2016 DigiPen (USA) Corporation, all rights reserved.
*/
/******************************************************************************/

#include "fmt/format.h"

#include <glm/gtx/transform.hpp>

#include "YTE/Core/Engine.hpp"
#include "YTE/Core/Space.hpp"

#include "YTE/Physics/PhysicsSystem.hpp"
#include "YTE/Physics/RigidBody.hpp"
#include "YTE/Physics/Transform.hpp"

#include "YTE/Utilities/Utilities.hpp"

namespace YTE
{
  YTEDefineEvent(PositionChanged);
  YTEDefineEvent(RotationChanged);
  YTEDefineEvent(ScaleChanged);

  YTEDefineType(TransformChanged)
  {
    RegisterType<TransformChanged>();
    TypeBuilder<TransformChanged> builder;
    builder.Field<&TransformChanged::Position>("Position", PropertyBinding::Get);
    builder.Field<&TransformChanged::Rotation>("Rotation", PropertyBinding::Get);
    builder.Field<&TransformChanged::Scale>("Scale", PropertyBinding::Get);
  }

  ///synchronizes world transform from user to physics
  void MotionState::getWorldTransform(btTransform& centerOfMassWorldTrans) const
  {
    centerOfMassWorldTrans.setOrigin(ToBullet(mTransform->GetWorldTranslation()));
    centerOfMassWorldTrans.setRotation(ToBullet(mTransform->GetWorldRotation()));
  }

  ///synchronizes world transform from physics to user
  ///Bullet only calls the update of world transform for active objects
  void MotionState::setWorldTransform(const btTransform& centerOfMassWorldTrans)
  {
    if (mKinematic)
    {
      return;
    }

    // If the motionstate is changing the transform, we don't need the transform to come back
    // and inform Bullet of changes.
    auto informOriginal = mTransform->GetInformPhysics();
    mTransform->SetInformPhysics(false);
    mTransform->SetWorldTranslation(ToGlm(centerOfMassWorldTrans.getOrigin()));
    mTransform->SetWorldRotation(ToGlm(centerOfMassWorldTrans.getRotation()));
    mTransform->SetInformPhysics(informOriginal);
  }

  YTEDefineType(Transform)
  {
    RegisterType<Transform>();
    TypeBuilder<Transform> builder;
    GetStaticType()->AddAttribute<RunInEditor>();

    builder.Property<&Transform::GetTranslation, &Transform::SetTranslationProperty>("Translation")
      .AddAttribute<Serializable>()
      .AddAttribute<EditorProperty>();

    builder.Property<&Transform::GetScale, &Transform::SetScaleProperty>("Scale")
      .AddAttribute<Serializable>()
      .AddAttribute<EditorProperty>();
    builder.Property<&Transform::GetRotation, &Transform::SetRotationPropertyQuat>("Rotation")
      .AddAttribute<Serializable>();
    builder.Property<&Transform::GetRotationAsEuler, &Transform::SetRotationProperty>("Rotation")
      .AddAttribute<EditorProperty>();

    builder.Function<SelectOverload<void (Transform::*) (const glm::vec3&)>(&Transform::SetRotation)>("SetRotation")
      .SetParameterNames("aEulerAngles")
      .SetDocumentation("Sets the local rotation relative to parent from a Real3 of Euler Angles");

    builder.Function<SelectOverload<void (Transform::*) (float, float, float)>(&Transform::SetRotation)>("SetRotation")
      .SetParameterNames("aThetaX", "aThetaY", "aThetaZ")
      .SetDocumentation("Sets the local rotation relative to parent from three individual Euler Angles X, Y, and Z (in degrees)");

    builder.Property<&Transform::GetWorldTranslation, &Transform::SetWorldTranslationProperty>("WorldTranslation")
      .AddAttribute<EditorProperty>(false);
    builder.Property<&Transform::GetWorldScale, &Transform::SetWorldScaleProperty>("WorldScale")
      .AddAttribute<EditorProperty>(false);
    builder.Property<&Transform::GetWorldRotationAsEuler, &Transform::SetWorldRotationProperty>("WorldRotation")
      .AddAttribute<EditorProperty>(false);

    builder.Function<SelectOverload<void (Transform::*) (const glm::vec3&)>(&Transform::SetWorldRotation)>("SetWorldRotation")
      .SetParameterNames("aEulerAngles")
      .SetDocumentation("SetWorlds the local rotation relative to parent from a Real3 of Euler Angles");
    builder.Function<SelectOverload<void (Transform::*) (float, float, float)>(&Transform::SetWorldRotation)>("SetWorldRotation")
      .SetParameterNames("aThetaX", "aThetaY", "aThetaZ")
      .SetDocumentation("SetWorlds the local rotation relative to parent from three individual Euler Angles X, Y, and Z (in degrees)");
  }

  Transform::Transform(Composition *aOwner, Space *aSpace)
    : Component(aOwner, aSpace)
    , mTranslation{ 0.f,0.f,0.f }
    , mScale{ 1.f, 1.f, 1.f }
    , mWorldTranslation{ 0.f,0.f,0.f }
    , mWorldScale{ 1.f, 1.f, 1.f }
    , mInformPhysics{ true }
  {
  };

  void Transform::Initialize()
  {
    auto parent{ mOwner->GetOwner() };

    mOwner->RegisterEvent<&Transform::ParentObjectChanged>(Events::ParentChanged, this);
    if (parent)
    {
      parent->RegisterEvent<&Transform::ParentPositionChanged>(Events::PositionChanged, this);
      parent->RegisterEvent<&Transform::ParentScaleChanged>(Events::ScaleChanged, this);
      parent->RegisterEvent<&Transform::ParentRotationChanged>(Events::RotationChanged, this);
    }
  }

  void Transform::ParentPositionChanged(TransformChanged *aEvent)
  {
    SetInternalTranslation(aEvent->WorldPosition, mTranslation);
  }

  void Transform::ParentScaleChanged(TransformChanged *aEvent)
  {
    SetInternalScale(aEvent->WorldScale, mScale);
  }

  void Transform::ParentRotationChanged(TransformChanged *aEvent)
  {
    auto localTranslation{ aEvent->WorldRotationDifference * mTranslation };

    SetInternalTranslation(aEvent->WorldPosition, localTranslation);
    SetInternalRotation(aEvent->WorldRotation, mRotation);
  }

  void Transform::ParentObjectChanged(ParentChanged *aEvent)
  {
    auto oldParent = aEvent->mOldParent;
    auto newParent = aEvent->mNewParent;

    if (oldParent)
    {
      oldParent->DeregisterEvent<&Transform::ParentPositionChanged>(Events::PositionChanged,  this);
      oldParent->DeregisterEvent<&Transform::ParentScaleChanged>(Events::ScaleChanged,  this);
      oldParent->DeregisterEvent<&Transform::ParentRotationChanged>(Events::RotationChanged,  this);
    }

    if (newParent)
    {
      newParent->RegisterEvent<&Transform::ParentPositionChanged>(Events::PositionChanged, this);
      newParent->RegisterEvent<&Transform::ParentScaleChanged>(Events::ScaleChanged, this);
      newParent->RegisterEvent<&Transform::ParentRotationChanged>(Events::RotationChanged, this);
    }

    // set translation
    auto parentTrans = aEvent->mNewParent->GetComponent<Transform>();

    if (parentTrans)
    {
      mTranslation = mTranslation - parentTrans->GetTranslation();
    }
  }

  ////////////////////////////////////////////////////////////////////////////
  // Local Getters
  ////////////////////////////////////////////////////////////////////////////
  const glm::vec3& Transform::GetTranslation() const
  {
    return mTranslation;
  }

  const glm::vec3& Transform::GetScale() const
  {
    return mScale;
  }

  const glm::quat& Transform::GetRotation() const
  {
    return mRotation;
  }

  glm::vec3 Transform::GetRotationAsEuler() const
  {
    return glm::degrees(glm::eulerAngles(mRotation));
  }

  glm::vec3 Transform::GetRotationAsEulerRadians() const
  {
    return glm::eulerAngles(mRotation);
  }

  ////////////////////////////////////////////////////////////////////////////
  // World Getters
  ////////////////////////////////////////////////////////////////////////////
  const glm::vec3& Transform::GetWorldTranslation() const
  {
    return mWorldTranslation;
  }

  const glm::vec3& Transform::GetWorldScale() const
  {
    return mWorldScale;
  }

  const glm::quat& Transform::GetWorldRotation() const
  {
    return mWorldRotation;
  }

  glm::vec3 Transform::GetWorldRotationAsEuler() const
  {
    return glm::degrees(glm::eulerAngles(mWorldRotation));
  }

  ////////////////////////////////////////////////////////////////////////////
  // Local Setters
  ////////////////////////////////////////////////////////////////////////////
  void Transform::SetTranslation(float aX, float aY, float aZ)
  {
    glm::vec3 local{ aX, aY, aZ };
    SetTranslation(local);
  }

  void Transform::SetTranslation(const glm::vec3& aLocalTranslation)
  {
    auto parentTranslation{ GetAccumulatedParentTranslation() };

    SetInternalTranslation(parentTranslation, aLocalTranslation);
  }

  void Transform::SetScale(float aX, float aY, float aZ)
  {
    glm::vec3 local{ aX, aY, aZ };
    SetScale(local);
  }

  void Transform::SetScale(const glm::vec3& aLocalScale)
  {
    auto parentScale{ GetAccumulatedParentScale() };

    SetInternalScale(parentScale, aLocalScale);
  }

  void Transform::SetRotation(float aThetaX, float aThetaY, float aThetaZ)
  {
    glm::vec3 localRotation{ aThetaX, aThetaY, aThetaZ };
    SetRotation(localRotation);
  }

  void Transform::SetRotation(const glm::vec3& aEulerRotation)
  {
    glm::vec3 eulerAngles = glm::radians(aEulerRotation);

    const glm::vec3 localXAxis{ 1.0f, 0.0f, 0.0f };
    const glm::vec3 localYAxis{ 0.0f, 1.0f, 0.0f };
    const glm::vec3 localZAxis{ 0.0f, 0.0f, 1.0f };

    auto localX = glm::rotate(eulerAngles.x, localXAxis);
    auto localY = glm::rotate(eulerAngles.y, localYAxis);
    auto localZ = glm::rotate(eulerAngles.z, localZAxis);

    auto localRotation = localZ * localY * localX;

    glm::quat local{ localRotation };

    SetRotation(local);
  }

  void Transform::SetRotation(const glm::quat& aLocalRotation)
  {
    auto parentRotation{ GetAccumulatedParentRotation() };

    SetInternalRotation(parentRotation, aLocalRotation);
  }


  void Transform::RotateAboutLocalAxis(glm::vec3 aAxis, float aAngle)
  {
    auto rotation = YTE::AroundAxis(aAxis, aAngle);

    SetRotation(mRotation * rotation);
  }

  void Transform::RotateAboutWorldAxis(const glm::vec3& aAxis, float aAngle)
  {
    auto rotation = YTE::AroundAxis(aAxis, aAngle);

    SetRotation(rotation * mRotation);
  }

  void Transform::RotateTowardPoint(const glm::vec3& aTargetPoint, const glm::vec3& aUpVector)
  {
    RotateToward(aTargetPoint - mWorldTranslation,
                 aUpVector);
  }

  void Transform::RotateToward(const glm::vec3& aTargetVector, const glm::vec3& aUpVector)
  {
    SetWorldRotation(glm::vec3(0.f));

    float magB = glm::length(aTargetVector);

    if (magB != 0)
    {
      float angle = glm::acos(glm::dot(glm::vec3(0.0f, 0.0f, 1.0), aTargetVector) / (magB));
      auto rotation = YTE::AroundAxis(aUpVector, angle);

      SetRotation(mRotation * rotation);
    }
  }

  void Transform::RotateAround(glm::vec3 aAxis, float aAngle, glm::vec3 aPoint)
  {
    // TODO: (Josh) Implement
    UnusedArguments(aAxis, aAngle, aPoint);
  }

  ////////////////////////////////////////////////////////////////////////////
  // World Setters
  ////////////////////////////////////////////////////////////////////////////
  void Transform::SetWorldTranslation(float aX, float aY, float aZ)
  {
    glm::vec3 world{ aX, aY, aZ };
    SetWorldTranslation(world);
  }

  void Transform::SetWorldTranslation(const glm::vec3& aWorldTranslation)
  {
    auto parent{ mWorldTranslation - mTranslation };
    auto local{ aWorldTranslation - parent };
    SetInternalTranslation(parent, local);
  }

  void Transform::SetWorldScale(float aX, float aY, float aZ)
  {
    glm::vec3 world{ aX, aY, aZ };
    SetWorldScale(world);
  }

  void Transform::SetWorldScale(const glm::vec3& aWorldScale)
  {
    glm::vec3 parent{};
    glm::vec3 local{0.f, 0.f, 0.f};

    if (0.0f != glm::length(mScale))
    {
      parent =  mWorldScale / mScale;
      local =  aWorldScale / parent;
    }
    else if (parent = GetAccumulatedParentScale();
             0.0f != glm::length(parent))
    {
      local = aWorldScale / parent;
    }

    SetInternalScale(parent, local);
  }

  void Transform::SetWorldRotation(float aThetaX, float aThetaY, float aThetaZ)
  {
    glm::vec3 world{ aThetaX, aThetaY, aThetaZ };
    SetWorldRotation(world);
  }

  void Transform::SetWorldRotation(const glm::vec3& aWorldEulerRotation)
  {
    glm::vec3 eulerAngles = glm::radians(aWorldEulerRotation);

    const glm::vec3 worldXAxis{ 1.0f, 0.0f, 0.0f };
    const glm::vec3 worldYAxis{ 0.0f, 1.0f, 0.0f };
    const glm::vec3 worldZAxis{ 0.0f, 0.0f, 1.0f };

    auto worldX = glm::rotate(eulerAngles.x, worldXAxis);
    auto worldY = glm::rotate(eulerAngles.y, worldYAxis);
    auto worldZ = glm::rotate(eulerAngles.z, worldZAxis);

    auto localRotation = worldZ * worldY * worldX;

    glm::quat world{ localRotation };

    SetRotation(world);
  }

  void Transform::SetWorldRotation(const glm::quat& aWorldRotation)
  {
    auto parent{ GetAccumulatedParentRotation() };
    auto local{ aWorldRotation * glm::inverse(parent) };

    SetInternalRotation(parent, local);
  }

  ////////////////////////////////////////////////////////////////////////////
  // Internal Sets
  ////////////////////////////////////////////////////////////////////////////
  void Transform::SetInternalTranslation(const glm::vec3 &aParentTranslation, const glm::vec3 &aLocalTranslation)
  {
    mWorldTranslation = aParentTranslation + aLocalTranslation;
    mTranslation = aLocalTranslation;

    SendTransformEvents(Events::PositionChanged);
  }

  void Transform::SetInternalScale(const glm::vec3 &aParentScale, const glm::vec3 &aLocalScale)
  {
    mWorldScale = aParentScale * aLocalScale;
    mScale = aLocalScale;

    SendTransformEvents(Events::ScaleChanged);
  }

  void Transform::SetInternalRotation(const glm::quat &aParentRotation, const glm::quat &aLocalRotation)
  {
    auto parent = glm::normalize(aParentRotation);
    auto local = glm::normalize(aLocalRotation);

    auto newWorldRotation = glm::normalize(parent * local);

    auto worldDifference{ glm::normalize(newWorldRotation) * glm::inverse(mWorldRotation) };
    auto localDifference{ local * glm::normalize(glm::inverse(mRotation)) };

    mWorldRotation = newWorldRotation;
    mRotation = local;

    SendTransformEvents(Events::RotationChanged, localDifference, worldDifference);
  }

  ////////////////////////////////////////////////////////////////////////////
  // Helpers
  ////////////////////////////////////////////////////////////////////////////
  // TODO: Make these functions faster by using WorldTransforms.
  glm::vec3 Transform::GetAccumulatedParentTranslation()
  {
    glm::vec3 translation;
    auto parent{ mOwner->GetOwner() };

    while (nullptr != parent)
    {
      auto transform{ parent->GetComponent<Transform>() };

      if (nullptr != transform)
      {
        translation += transform->mTranslation;
      }

      parent = parent->GetOwner();
    }

    return translation;
  }

  glm::vec3 Transform::GetAccumulatedParentScale()
  {
    glm::vec3 scale{ 1.0f, 1.0f, 1.0f };
    auto parent{ mOwner->GetOwner() };

    while (nullptr != parent)
    {
      auto transform{ parent->GetComponent<Transform>() };

      if (nullptr != transform)
      {
        scale *= transform->mScale;
      }

      parent = parent->GetOwner();
    }

    return scale;
  }

  glm::quat Transform::GetAccumulatedParentRotation()
  {
    glm::quat rotation;
    auto parent{ mOwner->GetOwner() };

    while (nullptr != parent)
    {
      auto transform{ parent->GetComponent<Transform>() };

      if (nullptr != transform)
      {
        rotation *= transform->mRotation;
      }

      parent = parent->GetOwner();
    }

    return rotation;
  }

  void Transform::SendTransformEvents(const std::string &aEvent,
                                      glm::quat aLocalRotationDifference,
                                      glm::quat aWorldRotationDifference)
  {
    if (mInformPhysics)
    {
      // TODO: Inform physics of scale change.
      auto rigidBody = mOwner->GetComponent<RigidBody>();

      if (rigidBody != nullptr)
      {
        rigidBody->SetPhysicsTransform(mWorldTranslation, mWorldRotation);
      }
    }

    TransformChanged newTransform;
    newTransform.Position = mTranslation;
    newTransform.Scale = mScale;
    newTransform.Rotation = mRotation;
    newTransform.WorldPosition = mWorldTranslation;
    newTransform.WorldScale = mWorldScale;
    newTransform.WorldRotation = mWorldRotation;
    newTransform.LocalRotationDifference = aLocalRotationDifference;
    newTransform.WorldRotationDifference = aWorldRotationDifference;

    mOwner->SendEvent(aEvent, &newTransform);
  }


  glm::mat4 Transform::GetTransformMatrix()
  {
    auto matrix = glm::translate(glm::mat4(1.0f), mWorldTranslation);
    matrix = matrix * glm::toMat4(mWorldRotation);
    matrix = glm::scale(matrix, mWorldScale);

    return matrix;
  }
}