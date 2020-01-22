#pragma once

#ifndef YTE_Physics_GhostBody_h
#define YTE_Physics_GhostBody_h

#include "YTE/Core/Component.hpp"

#include "YTE/Physics/Body.hpp"
#include "YTE/Physics/Collider.hpp"
#include "YTE/Physics/ForwardDeclarations.hpp"

namespace YTE
{
  class GhostBody : public Body
  {
  public:
    YTEDeclareType(GhostBody);

    GhostBody(Composition *aOwner, Space *aSpace);

    ~GhostBody() override;

    void PhysicsInitialize() override;

    void TransformEvent(TransformChanged *aEvent);

    btGhostObject* GetBody() { return mGhostBody.get(); };

  private:
    UniquePointer<btGhostObject> mGhostBody;
    UniquePointer<MotionState> mMotionState;

    glm::vec3 mVelocity;
    bool mIsInitialized;
  };
}
#endif
