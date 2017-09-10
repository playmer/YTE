/******************************************************************************/
/*!
* \author Joshua T. Fisher
* \date   2015-11-20
*
* \copyright All content 2016 DigiPen (USA) Corporation, all rights reserved.
*/
/******************************************************************************/
#include "YTE/Core/Engine.hpp"
#include "YTE/Core/Space.hpp"

#include "Bullet/btBulletCollisionCommon.h"
#include "Bullet/BulletCollision/CollisionDispatch/btCollisionObject.h"


#include "YTE/Physics/PhysicsSystem.hpp"
#include "YTE/Physics/CollisionBody.hpp"
#include "YTE/Physics/Transform.hpp"

namespace YTE
{
  DefineType(CollisionBody)
  {
    YTERegisterType(CollisionBody);
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


  CollisionBody::CollisionBody(Composition *aOwner, Space *aSpace, RSValue *aProperties)
    : Body(aOwner, aSpace, aProperties), mVelocity(0.f, 0.f, 0.f), mIsInitialized(false)
  {
    DeserializeByType<CollisionBody*>(aProperties, this, CollisionBody::GetStaticType());
  };

  CollisionBody::~CollisionBody()
  {
    auto world = mSpace->GetComponent<PhysicsSystem>()->GetWorld();
    world->removeCollisionObject(mCollisionBody.get());
  }

  void CollisionBody::PhysicsInitialize()
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

    mCollisionBody = std::make_unique<btCollisionObject>();
    mCollisionBody->setCollisionShape(collisionShape);
    mCollisionBody->activate(true);
    mCollisionBody->setActivationState(DISABLE_DEACTIVATION);
    mCollisionBody->setUserPointer(mOwner);

    world->addCollisionObject(mCollisionBody.get());

    mIsInitialized = true;
  }
}
