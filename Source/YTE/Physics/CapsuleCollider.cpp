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
#include "YTE/Physics/CapsuleCollider.h"
#include "YTE/Physics/GhostBody.h"
#include "YTE/Physics/PhysicsSystem.h"
#include "YTE/Physics/RigidBody.h"
#include "YTE/Physics/Transform.h"

namespace YTE
{
  DefineType(CapsuleCollider)
  {
    YTERegisterType(CapsuleCollider);
    auto radius = YTEBindField(&CapsuleCollider::mRadius, "Radius", PropertyBinding::GetSet);
    radius->Description() = "Only works for getting. Setting is used exclusively for serialization.";
    radius->AddAttribute<EditorProperty>();

    auto height = YTEBindField(&CapsuleCollider::mHeight, "Height", PropertyBinding::GetSet);
    height->Description() = "Only works for getting. Setting is used exclusively for serialization.";
    height->AddAttribute<EditorProperty>();
  }

  CapsuleCollider::CapsuleCollider(Composition *aOwner, Space *aSpace, RSValue *aProperties)
    : Collider(aOwner, aSpace), mRadius(0.0f), mHeight(0.0f)
  {
    DeserializeByType<CapsuleCollider*>(aProperties, this, CapsuleCollider::GetStaticType());
  }

  void CapsuleCollider::PhysicsInitialize()
  {
    DebugAssert(mOwner->GetComponent<RigidBody>()     == nullptr && 
                mOwner->GetComponent<CollisionBody>() == nullptr && 
                mOwner->GetComponent<GhostBody>()     == nullptr,
                "Colliders require a Body component of some sort, sorry!\n ObjectName: %s", mOwner->GetName().c_str());

      // Get info from transform and feed that ish to the Bullet collider
    auto transform = mOwner->GetComponent<Transform>();
    auto translation = transform->GetTranslation();
    auto scale = transform->GetScale();
    auto bulletRot = OurQuatToBt(transform->GetRotation());
    auto bulletTransform = btTransform(bulletRot, btVector3(translation.x, translation.y, translation.z));

    mCapsuleShape = std::make_unique<btCapsuleShape>(mRadius, mHeight);

    mCollider = std::make_unique<btCollisionObject>();
    mCollider->setCollisionShape(mCapsuleShape.get());
    mCollider->setWorldTransform(bulletTransform);
    mCollider->setUserPointer(mOwner);
  }
}
