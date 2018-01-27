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

    RigidBody(Composition *aOwner, Space *aSpace, RSValue *aProperties);

    ~RigidBody();

    void PhysicsInitialize() override;

    void ApplyImpulse(const glm::vec3& aImpulse, const glm::vec3& aRelativePosition);
    void SetPhysicsTransform(const glm::vec3& aTranslation, const glm::quat& aRotation);

    const glm::vec3 GetVelocity() const;
    void SetVelocity(const glm::vec3& aVelocity);
    void SetVelocity(float aVelX, float aVelY, float aVelZ);

    btRigidBody* GetBody() { return mRigidBody.get(); };

    void SetKinematic(bool flag);
    bool IsKinematic() const;

    void SetMass(float aMass);
    float GetMass() const;

    void SetGravity(glm::vec3 aAcceleration);
    glm::vec3 GetGravity();

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
