/******************************************************************************/
/*!
* \author Isaac Dayton
* \date   2015-11-27
*
* \copyright All content 2016 DigiPen (USA) Corporation, all rights reserved.
*/
/******************************************************************************/
#pragma once

#ifndef YTE_Physics_MenuCollider_h
#define YTE_Physics_MenuCollider_h

#include "YTE/Core/Component.hpp"

#include "YTE/Physics/Collider.hpp"
#include "YTE/Physics/ForwardDeclarations.hpp"

namespace YTE
{
  class MenuCollider : public Collider
  {
  public:
    YTEDeclareType(MenuCollider);

    MenuCollider(Composition *aOwner, Space *aSpace, RSValue *aProperties);

    void Initialize() override;
    void OnPositionChanged(PositionChanged *aEvent);
    void OnScaleChanged(ScaleChanged* aEvent);

    void PointToBox2D(const glm::vec2& aPoint);
    void PointToBox2D(const glm::vec2& aBox2D, const glm::vec2& aPoint);

    // Accessors
    const glm::vec3& GetSize();
    const glm::vec3& GetOffset();
    const glm::vec3& GetScale();
    const glm::vec3& GetPosition();

    // Mutators
    void SetSize(const glm::vec3& aSize);
    void SetSize(float aX, float aY, float aZ);
    void SetOffset(const glm::vec3& aOffset);
    void SetOffset(float aX, float aY, float aZ);
    void SetScale(const glm::vec3& aScale);
    void SetScale(float aX, float aY, float aZ);
    void SetPosition(const glm::vec3& aPosition);
    void SetPosition(float aX, float aY, float aZ);

  private:
    glm::vec3 mSize;      // The size ratio of the collider relative to the transform it represents
    glm::vec3 mOffset;    // The offset of the collider relative to the transform it represents

    glm::vec3 mScale;     // The "true" scale of the collider
    glm::vec3 mPosition;  // The "true" position of the collider

    CollisionStatus mCollisionStatus;

    inline void SetSizeProperty(const glm::vec3& aSize) { SetSize(aSize); };
    inline void SetOffsetProperty(const glm::vec3& aOffset) { SetOffset(aOffset); };
  };
}

#endif
