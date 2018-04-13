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


#include "YTE/Physics/CollisionBody.hpp"
#include "YTE/Physics/CapsuleCollider.hpp"
#include "YTE/Physics/GhostBody.hpp"
#include "YTE/Physics/PhysicsSystem.hpp"
#include "YTE/Physics/RigidBody.hpp"
#include "YTE/Physics/Transform.hpp"

namespace YTE
{
  YTEDefineType(CapsuleCollider)
  {
    YTERegisterType(CapsuleCollider);
    YTEBindField(&CapsuleCollider::mRadius, "Radius", PropertyBinding::GetSet)
      .SetDocumentation("Only works for getting. Setting is used exclusively for serialization.")
      .AddAttribute<EditorProperty>()
      .AddAttribute<Serializable>();

    YTEBindField(&CapsuleCollider::mHeight, "Height", PropertyBinding::GetSet)
      .SetDocumentation("Only works for getting. Setting is used exclusively for serialization.")
      .AddAttribute<EditorProperty>()
      .AddAttribute<Serializable>();
  }

  CapsuleCollider::CapsuleCollider(Composition *aOwner, Space *aSpace, RSValue *aProperties)
    : Collider(aOwner, aSpace), mRadius(1.0f), mHeight(1.0f)
  {
    DeserializeByType(aProperties, this, GetStaticType());
  }

  void CapsuleCollider::PhysicsInitialize()
  {
      // Get info from transform and feed that ish to the Bullet collider
    auto transform = mOwner->GetComponent<Transform>();
    auto translation = transform->GetTranslation();
    auto scale = transform->GetScale();
    auto bulletRot = OurQuatToBt(transform->GetRotation());
    auto bulletTransform = btTransform(bulletRot, btVector3(translation.x, translation.y, translation.z));

    mCapsuleShape = std::make_unique<btCapsuleShape>(mRadius, mHeight);
    mCapsuleShape->setLocalScaling(OurVec3ToBt(scale));

    mCollider = std::make_unique<btCollisionObject>();
    mCollider->setCollisionShape(mCapsuleShape.get());
    mCollider->setWorldTransform(bulletTransform);
    mCollider->setUserPointer(mOwner);
  }

  void CapsuleCollider::ScaleUpdate(TransformChanged *aEvent)
  {
    if (mCapsuleShape)
    {
      mCapsuleShape->setLocalScaling(OurVec3ToBt(aEvent->WorldScale));
    }
  }
}
