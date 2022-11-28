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
    RegisterType<GhostBody>();
    TypeBuilder<GhostBody> builder;

    std::vector<std::vector<Type*>> deps = { { TypeId<Transform>() },
                                             { TypeId<BoxCollider>(),
                                               TypeId<CapsuleCollider>(),
                                               TypeId<CylinderCollider>(),
                                               TypeId<MenuCollider>(),
                                               TypeId<MeshCollider>(),
                                               TypeId<SphereCollider>() } };

    GetStaticType()->AddAttribute<ComponentDependencies>(deps);
  }

  GhostBody::GhostBody(Composition* aOwner, Space* aSpace)
    : Body{ aOwner, aSpace }
    , mVelocity{ 0.f, 0.f, 0.f }
    , mIsInitialized{ false }
  {
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
    translationAndRotation.setOrigin(ToBullet(transform->GetWorldTranslation()));
    translationAndRotation.setRotation(ToBullet(transform->GetWorldRotation()));
    mGhostBody->setWorldTransform(translationAndRotation);

    world->addCollisionObject(mGhostBody.get());

    mIsInitialized = true;

    mOwner->RegisterEvent<&GhostBody::TransformEvent>(Events::PositionChanged, this);
  }

  void GhostBody::TransformEvent(TransformChanged *aEvent)
  {
    btTransform translationAndRotation;
    translationAndRotation.setOrigin(ToBullet(aEvent->WorldPosition));
    translationAndRotation.setRotation(ToBullet(aEvent->WorldRotation));
    mGhostBody->setWorldTransform(translationAndRotation);
  }
}
