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

#include "YTE/Physics/CollisionBody.h"
#include "YTE/Physics/GhostBody.h"
#include "YTE/Physics/RigidBody.h"
#include "YTE/Physics/SphereCollider.h"
#include "YTE/Physics/Transform.h"

namespace YTE
{
  DefineType(SphereCollider)
  {
    YTERegisterType(SphereCollider);
    YTEBindProperty(&SphereCollider::GetRadius, &SphereCollider::SetRadius, "Radius")->AddAttribute<EditorProperty>();
    YTEBindProperty(&SphereCollider::GetOffset, &SphereCollider::SetOffsetProperty, "Offset")->AddAttribute<EditorProperty>();

    YTEAddFunction( &SphereCollider::SetRadius, (void (SphereCollider::*) (float)), "SetRadius", YTEParameterNames("aRadius"))
      ->Description() = "Sets the collider scale as a multiple of the scale of the transform";

    YTEAddFunction( &SphereCollider::SetOffset, (void (SphereCollider::*) (const glm::vec3&)), "SetOffset", YTEParameterNames("aOffset"))
      ->Description() = "Sets the collider position offset from the World Translation of the transform";
    YTEAddFunction( &SphereCollider::SetOffset, (void (SphereCollider::*) (float, float, float)), "SetOffset", YTEParameterNames("aX", "aY", "aZ"))
      ->Description() = "Sets the collider position offset from the World Translation of the transform";
  }

  SphereCollider::SphereCollider(Composition *aOwner, Space *aSpace, RSValue *aProperties)
    : Collider(aOwner, aSpace), mRadius(1.f)
  {
    DeserializeByType<SphereCollider*>(aProperties, 
      this, SphereCollider::GetStaticType());
  }

  void SphereCollider::PhysicsInitialize()
  {
    DebugObjection(mOwner->GetComponent<RigidBody>()     == nullptr && 
                mOwner->GetComponent<CollisionBody>() == nullptr && 
                mOwner->GetComponent<GhostBody>()     == nullptr,
                "Colliders require a Body component of some sort, sorry!\n ObjectName: %s", mOwner->GetName().c_str());

      // Get info from transform and feed that ish to the Bullet collider
    auto transform = mOwner->GetComponent<Transform>();
    auto translation = transform->GetTranslation();
    auto scale = transform->GetScale() / 2.0f;
    auto rotation = transform->GetRotation();
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
      auto scale = mOwner->GetComponent<Transform>()->GetScale() / 2.0f;
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
      auto translation = transform->GetTranslation();
      auto rotation = transform->GetRotation();
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
}
