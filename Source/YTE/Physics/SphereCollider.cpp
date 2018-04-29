/******************************************************************************/
/*!
* \author Isaac Dayton
* \date   2016-03-06
*
* \copyright All content 2016 DigiPen (USA) Corporation, all rights reserved.
*/
/******************************************************************************/
#include "YTE/Core/Composition.hpp"

#include "YTE/Core/Engine.hpp"
#include "YTE/Core/Space.hpp"

#include "YTE/Physics/PhysicsSystem.hpp"
#include "YTE/Physics/CollisionBody.hpp"
#include "YTE/Physics/GhostBody.hpp"
#include "YTE/Physics/RigidBody.hpp"
#include "YTE/Physics/SphereCollider.hpp"
#include "YTE/Physics/Transform.hpp"

namespace YTE
{
  YTEDefineType(SphereCollider)
  {
    RegisterType<SphereCollider>();

    std::vector<std::vector<Type*>> deps = { { TypeId<Transform>() } };

    GetStaticType()->AddAttribute<ComponentDependencies>(deps);

    YTEBindProperty(&SphereCollider::GetRadius, &SphereCollider::SetRadius, "Radius")
      .AddAttribute<Serializable>()
      .AddAttribute<EditorProperty>();
    YTEBindProperty(&SphereCollider::GetOffset, &SphereCollider::SetOffsetProperty, "Offset")
      .AddAttribute<Serializable>()
      .AddAttribute<EditorProperty>();

    YTEBindFunction(&SphereCollider::SetRadius, (void (SphereCollider::*) (float)), "SetRadius", YTEParameterNames("aRadius"))
      .SetDocumentation("Sets the collider scale as a multiple of the scale of the transform");

    YTEBindFunction(&SphereCollider::SetOffset, (void (SphereCollider::*) (const glm::vec3&)), "SetOffset", YTEParameterNames("aOffset"))
      .SetDocumentation("Sets the collider position offset from the World Translation of the transform");
    YTEBindFunction(&SphereCollider::SetOffset, (void (SphereCollider::*) (float, float, float)), "SetOffset", YTEParameterNames("aX", "aY", "aZ"))
      .SetDocumentation("Sets the collider position offset from the World Translation of the transform");
  }

  SphereCollider::SphereCollider(Composition *aOwner, Space *aSpace, RSValue *aProperties)
    : Collider(aOwner, aSpace), mRadius(1.0f)
  {
    DeserializeByType(aProperties, this, GetStaticType());
  }

  void SphereCollider::PhysicsInitialize()
  {
      // Get info from transform and feed that ish to the Bullet collider
    auto transform = mOwner->GetComponent<Transform>();
    auto translation = transform->GetWorldTranslation();
    auto scale = transform->GetWorldScale() / 2.0f;
    auto rotation = transform->GetWorldRotation();
    auto bulletRot = btQuaternion(rotation.x, rotation.y, rotation.z, rotation.w);
    auto bulletTransform = btTransform(bulletRot,
                                        btVector3(translation.x + mOffset.x,
                                                  translation.y + mOffset.y,
                                                  translation.z + mOffset.z));

    mSphereShape = std::make_unique<btSphereShape>(scale.x * mRadius);

    mCollider = std::make_unique<btCollisionObject>();
    mCollider->setCollisionShape(mSphereShape.get());
    mCollider->setWorldTransform(bulletTransform);
  }

  void SphereCollider::SetRadius(float aRadius)
  {
    mRadius = aRadius;

      // Update the underlying collider if it exists
    if (mSphereShape)
    {
      glm::vec3 a;
      auto scale = mOwner->GetComponent<Transform>()->GetWorldScale() / 2.0f;
      auto localScaling = btVector3(aRadius * scale.x, aRadius * scale.x, aRadius * scale.x);

      mSphereShape->setLocalScaling(localScaling);
    }
  }

  void SphereCollider::SetOffset(const glm::vec3& aOffset)
  {
    mOffset = aOffset;

      // Update the underlying collider if it exists
    if (mCollider)
    {
      auto transform = mOwner->GetComponent<Transform>();
      auto translation = transform->GetWorldTranslation();
      auto rotation = transform->GetWorldRotation();
      auto bulletRot = btQuaternion(rotation.x, rotation.y, rotation.z, rotation.w);
      auto bulletTransform = btTransform(bulletRot,
                                          btVector3(translation.x + mOffset.x,
                                                    translation.y + mOffset.y,
                                                    translation.z + mOffset.z));

      mCollider->setWorldTransform(bulletTransform);
    }
  }

  void SphereCollider::SetOffset(float aX, float aY, float aZ)
  {
    auto offset = glm::vec3(aX, aY, aZ);

    SetOffset(offset);
  }

  void SphereCollider::ScaleUpdate(TransformChanged *aEvent)
  {
    if (mSphereShape)
    {
      mSphereShape->setLocalScaling(OurVec3ToBt(aEvent->WorldScale));
    }
  }
}
