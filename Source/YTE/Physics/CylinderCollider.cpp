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

#include "CylinderCollider.h"
#include "CollisionBody.h"
#include "GhostBody.h"
#include "PhysicsSystem.h"
#include "RigidBody.h"
#include "Transform.h"

namespace YTE
{
  DefineType(CylinderCollider)
  {
    YTERegisterType(CylinderCollider);
  }

  CylinderCollider::CylinderCollider(Composition *aOwner, Space *aSpace, RSValue *aProperties)
    : Collider(aOwner, aSpace)
  {
    DeserializeByType<CylinderCollider*>(aProperties, this, CylinderCollider::GetStaticType());
  }

  void CylinderCollider::PhysicsInitialize()
  {
    DebugObjection(mOwner->GetComponent<RigidBody>()     == nullptr && 
                mOwner->GetComponent<CollisionBody>() == nullptr && 
                mOwner->GetComponent<GhostBody>()     == nullptr,
                "Colliders require a Body component of some sort, sorry!\n ObjectName: %s", mOwner->GetName().c_str());

      // Get info from transform and feed that ish to the Bullet collider
    auto transform = mOwner->GetComponent<Transform>();
    auto translation = transform->GetTranslation();
    auto scale = transform->GetScale();
    auto rotation = transform->GetRotation();
    auto bulletRot = btQuaternion(rotation.x, rotation.y, rotation.z, rotation.w);
    auto bulletTransform = btTransform(bulletRot, btVector3(translation.x, translation.y, translation.z));

    mCylinderShape = std::make_unique<btCylinderShape>(btVector3(1.f,1.f,1.f));

    mCollider = std::make_unique<btCollisionObject>();
    mCollider->setCollisionShape(mCylinderShape.get());
    mCollider->setWorldTransform(bulletTransform);
  }
}
