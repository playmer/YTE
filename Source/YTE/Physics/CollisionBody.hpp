/******************************************************************************/
/*!
* \author Joshua T. Fisher
* \date   2015-11-20
*
* \copyright All content 2016 DigiPen (USA) Corporation, all rights reserved.
*/
/******************************************************************************/

#pragma once

#ifndef YTE_Physics_CollisionBody_h
#define YTE_Physics_CollisionBody_h

#include "YTE/Core/Component.hpp"

#include "YTE/Physics/Body.hpp"
#include "YTE/Physics/Collider.hpp"
#include "YTE/Physics/ForwardDeclarations.hpp"

namespace YTE
{
  class CollisionBody : public Body
  {
  public:
    YTEDeclareType(CollisionBody);

    CollisionBody(Composition *aOwner, Space *aSpace, RSValue *aProperties);

    ~CollisionBody() override;

    void PhysicsInitialize();

    btCollisionObject* GetBody() { return mCollisionBody.get(); };

  private:
    UniquePointer<btCollisionObject> mCollisionBody;
    UniquePointer<MotionState> mMotionState;

    glm::vec3 mVelocity;
    bool mIsInitialized;
  };
}
#endif
