/******************************************************************************/
/*!
* \author Joshua T. Fisher, Isaac Dayton
* \date   2016-03-06
*
* \copyright All content 2016 DigiPen (USA) Corporation, all rights reserved.
*/
/******************************************************************************/
#pragma once

#ifndef YTE_Physics_BoxCollider_h
#define YTE_Physics_BoxCollider_h

#include "YTE/Core/Component.hpp"

#include "YTE/Physics/Collider.hpp"
#include "YTE/Physics/Transform.hpp"

namespace YTE
{
  class BoxCollider : public Collider
  {
  public:
    YTEDeclareType(BoxCollider);

    BoxCollider(Composition *aOwner, Space *aSpace);

    void PhysicsInitialize() override;

    //const glm::vec3& GetSize() const { return mSize; }
    //const glm::vec3& GetOffset() const { return mOffset; }

    //void SetSize(const glm::vec3& aSize);
    //void SetSize(float aX, float aY, float aZ);
    //void SetOffset(const glm::vec3& aOffset);
    //void SetOffset(float aX, float aY, float aZ);

    void ScaleUpdate(TransformChanged *aEvent);

  private:
    UniquePointer<btBoxShape> mBoxShape;

    glm::vec3 mSize;    // Collider scale as a multiple of the scale of the object
    glm::vec3 mOffset;  // Collider position offset from the World Position of the object

    //inline void SetSizeProperty(const glm::vec3& aSize) { SetSize(aSize); }
    //inline void SetOffsetProperty(const glm::vec3& aOffset) { SetOffset(aOffset); }
  };
}

#endif
