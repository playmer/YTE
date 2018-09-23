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

#include "YTE/Physics/BoxCollider.hpp"
#include "YTE/Physics/CapsuleCollider.hpp"
#include "YTE/Physics/CylinderCollider.hpp"
#include "YTE/Physics/MenuCollider.hpp"
#include "YTE/Physics/MeshCollider.hpp"
#include "YTE/Physics/SphereCollider.hpp"

namespace YTE
{
  YTEDefineType(RigidBody)
  {
    RegisterType<RigidBody>();
    TypeBuilder<RigidBody> builder;
    builder.Property<&RigidBody::GetVelocity, &RigidBody::SetVelocityProperty>("Velocity")
      .AddAttribute<EditorProperty>()
      .AddAttribute<Serializable>();

    std::vector<std::vector<Type*>> deps = { { TypeId<Transform>() },
                                             { TypeId<BoxCollider>(),
                                               TypeId<CapsuleCollider>(),
                                               TypeId<CylinderCollider>(),
                                               TypeId<MenuCollider>(),
                                               TypeId<MeshCollider>(),
                                               TypeId<SphereCollider>() } };

    GetStaticType()->AddAttribute<ComponentDependencies>(deps);

    builder.Property<&RigidBody::GetMass, &RigidBody::SetMassProperty>("Mass")
      .SetDocumentation("This is the mass of the object, but you should know that it is not dynamically changeable.")
      .AddAttribute<EditorProperty>()
      .AddAttribute<Serializable>();

    builder.Property<&RigidBody::IsKinematic, &RigidBody::SetKinematic>("Kinematic")
      .SetDocumentation("If the object is kinematic, it can move. Non-kinematic objects are static.")
      .AddAttribute<EditorProperty>()
      .AddAttribute<Serializable>();

    builder.Property<&RigidBody::GetGravity, &RigidBody::SetGravity>("Gravity")
      .SetDocumentation("This is the acceleration due to gravity")
      .AddAttribute<EditorProperty>()
      .AddAttribute<Serializable>();

    builder.Function<&RigidBody::ApplyImpulse>("ApplyImpulse")
      .SetParameterNames("aImpulse", "aRelativePositon")
      .SetDocumentation("Applies an impulse to the RigidBody.");

    builder.Function<SelectOverload<void (RigidBody::*) (const glm::vec3&),&RigidBody::SetVelocity>()>("SetVelocity")
      .SetParameterNames("aVelocityVector")
      .SetDocumentation("Sets the object velocity from a Real3 of values");
    builder.Function<SelectOverload<void (RigidBody::*) (float, float, float),&RigidBody::SetVelocity>()>("SetVelocity")
      .SetParameterNames("aVelX", "aVelY","aVelZ")
      .SetDocumentation("Sets the object velocity from three float values");
  }

  RigidBody::RigidBody(Composition *aOwner, Space *aSpace)
    : Body(aOwner, aSpace)
    , mVelocity(0.f, 0.f, 0.f)
    , mMass(1.0f)
    , mStatic(false)
    , mIsInitialized(false)
    , mKinematic(true)
  {
  };

  RigidBody::~RigidBody()
  {
    if (mRigidBody)
    {
      auto world = mSpace->GetComponent<PhysicsSystem>()->GetWorld();
      world->removeRigidBody(mRigidBody.get());
    }
  }

  void RigidBody::PhysicsInitialize()
  {
    auto world = mSpace->GetComponent<PhysicsSystem>()->GetWorld();
    auto collider = GetColliderFromObject(mOwner);

    //rigidbody is dynamic if and only if mass is non zero, otherwise static
    bool isDynamic = (mMass != 0.f) && !mStatic;
    DebugObjection((mMass != 0.f) && mStatic,
      "Shouldn't make a non-zero mass object that's also static.\n ObjectName: %s",
      mOwner->GetName().c_str());
    DebugObjection((mMass == 0.f) && !mStatic,
      "Can't make a zero mass object that's also non-static.\n ObjectName: %s",
      mOwner->GetName().c_str());

    btVector3 localInertia(0.f, 0.f, 0.f);
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
      mRigidBody->setLinearVelocity(ToBullet(mVelocity));
      mRigidBody->setActivationState(DISABLE_DEACTIVATION);
      mRigidBody->setDamping(0.8f, 0.8f);
      mRigidBody->updateInertiaTensor();
    }

    world->addRigidBody(mRigidBody.get());
    mRigidBody->setGravity(ToBullet(mGravityAcceleration));

    mIsInitialized = true;
  }

  void RigidBody::ApplyForce(const glm::vec3& aForce, const glm::vec3& aRelativePosition)
  {
    mRigidBody->applyForce(ToBullet(aForce), ToBullet(aRelativePosition));
  }

  void RigidBody::ApplyImpulse(const glm::vec3 & aImpulse, const glm::vec3 & aRelativePosition)
  {
    mRigidBody->applyImpulse(ToBullet(aImpulse), ToBullet(aRelativePosition));
  }

  void RigidBody::SetPhysicsTransform(const glm::vec3& aTranslation, const glm::quat& aRotation)
  {
    if (mRigidBody.get() != nullptr)
    {
      btTransform transform;
      transform.setOrigin(ToBullet(aTranslation));
      transform.setRotation(ToBullet(aRotation));

      mRigidBody->setWorldTransform(transform);

      // set the collider's translation
      auto collider = GetColliderFromObject(mOwner);
      collider->SetTranslation(aTranslation.x, aTranslation.y, aTranslation.z);
    }
  }

  const glm::vec3 RigidBody::GetAngularVelocity() const
  {
    if (mIsInitialized)
    {
      auto velocity = ToGlm(mRigidBody->getAngularVelocity());
      return velocity;
    }
    return glm::vec3();
  }

  const glm::vec3 RigidBody::GetVelocity() const
  {
    if (mIsInitialized)
    {
      auto velocity = ToGlm(mRigidBody->getLinearVelocity());
      return velocity;
    }
    else
    {
      return mVelocity;
    }
  }

  void RigidBody::SetVelocity(const glm::vec3& aVelocity)
  {
    mVelocity = aVelocity;

    if (mIsInitialized)
    {
      mRigidBody->setLinearVelocity(ToBullet(mVelocity));
    }
  }

  void RigidBody::SetVelocity(float aVelX, float aVelY, float aVelZ)
  {
    if (mIsInitialized)
    {
      mRigidBody->setLinearVelocity(btVector3(aVelX, aVelY, aVelZ));
    }
    else
    {
      mVelocity = glm::vec3(aVelX, aVelY, aVelZ);
    }
  }

  void RigidBody::SetKinematic(bool flag)
  {
    mKinematic = flag;
    if (mMotionState)
    {
      mMotionState->SetKinematic(mKinematic);
    }
  }

  bool RigidBody::IsKinematic() const
  {
    if (mMotionState)
    {
      return mMotionState->IsKinematic();
    }

    return false;
  }

  void RigidBody::SetMass(float aMass)
  {
    if (aMass == 0.0)
    {
      mStatic = true;
    }
    else
    {
      mStatic = false;
    }

    mMass = aMass;

    if (mRigidBody)
    {
      mRigidBody->setMassProps(mMass, btVector3{ 0.0f, 0.0f, 0.0f });
    }
  }

  float RigidBody::GetMass() const
  {
    return mMass;
  }

  void RigidBody::SetGravity(glm::vec3 aAcceleration)
  {
    mGravityAcceleration = aAcceleration;
    btVector3 accel = ToBullet(aAcceleration);
    if (mRigidBody)
    {
      mRigidBody->setGravity(accel);
    }
  }

  glm::vec3 RigidBody::GetGravity()
  {
    return mGravityAcceleration;
  }

  void RigidBody::SetDamping(float aLinearDamp, float aAngularDamp)
  {
    mRigidBody->setDamping(aLinearDamp, aAngularDamp);
  }
}
