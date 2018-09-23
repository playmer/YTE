/******************************************************************************/
/*!
* \author Isaac Dayton
* \date   2016-03-06
*
* \copyright All content 2016 DigiPen (USA) Corporation, all rights reserved.
*/
/******************************************************************************/
#pragma once

#ifndef YTE_Physics_SphereCollider_h
#define YTE_Physics_SphereCollider_h

#include "YTE/Core/Component.hpp"

#include "YTE/Physics/Collider.hpp"
#include "YTE/Physics/Transform.hpp"

namespace YTE
{
  class SphereCollider : public Collider
  {
  public:
    YTEDeclareType(SphereCollider);

    YTE_Shared SphereCollider(Composition *aOwner, Space *aSpace);

    YTE_Shared void PhysicsInitialize() override;

    float GetRadius() const { return mRadius; }
    const glm::vec3& GetOffset() const { return mOffset; }

    YTE_Shared void SetRadius(float aRadius);
    YTE_Shared void SetOffset(const glm::vec3& aOffset);
    YTE_Shared void SetOffset(float aX, float aY, float aZ);

    YTE_Shared void ScaleUpdate(TransformChanged *aEvent);

  private:
    UniquePointer<btSphereShape> mSphereShape;

    float mRadius;           // Collider scale as a multiple of the scale of the object
    glm::vec3 mOffset;  // Collider position offset from the World Position of the object

    inline void SetOffsetProperty(const glm::vec3& aOffset) { SetOffset(aOffset); }
  };
}

#endif
