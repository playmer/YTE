/******************************************************************************/
/*!
 * \author Joshua T. Fisher
 * \date   2015-11-20
 *
 * \copyright All content 2016 DigiPen (USA) Corporation, all rights reserved.
*/
/******************************************************************************/

#pragma once

#ifndef YTE_Physics_RigidBody_h
#define YTE_Physics_RigidBody_h

#include "YTE/Core/Component.hpp"

#include "YTE/Physics/Body.hpp"
#include "YTE/Physics/Collider.hpp"
#include "YTE/Physics/ForwardDeclarations.hpp"

namespace YTE
{
  class RigidBody : public Body
  {
  public:
    YTEDeclareType(RigidBody);

    YTE_Shared RigidBody(Composition *aOwner, Space *aSpace, RSValue *aProperties);

    YTE_Shared ~RigidBody();

    YTE_Shared void PhysicsInitialize() override;

    YTE_Shared void ApplyForce(const glm::vec3& aForce, const glm::vec3& aRelativePosition);
    YTE_Shared void ApplyImpulse(const glm::vec3& aImpulse, const glm::vec3& aRelativePosition);
    YTE_Shared void SetPhysicsTransform(const glm::vec3& aTranslation, const glm::quat& aRotation);

    YTE_Shared const glm::vec3 GetAngularVelocity() const;

    YTE_Shared const glm::vec3 GetVelocity() const;
    YTE_Shared void SetVelocity(const glm::vec3& aVelocity);
    YTE_Shared void SetVelocity(float aVelX, float aVelY, float aVelZ);

    btRigidBody* GetBody() { return mRigidBody.get(); };

    YTE_Shared void SetKinematic(bool flag);
    YTE_Shared bool IsKinematic() const;

    YTE_Shared void SetMass(float aMass);
    YTE_Shared float GetMass() const;

    YTE_Shared void SetGravity(glm::vec3 aAcceleration);
    YTE_Shared glm::vec3 GetGravity();

    YTE_Shared void SetDamping(float aLinearDamp, float aAngularDamp);

  private:
    UniquePointer<btRigidBody> mRigidBody;
    UniquePointer<MotionState> mMotionState;

    glm::vec3 mGravityAcceleration;
    glm::vec3 mVelocity;
    float mMass;
    bool mStatic;
    bool mIsInitialized;
    bool mKinematic;

    inline void SetVelocityProperty(const glm::vec3& aVelocity) { SetVelocity(aVelocity); };
    inline void SetMassProperty(float aMass) { SetMass(aMass); };


  };
}
#endif
