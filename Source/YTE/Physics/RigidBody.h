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

#include "YTE/Event/StandardEvents.h"

#include "YTE/Physics/Body.h"
#include "YTE/Physics/Collider.h"
#include "YTE/Physics/ForwardDeclarations.h"

namespace YTE
{
  class MotionState;

  class RigidBody : public Body
  {
  public:
    DeclareType(RigidBody);

    RigidBody(Composition *aOwner, Space *aSpace, RSValue *aProperties);

    ~RigidBody();

    void PhysicsInitialize();

    void ApplyImpulse(const glm::vec3& aImpulse, const glm::vec3& aRelativePosition);
    void SetPhysicsTransform(const glm::vec3& aTranslation, const glm::quat& aRotation);

    const glm::vec3 GetVelocity() const;
    void SetVelocity(const glm::vec3& aVelocity);
    void SetVelocity(float aVelX, float aVelY, float aVelZ);

    btRigidBody* GetBody() { return mRigidBody.get(); };

    void SetKinematic(bool flag);

  private:
    UniquePointer<btRigidBody> mRigidBody;
    UniquePointer<MotionState> mMotionState;

    glm::vec3 mVelocity;
    float mMass;
    bool mStatic;
    bool mIsInitialized;

    inline void SetVelocityProperty(const glm::vec3& aVelocity) { SetVelocity(aVelocity); };
  };
}
#endif
