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

#include "CylinderCollider.hpp"
#include "CollisionBody.hpp"
#include "GhostBody.hpp"
#include "PhysicsSystem.hpp"
#include "RigidBody.hpp"
#include "Transform.hpp"

namespace YTE
{
  YTEDefineType(CylinderCollider)
  {
    RegisterType<CylinderCollider>();
    TypeBuilder<CylinderCollider> builder;

    std::vector<std::vector<Type*>> deps = { { TypeId<Transform>() } };

    GetStaticType()->AddAttribute<ComponentDependencies>(deps);
  }

  CylinderCollider::CylinderCollider(Composition *aOwner, Space *aSpace, RSValue *aProperties)
    : Collider(aOwner, aSpace)
  {
    DeserializeByType(aProperties, this, GetStaticType());
  }

  void CylinderCollider::PhysicsInitialize()
  {
      // Get info from transform and feed that ish to the Bullet collider
    auto transform = mOwner->GetComponent<Transform>();
    auto translation = transform->GetTranslation();
    auto scale = transform->GetScale();
    auto rotation = transform->GetRotation();
    auto bulletTransform = btTransform(ToBullet(rotation), ToBullet(translation));

    mCylinderShape = std::make_unique<btCylinderShape>(btVector3(1.f, 1.f, 1.f));
    mCylinderShape->setLocalScaling(ToBullet(scale));

    mCollider = std::make_unique<btCollisionObject>();
    mCollider->setCollisionShape(mCylinderShape.get());
    mCollider->setWorldTransform(bulletTransform);
  }

  void CylinderCollider::ScaleUpdate(TransformChanged *aEvent)
  {
    if (mCylinderShape)
    {
      auto scale = aEvent->WorldScale;
      mCylinderShape->setLocalScaling(ToBullet(scale));
    }
  }
}
