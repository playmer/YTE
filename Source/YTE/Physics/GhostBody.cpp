/******************************************************************************/
/*!
* \author Joshua T. Fisher, 
* \date   2015-11-20
*
* \copyright All content 2016 DigiPen (USA) Corporation, all rights reserved.
*/
/******************************************************************************/
#include "YTE/Core/Engine.hpp"
#include "YTE/Core/Space.hpp"

#include "Bullet/btBulletCollisionCommon.h"
#include "Bullet/BulletCollision/CollisionDispatch/btGhostObject.h"

#include "YTE/Physics/PhysicsSystem.hpp"
#include "YTE/Physics/GhostBody.hpp"
#include "YTE/Physics/Transform.hpp"

namespace YTE
{
  YTEDefineType(GhostBody)
  {
    YTERegisterType(GhostBody);
  }

  GhostBody::GhostBody(Composition *aOwner, Space *aSpace, RSValue *aProperties)
    : Body(aOwner, aSpace, aProperties), mVelocity(0.f, 0.f, 0.f), mIsInitialized(false)
  {
    DeserializeByType<GhostBody*>(aProperties, this, GhostBody::GetStaticType());
  };

  GhostBody::~GhostBody()
  {
    auto world = mSpace->GetComponent<PhysicsSystem>()->GetWorld();
    world->removeCollisionObject(mGhostBody.get());
  }

  void GhostBody::PhysicsInitialize()
  {
    auto world = mSpace->GetComponent<PhysicsSystem>()->GetWorld();
    auto collider = GetColliderFromObject(mOwner);

    DebugObjection(collider == nullptr,
                "CollisionBodies require a collider currently, sorry!\n ObjectName: %s",
                mOwner->GetName().c_str());

    auto baseCollider = collider->GetCollider();
    auto collisionShape = baseCollider->getCollisionShape();

    auto transform = mOwner->GetComponent<Transform>();

    //using motionstate is recommended, it provides interpolation capabilities, and only synchronizes 'active' objects

    mGhostBody = std::make_unique<btGhostObject>();
    mGhostBody->setCollisionShape(collisionShape);
    //mGhostBody->activate(true);
    //mGhostBody->setActivationState(DISABLE_DEACTIVATION);
    mGhostBody->setUserPointer(mOwner);

    btTransform translationAndRotation;
    translationAndRotation.setOrigin(OurVec3ToBt(transform->GetTranslation()));
    translationAndRotation.setRotation(OurQuatToBt(transform->GetRotation()));
    mGhostBody->setWorldTransform(translationAndRotation);

    world->addCollisionObject(mGhostBody.get());

    mIsInitialized = true;
  }
}
