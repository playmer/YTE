#include "YTE/Core/Engine.hpp"
#include "YTE/Core/Space.hpp"

#include "btBulletCollisionCommon.h"
#include "BulletCollision/CollisionDispatch/btCollisionObject.h"

#include "YTE/Physics/PhysicsSystem.hpp"
#include "YTE/Physics/CollisionBody.hpp"
#include "YTE/Physics/Transform.hpp"

namespace YTE
{
  YTEDefineType(CollisionBody)
  {
    RegisterType<CollisionBody>();
    TypeBuilder<CollisionBody> builder;
  }
  
  CollisionBody::CollisionBody(Composition *aOwner, Space *aSpace)
    : Body(aOwner, aSpace)
    , mVelocity(0.f, 0.f, 0.f)
    , mIsInitialized(false)
  {
  };

  CollisionBody::~CollisionBody()
  {
    if (mCollisionBody)
    {
      auto world = mSpace->GetComponent<PhysicsSystem>()->GetWorld();
      world->removeCollisionObject(mCollisionBody.get());
    }
  }

  void CollisionBody::PhysicsInitialize()
  {
    auto world = mSpace->GetComponent<PhysicsSystem>()->GetWorld();
    auto collider = GetColliderFromObject(mOwner);

    auto baseCollider = collider->GetCollider();
    auto collisionShape = baseCollider->getCollisionShape();

    //using motionstate is recommended, it provides interpolation capabilities, and only synchronizes 'active' objects
    mCollisionBody = std::make_unique<btCollisionObject>();
    mCollisionBody->setCollisionShape(collisionShape);
    mCollisionBody->activate(true);
    mCollisionBody->setActivationState(DISABLE_DEACTIVATION);
    mCollisionBody->setUserPointer(mOwner);

    world->addCollisionObject(mCollisionBody.get());

    mIsInitialized = true;
  }
}
