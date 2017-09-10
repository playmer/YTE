/******************************************************************************/
/*!
 * \author Isaac Dayton
 * \date   2015-11-27
 *
 * \copyright All content 2016 DigiPen (USA) Corporation, all rights reserved.
*/
/******************************************************************************/
#include "YTE/Core/Composition.hpp"
#include "YTE/Core/Engine.hpp"
#include "YTE/Core/Space.hpp"

#include "YTE/Physics/Transform.h"
#include "YTE/Physics/MenuCollider.h"

namespace YTE
{
  DefineType(MenuCollider)
  {
    YTERegisterType(MenuCollider);
    YTEBindProperty(&MenuCollider::GetSize, &MenuCollider::SetSizeProperty, "Size")->AddAttribute<EditorProperty>();
    YTEBindProperty(&MenuCollider::GetOffset, &MenuCollider::SetOffsetProperty, "Offset")->AddAttribute<EditorProperty>();

    YTEAddFunction( &MenuCollider::SetSize, (void (MenuCollider::*) (const glm::vec3&)), "SetSize", YTEParameterNames("size"))
      ->Description() = "Sets the size of the box collider from a Real3";
    YTEAddFunction( &MenuCollider::SetSize, (void (MenuCollider::*) (float, float, float)), "SetSize", YTEParameterNames("x", "y", "z"))
      ->Description() = "Sets the size of the box collider from three Reals X, Y, and Z";

    YTEAddFunction( &MenuCollider::SetOffset, (void (MenuCollider::*) (const glm::vec3&)), "SetOffset", YTEParameterNames("offset"))
      ->Description() = "Sets the position offset of the box collider from a Real3 of x, y, and z coordinates";
    YTEAddFunction( &MenuCollider::SetOffset, (void (MenuCollider::*) (float, float, float)), "SetOffset", YTEParameterNames("x", "y", "z"))
      ->Description() = "Sets the position offset of the box collider from three Reals X, Y, and Z";
  }

  MenuCollider::MenuCollider(Composition *aOwner, Space *aSpace, RSValue *aProperties)
    : Collider(aOwner, aSpace)
  {
    mSize = glm::vec3(1, 1, 1);
    mOffset = glm::vec3(0, 0, 0);

    DeserializeByType<MenuCollider*>(aProperties, this, MenuCollider::GetStaticType());
  }

  void MenuCollider::Initialize()
  {
    Transform* myTransform = mOwner->GetComponent<Transform>();

    mCollisionStatus = CollisionStatus::None;
    mPosition = myTransform->GetTranslation() + mOffset;
    mScale = glm::vec3(myTransform->GetScale().x * mSize.x, myTransform->GetScale().y * mSize.y, myTransform->GetScale().z * mSize.z);

    mOwner->YTERegister(Events::PositionChanged, this, &MenuCollider::OnPositionChanged);
    mOwner->YTERegister(Events::ScaleChanged, this, &MenuCollider::OnScaleChanged);
  }

  void MenuCollider::OnPositionChanged(PositionChanged *aEvent)
  {
    mPosition = aEvent->Position + mOffset;
  }

  void MenuCollider::OnScaleChanged(ScaleChanged* aEvent)
  {
    glm::vec3 scale = aEvent->Scale;

    mScale = glm::vec3(scale.x * mSize.x, scale.y * mSize.y, scale.z * mSize.z);
  }

  void MenuCollider::PointToBox2D(const glm::vec2& aPoint)
  {
      // If the point is within the box, a collision is happening
    if (aPoint.x <= (mPosition.x + 0.5 * mScale.x) && aPoint.x >= (mPosition.x - 0.5 * mScale.x) &&
        aPoint.y <= (mPosition.y + 0.5 * mScale.y) && aPoint.y >= (mPosition.y - 0.5 * mScale.y))
    {
      if (mCollisionStatus == CollisionStatus::None)
      {
          // Send CollisionStarted event
        CollisionStarted collisionStarted;
        mOwner->SendEvent(Events::CollisionStarted, &collisionStarted);

          // Set CollisionStatus to started
        mCollisionStatus = CollisionStatus::Started;
      }

      else
      {
          // Send CollisionPersisted event
        CollisionPersisted collisionPersist;
        mOwner->SendEvent(Events::CollisionPersisted, &collisionPersist);

          // Set CollisionStatus to persisted
        mCollisionStatus = CollisionStatus::Persisted;
      }
    }

      // The point is not in the box, thus there is no longer a collision happening
    else
    {
      if (mCollisionStatus != CollisionStatus::None)
      {
          // Send CollisionEnded event
        CollisionEnded collisionEnd;
        mOwner->SendEvent(Events::CollisionEnded, &collisionEnd);

          // Set collisionStatus to none
        mCollisionStatus = CollisionStatus::None;
      }
    }
  }

  void MenuCollider::PointToBox2D(const glm::vec2& aBox2D, const glm::vec2& aPoint)
  {
    // If the point is within the box, a collision is happening
    if (aPoint.x <= (mPosition.x + 0.5 * aBox2D.x) && aPoint.x >= (mPosition.x - 0.5 * aBox2D.x) &&
      aPoint.y <= (mPosition.y + 0.5 * aBox2D.y) && aPoint.y >= (mPosition.y - 0.5 * aBox2D.y))
    {
      if (mCollisionStatus == CollisionStatus::None)
      {
        // Send CollisionStarted event
        CollisionStarted collisionStarted;
        mOwner->SendEvent(Events::CollisionStarted, &collisionStarted);

        // Set CollisionStatus to started
        mCollisionStatus = CollisionStatus::Started;
      }

      else
      {
        // Send CollisionPersisted event
        CollisionPersisted collisionPersist;
        mOwner->SendEvent(Events::CollisionPersisted, &collisionPersist);

        // Set CollisionStatus to persisted
        mCollisionStatus = CollisionStatus::Persisted;
      }
    }

    // The point is not in the box, thus there is no longer a collision happening
    else
    {
      if (mCollisionStatus != CollisionStatus::None)
      {
        // Send CollisionEnded event
        CollisionEnded collisionEnd;
        mOwner->SendEvent(Events::CollisionEnded, &collisionEnd);

        // Set collisionStatus to none
        mCollisionStatus = CollisionStatus::None;
      }
    }
  }

  // GETTERS / SETTERS //////////////////////////////////////////////////////////////////////////
  const glm::vec3& MenuCollider::GetSize()
  {
    return mSize;
  }

  const glm::vec3& MenuCollider::GetOffset()
  {
    return mOffset;
  }

  const glm::vec3& MenuCollider::GetScale()
  {
    return mScale;
  }

  const glm::vec3& MenuCollider::GetPosition()
  {
    return mPosition;
  }

  void MenuCollider::SetSize(const glm::vec3& aSize)
  {
    mSize = aSize;

    SetScale(mScale.x * aSize.x, mScale.y * aSize.y, mScale.z * aSize.z);
  }

  void MenuCollider::SetSize(float aX, float aY, float aZ)
  {
    mSize = glm::vec3(aX, aY, aZ);

    SetScale(mScale.x * aX, mScale.y * aY, mScale.z * aZ);
  }

  void MenuCollider::SetOffset(const glm::vec3& aOffset)
  {
    mOffset = aOffset;

    SetPosition(mPosition + aOffset);
  }

  void MenuCollider::SetOffset(float aX, float aY, float aZ)
  {
    mOffset = glm::vec3(aX, aY, aZ);

    SetPosition(mPosition + mOffset);
  }

  void MenuCollider::SetScale(const glm::vec3& aScale)
  {
    mScale = aScale;
  }

  void MenuCollider::SetScale(float aX, float aY, float aZ)
  {
    mScale = glm::vec3(aX, aY, aZ);
  }

  void MenuCollider::SetPosition(const glm::vec3& aPosition)
  {
    mPosition= aPosition;
  }

  void MenuCollider::SetPosition(float aX, float aY, float aZ)
  {
    mPosition = glm::vec3(aX, aY, aZ);
  }
}
