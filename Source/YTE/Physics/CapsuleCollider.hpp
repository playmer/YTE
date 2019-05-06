#pragma once

#ifndef YTE_Physics_CapsuleCollider_h
#define YTE_Physics_CapsuleCollider_h

#include "YTE/Core/Component.hpp"

#include "YTE/Physics/Collider.hpp"
#include "YTE/Physics/Transform.hpp"

namespace YTE
{
  class CapsuleCollider : public Collider
  {
  public:
    YTEDeclareType(CapsuleCollider);

    CapsuleCollider(Composition *aOwner, Space *aSpace);

    void PhysicsInitialize();

    void ScaleUpdate(TransformChanged *aEvent);

  private:
    UniquePointer<btCapsuleShape> mCapsuleShape;

    float mRadius;
    float mHeight;

  };
}

#endif
