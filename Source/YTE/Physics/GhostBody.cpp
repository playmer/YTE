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


#include "YTE/Physics/PhysicsSystem.h"
#include "YTE/Physics/GhostBody.h"
#include "YTE/Physics/Transform.h"

namespace YTE
{
  DefineType(GhostBody)
  {
    YTERegisterType(GhostBody);
  }

  class MotionState : public btMotionState
  {
  public:
    MotionState(Transform *aTransform) : mTransform(aTransform) {};
    ///synchronizes world transform from user to physics
    virtual void	getWorldTransform(btTransform& centerOfMassWorldTrans) const
    {
      centerOfMassWorldTrans.setOrigin(OurVec3ToBt(mTransform->GetTranslation()));
      centerOfMassWorldTrans.setRotation(OurQuatToBt(mTransform->GetRotation()));
    }

    ///synchronizes world transform from physics to user
    ///Bullet only calls the update of worldtransform for active objects
    virtual void	setWorldTransform(const btTransform& centerOfMassWorldTrans)
    {
      mTransform->SetTranslation(BtToOurVec3(centerOfMassWorldTrans.getOrigin()));
      mTransform->SetRotation(BtToOurQuat(centerOfMassWorldTrans.getRotation()));
    }

  private:
    Transform *mTransform;
  };


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
