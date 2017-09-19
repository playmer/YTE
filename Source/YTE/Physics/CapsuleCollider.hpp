/******************************************************************************/
/*!
* \author Isaac Dayton
* \date   2016-03-06
*
* \copyright All content 2016 DigiPen (USA) Corporation, all rights reserved.
*/
/******************************************************************************/
#pragma once

#ifndef YTE_Physics_CapsuleCollider_h
#define YTE_Physics_CapsuleCollider_h

#include "YTE/Core/Component.hpp"

#include "YTE/Physics/Collider.hpp"

namespace YTE
{
  class CapsuleCollider : public Collider
  {
  public:
    YTEDeclareType(CapsuleCollider);

    CapsuleCollider(Composition *aOwner, Space *aSpace, RSValue *aProperties);

    void PhysicsInitialize();

  private:
    UniquePointer<btCapsuleShape> mCapsuleShape;

    float mRadius;
    float mHeight;

  };
}

#endif
