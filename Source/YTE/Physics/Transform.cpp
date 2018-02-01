/******************************************************************************/
/*!
* \author Isaac Dayton
* \date   2015-10-16
*
* \copyright All content 2016 DigiPen (USA) Corporation, all rights reserved.
*/
/******************************************************************************/

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
    YTERegisterType(TransformChanged);
    YTEBindField(&TransformChanged::Position, "Position", PropertyBinding::Get);
    YTEBindField(&TransformChanged::Rotation, "Rotation", PropertyBinding::Get);
    YTEBindField(&TransformChanged::Scale, "Scale", PropertyBinding::Get);
  }

  ///synchronizes world transform from user to physics
  void MotionState::getWorldTransform(btTransform& centerOfMassWorldTrans) const
  {
    centerOfMassWorldTrans.setOrigin(OurVec3ToBt(mTransform->GetWorldTranslation()));
    centerOfMassWorldTrans.setRotation(OurQuatToBt(mTransform->GetWorldRotation()));
  }

  ///synchronizes world transform from physics to user
  ///Bullet only calls the update of world transform for active objects
  void MotionState::setWorldTransform(const btTransform& centerOfMassWorldTrans)
  {
    if (mKinematic)
      return;

    mTransform->SetWorldTranslation(BtToOurVec3(centerOfMassWorldTrans.getOrigin()));
    mTransform->SetWorldRotation(BtToOurQuat(centerOfMassWorldTrans.getRotation()));
  }

  YTEDefineType(Transform)
  {
    YTERegisterType(Transform);
    YTEBindProperty(&Transform::GetTranslation, &Transform::SetTranslationProperty, "Translation")
      .AddAttribute<Serializable>()
      .AddAttribute<EditorProperty>();

    YTEBindProperty(&Transform::GetScale, &Transform::SetScaleProperty, "Scale")
      .AddAttribute<Serializable>()
      .AddAttribute<EditorProperty>();
    YTEBindProperty(&Transform::GetRotation, &Transform::SetRotationPropertyQuat, "Rotation")
      .AddAttribute<Serializable>();
    YTEBindProperty(&Transform::GetRotationAsEuler, &Transform::SetRotationProperty, "Rotation")
      .AddAttribute<EditorProperty>();

    YTEBindFunction(&Transform::SetRotation, (void (Transform::*) (const glm::vec3&)), "SetRotation", YTEParameterNames("eulerAngles"))
      .Description() = "Sets the local rotation relative to parent from a Real3 of Euler Angles";
    YTEBindFunction(&Transform::SetRotation, (void (Transform::*) (float, float, float)), "SetRotation", YTEParameterNames("aThetaX", "aThetaY", "aThetaZ"))
      .Description() = "Sets the local rotation relative to parent from three individual Euler Angles X, Y, and Z (in degrees)";

    YTEBindProperty(&Transform::GetWorldTranslation, &Transform::SetWorldTranslationProperty, "WorldTranslation")
      .AddAttribute<EditorProperty>(false);
    YTEBindProperty(&Transform::GetWorldScale, &Transform::SetWorldScaleProperty, "WorldScale")
      .AddAttribute<EditorProperty>(false);
    YTEBindProperty(&Transform::GetWorldRotationAsEuler, &Transform::SetWorldRotationProperty, "WorldRotation")
      .AddAttribute<EditorProperty>(false);

    YTEBindFunction(&Transform::SetWorldRotation, (void (Transform::*) (const glm::vec3&)), "SetWorldRotation", YTEParameterNames("eulerAngles"))
      .Description() = "SetWorlds the local rotation relative to parent from a Real3 of Euler Angles";
    YTEBindFunction(&Transform::SetWorldRotation, (void (Transform::*) (float, float, float)), "SetWorldRotation", YTEParameterNames("aThetaX", "aThetaY", "aThetaZ"))
      .Description() = "SetWorlds the local rotation relative to parent from three individual Euler Angles X, Y, and Z (in degrees)";
  }

  Transform::Transform(Composition *aOwner, Space *aSpace, RSValue *aProperties)
    : Component(aOwner, aSpace)
    , mScale(1.f, 1.f, 1.f)
    , mWorldScale(1.f, 1.f, 1.f)
  {
    DeserializeByType(aProperties, this, GetStaticType());
  };

  void Transform::Initialize()
  {
    auto parent{ mOwner->GetOwner() };

    mOwner->YTERegister(Events::ParentChanged, this, &Transform::ParentObjectChanged);
    if (parent)
    {
      parent->YTERegister(Events::PositionChanged, this, &Transform::ParentPositionChanged);
      parent->YTERegister(Events::ScaleChanged, this, &Transform::ParentScaleChanged);
      parent->YTERegister(Events::RotationChanged, this, &Transform::ParentRotationChanged);
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
    auto localTranslation{ aEvent->RotationDifference * mTranslation };

    SetInternalTranslation(aEvent->WorldPosition, localTranslation);
    SetInternalRotation(aEvent->WorldRotation, mRotation);
  }

  void Transform::ParentObjectChanged(ParentChanged * aEvent)
  {
    auto oldParent = aEvent->mOldParent;
    auto newParent = aEvent->mNewParent;
    if (oldParent)
    {
      oldParent->YTEDeregister(Events::PositionChanged, this, &Transform::ParentPositionChanged);
      oldParent->YTEDeregister(Events::ScaleChanged, this, &Transform::ParentScaleChanged);
      oldParent->YTEDeregister(Events::RotationChanged, this, &Transform::ParentRotationChanged);
    }

    if (newParent)
    {
      newParent->YTERegister(Events::PositionChanged, this, &Transform::ParentPositionChanged);
      newParent->YTERegister(Events::ScaleChanged, this, &Transform::ParentScaleChanged);
      newParent->YTERegister(Events::RotationChanged, this, &Transform::ParentRotationChanged);
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

    const glm::vec3 localXAxis{1.0f, 0.0f, 0.0f};
    const glm::vec3 localYAxis{0.0f, 1.0f, 0.0f};
    const glm::vec3 localZAxis{0.0f, 0.0f, 1.0f};

    auto localX = glm::rotate(eulerAngles.x, localXAxis);
    auto localY = glm::rotate(eulerAngles.y, localYAxis);
    auto localZ = glm::rotate(eulerAngles.z, localZAxis);

    auto localRotation = localZ * localY * localX;

    glm::quat local{localRotation};

    SetRotation(local);
  }

  void Transform::SetRotation(const glm::quat& aLocalRotation)
  {
    auto parentRotation{ GetAccumulatedParentRotation() };

    SetInternalRotation(parentRotation, aLocalRotation);
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
    auto parent{ mWorldScale - mScale };
    auto local{ aWorldScale - parent };
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

    const glm::vec3 worldXAxis{1.0f, 0.0f, 0.0f};
    const glm::vec3 worldYAxis{0.0f, 1.0f, 0.0f};
    const glm::vec3 worldZAxis{0.0f, 0.0f, 1.0f};
    
    auto worldX = glm::rotate(eulerAngles.x, worldXAxis);
    auto worldY = glm::rotate(eulerAngles.y, worldYAxis);
    auto worldZ = glm::rotate(eulerAngles.z, worldZAxis);
    
    auto localRotation = worldZ * worldY * worldX;
    
    glm::quat world{localRotation};
    
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

    InformPhysicsOfChange(Events::PositionChanged);
  }

  void Transform::SetInternalScale(const glm::vec3 &aParentScale, const glm::vec3 &aLocalScale)
  {
    mWorldScale = aParentScale * aLocalScale;
    mScale = aLocalScale;

    InformPhysicsOfChange(Events::ScaleChanged);
  }

  void Transform::SetInternalRotation(const glm::quat &aParentRotation, const glm::quat &aLocalRotation)
  {
    auto parent = glm::normalize(aParentRotation);
    auto local = glm::normalize(aLocalRotation);

    auto difference{ glm::normalize(glm::inverse(mRotation) * local) };

    mWorldRotation = glm::normalize(parent * local);
    mRotation = local;

    InformPhysicsOfChange(Events::RotationChanged, difference);
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
    glm::vec3 scale{1.0f, 1.0f, 1.0f};
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

  void Transform::InformPhysicsOfChange(const std::string &aEvent, glm::quat aRotationDifference)
  {
    // TODO: Inform physics of scale change.
    auto rigidBody = mOwner->GetComponent<RigidBody>();
    if (rigidBody != nullptr)
    {
      rigidBody->SetPhysicsTransform(mTranslation, mRotation);
    }

    TransformChanged newTransform;
    newTransform.Position = mTranslation;
    newTransform.Scale = mScale;
    newTransform.Rotation = mRotation;
    newTransform.WorldPosition = mWorldTranslation;
    newTransform.WorldScale = mWorldScale;
    newTransform.WorldRotation = mWorldRotation;
    newTransform.RotationDifference = aRotationDifference;

    mOwner->SendEvent(aEvent, &newTransform);
  }
}
