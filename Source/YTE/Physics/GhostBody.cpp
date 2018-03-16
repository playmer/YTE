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

#include "btBulletCollisionCommon.h"
#include "BulletCollision/CollisionDispatch/btGhostObject.h"

#include "YTE/Physics/PhysicsSystem.hpp"
#include "YTE/Physics/GhostBody.hpp"
#include "YTE/Physics/Transform.hpp"

#include "YTE/Physics/BoxCollider.hpp"
#include "YTE/Physics/CapsuleCollider.hpp"
#include "YTE/Physics/CylinderCollider.hpp"
#include "YTE/Physics/MenuCollider.hpp"
#include "YTE/Physics/MeshCollider.hpp"
#include "YTE/Physics/SphereCollider.hpp"

namespace YTE
{
  YTEDefineType(GhostBody)
  {
    YTERegisterType(GhostBody);

    std::vector<std::vector<Type*>> deps = { { TypeId<Transform>() },
                                             { TypeId<BoxCollider>(),
                                               TypeId<CapsuleCollider>(),
                                               TypeId<CylinderCollider>(),
                                               TypeId<MenuCollider>(),
                                               TypeId<MeshCollider>(),
                                               TypeId<SphereCollider>() } };

    GetStaticType()->AddAttribute<ComponentDependencies>(deps);
  }

  GhostBody::GhostBody(Composition *aOwner, Space *aSpace, RSValue *aProperties)
    : Body(aOwner, aSpace, aProperties), mVelocity(0.f, 0.f, 0.f), mIsInitialized(false)
  {
    DeserializeByType(aProperties, this, GetStaticType());
  };

  GhostBody::~GhostBody()
  {
    if (mGhostBody)
    {
      auto world = mSpace->GetComponent<PhysicsSystem>()->GetWorld();
      world->removeCollisionObject(mGhostBody.get());
    }
  }

  void GhostBody::PhysicsInitialize()
  {
    auto world = mSpace->GetComponent<PhysicsSystem>()->GetWorld();
    auto collider = GetColliderFromObject(mOwner);

    auto baseCollider = collider->GetCollider();
    auto collisionShape = baseCollider->getCollisionShape();

    auto transform = mOwner->GetComponent<Transform>();

    //using motionstate is recommended, it provides interpolation capabilities, and only synchronizes 'active' objects

    mGhostBody = std::make_unique<btGhostObject>();
    mGhostBody->setCollisionShape(collisionShape);
    //mGhostBody->activate(true);
    //mGhostBody->setActivationState(DISABLE_DEACTIVATION);
    mGhostBody->setCollisionFlags(mGhostBody->getCollisionFlags() | btCollisionObject::CF_NO_CONTACT_RESPONSE);
    mGhostBody->setUserPointer(mOwner);

    btTransform translationAndRotation;
    translationAndRotation.setOrigin(OurVec3ToBt(transform->GetWorldTranslation()));
    translationAndRotation.setRotation(OurQuatToBt(transform->GetWorldRotation()));
    mGhostBody->setWorldTransform(translationAndRotation);

    world->addCollisionObject(mGhostBody.get());

    mIsInitialized = true;
  }
}
