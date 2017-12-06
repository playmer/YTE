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

#include "YTE/Physics/PhysicsSystem.hpp"
#include "YTE/Physics/RigidBody.hpp"
#include "YTE/Physics/Transform.hpp"

namespace YTE
{
  YTEDefineType(RigidBody)
  {
    YTERegisterType(RigidBody);
    YTEBindProperty(&RigidBody::GetVelocity, &RigidBody::SetVelocityProperty, "Velocity")
      .AddAttribute<EditorProperty>()
      .AddAttribute<Serializable>();

    YTEBindField(&RigidBody::mMass, "Mass", PropertyBinding::GetSet)
      .SetDocumentation("This is the mass of the object, but you should know that it is not dynamically changeable")
      .AddAttribute<EditorProperty>()
      .AddAttribute<Serializable>();

    YTEBindField(&RigidBody::mStatic, "Static", PropertyBinding::GetSet)
      .SetDocumentation("This is the mass of the object, but you should know that it is not dynamically changeable")
      .AddAttribute<EditorProperty>()
      .AddAttribute<Serializable>();

    YTEBindFunction(&RigidBody::ApplyImpulse, YTENoOverload, "ApplyImpulse", YTEParameterNames("aImpulse", "aRelativePositon"))
      .Description() = "Applies an impulse to the RigidBody.";

    YTEBindFunction(&RigidBody::SetVelocity, (void (RigidBody::*) (const glm::vec3&)), "SetVelocity", YTEParameterNames("aVelocityVector"))
      .Description() = "Sets the object velocity from a Real3 of values";
    YTEBindFunction(&RigidBody::SetVelocity, (void (RigidBody::*) (float, float, float)), "SetVelocity", YTEParameterNames("aVelX", "aVelY", "aVelZ"))
      .Description() = "Sets the object velocity from three float values";
  }

  RigidBody::RigidBody(Composition *aOwner, Space *aSpace, RSValue *aProperties)
    : Body(aOwner, aSpace, aProperties), mVelocity(0.f, 0.f, 0.f), mMass(1.0f), mStatic(false), mIsInitialized(false)
  {
    DeserializeByType<RigidBody*>(aProperties,  this, RigidBody::GetStaticType());
  };

  RigidBody::~RigidBody() 
  {
    auto world = mSpace->GetComponent<PhysicsSystem>()->GetWorld();
    world->removeRigidBody(mRigidBody.get());
  }

  void RigidBody::PhysicsInitialize()
  {
    auto world = mSpace->GetComponent<PhysicsSystem>()->GetWorld();
    auto collider =  GetColliderFromObject(mOwner);

    DebugObjection(collider == nullptr, 
                "RigidBodies require a collider currently, sorry!\n ObjectName: %s",
                mOwner->GetName().c_str());

    //rigidbody is dynamic if and only if mass is non zero, otherwise static
    bool isDynamic = (mMass != 0.f) && !mStatic;
    DebugObjection((mMass != 0.f) && mStatic, 
                "Shouldn't make a non-zero mass object that's also static.\n ObjectName: %s", 
                mOwner->GetName().c_str());
    DebugObjection((mMass == 0.f) && !mStatic, 
                "Can't make a zero mass object that's also non-static.\n ObjectName: %s", 
                mOwner->GetName().c_str());

    btVector3 localInertia (0.f, 0.f, 0.f);
    auto baseCollider = collider->GetCollider();
    auto collisionShape = baseCollider->getCollisionShape();

    if (isDynamic)
    {
      baseCollider->getCollisionShape()->calculateLocalInertia(mMass, localInertia);
    }

    auto transform = mOwner->GetComponent<Transform>();

    //using motionstate is recommended, it provides interpolation capabilities, and only synchronizes 'active' objects
    mMotionState = std::make_unique<MotionState>(transform);
    btRigidBody::btRigidBodyConstructionInfo rbInfo(mMass, mMotionState.get(), collisionShape, localInertia);
    rbInfo.m_friction = 0.0f;
    rbInfo.m_restitution = 0.0f;
    rbInfo.m_mass = mMass;
    rbInfo.m_rollingFriction = 0.0;

    mRigidBody = std::make_unique<btRigidBody>(rbInfo);
    mRigidBody->setUserPointer(mOwner);


    if (isDynamic)
    {
      //mRigidBody->setAngularFactor(btVector3(1, 1, 1));
      //mRigidBody->setAngularFactor(btVector3(0, 0, 0));
      mRigidBody->setLinearVelocity(OurVec3ToBt(mVelocity));
      mRigidBody->setActivationState(DISABLE_DEACTIVATION);
      mRigidBody->updateInertiaTensor();
    }

    world->addRigidBody(mRigidBody.get());

    mIsInitialized = true;
  }

  void RigidBody::ApplyImpulse(const glm::vec3 & aImpulse, const glm::vec3 & aRelativePosition)
  {
    mRigidBody->applyImpulse(OurVec3ToBt(aImpulse), OurVec3ToBt(aRelativePosition));
  }

  void RigidBody::SetPhysicsTransform(const glm::vec3& aTranslation, const glm::quat& aRotation)
  {
    if (mRigidBody.get() != nullptr)
    {
      btTransform transform;
      transform.setOrigin(OurVec3ToBt(aTranslation));
      transform.setRotation(OurQuatToBt(aRotation));

      mRigidBody->setWorldTransform(transform);

      // set the collider's translation
      auto collider = GetColliderFromObject(mOwner);
      collider->SetTranslation(aTranslation.x, aTranslation.y, aTranslation.z);

    }
  }

  const glm::vec3 RigidBody::GetVelocity() const
  {
    if (mIsInitialized)
    {
      auto velocity = BtToOurVec3(mRigidBody->getLinearVelocity());
      return velocity;
    }
    else
    {
      return mVelocity;
    }
  }

  void RigidBody::SetVelocity(const glm::vec3& aVelocity)
  {
    if (mIsInitialized)
    {
      mRigidBody->setLinearVelocity(OurVec3ToBt(mVelocity));
    }
    else
    {
      mVelocity = aVelocity;
    }

    // TODO@@@(Isaac): SEND UPDATED VELOCITY EVENT
  }

  void RigidBody::SetVelocity(float aVelX, float aVelY, float aVelZ)
  {

    //std::cout << "Setting Velocity (Probably don't do this.) " << std::endl;

    if (mIsInitialized)
    {
      mRigidBody->setLinearVelocity(btVector3(aVelX, aVelY, aVelZ));
    }
    else
    {
      mVelocity = glm::vec3(aVelX, aVelY, aVelZ);
    }

    // TODO@@@(Isaac): SEND UPDATED VELOCITY EVENT
  }

  void RigidBody::SetKinematic(bool flag)
  {
    mMotionState->SetKinematic(flag);
  }
}
