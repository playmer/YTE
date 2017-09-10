/******************************************************************************/
/*!
* \author Joshua T. Fisher
* \date   2015-11-20
*
* \copyright All content 2016 DigiPen (USA) Corporation, all rights reserved.
*/
/******************************************************************************/

#pragma once

#ifndef YTE_Physics_GhostBody_h
#define YTE_Physics_GhostBody_h

#include "YTE/Core/Component.hpp"



#include "YTE/Physics/Body.h"
#include "YTE/Physics/Collider.h"
#include "YTE/Physics/ForwardDeclarations.h"

namespace YTE
{
  class MotionState;

  class GhostBody : public Body
  {
  public:
    DeclareType(GhostBody);

    GhostBody(Composition *aOwner, Space *aSpace, RSValue *aProperties);

    ~GhostBody() override;

    void PhysicsInitialize();

    btGhostObject* GetBody() { return mGhostBody.get(); };

  private:
    UniquePointer<btGhostObject> mGhostBody;
    UniquePointer<MotionState> mMotionState;

    glm::vec3 mVelocity;
    bool mIsInitialized;
  };
}
#endif
