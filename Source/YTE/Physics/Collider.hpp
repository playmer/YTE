/******************************************************************************/
/*!
 * \author Isaac Dayton
 * \date   2015-11-27
 *
 * \copyright All content 2016 DigiPen (USA) Corporation, all rights reserved.
*/
/******************************************************************************/
#pragma once

#ifndef YTE_Physics_Collider_h
#define YTE_Physics_Collider_h

#include "YTE/Core/Component.hpp"

#include "btBulletDynamicsCommon.h"

namespace YTE
{
  YTEDeclareEvent(CollisionStarted);
  YTEDeclareEvent(CollisionPersisted);
  YTEDeclareEvent(CollisionEnded);
  class CollisionEvent : public Event
  {
  public:
    YTEDeclareType(CollisionEvent);

    CollisionEvent()
      : OtherObject(nullptr)
    {
    }

    Composition *OtherObject;
  };

  class CollisionStarted : public Event
  {
  public:
    YTEDeclareType(CollisionStarted);

    CollisionStarted()
      : OtherObject(nullptr)
      , Object(nullptr)
    {
    }

    Composition *OtherObject;
    String Name;
    Composition* Object;
  };

  class CollisionPersisted : public Event
  {
  public:
    YTEDeclareType(CollisionPersisted);

    CollisionPersisted() 
      : OtherObject(nullptr)
      , Object(nullptr)
    {
    }

    Composition *OtherObject;

    String Name;
    Composition* Object;
  };

  class CollisionEnded : public Event
  {
  public:
    YTEDeclareType(CollisionEnded);

    CollisionEnded()
      : OtherObject(nullptr)
      , Object(nullptr)
    {
    }

    Composition *OtherObject;

    String Name;
    Composition* Object;
  };

  enum class CollisionStatus {None, Started, Persisted};

  class Collider : public Component
  {
  public:
    YTEDeclareType(Collider);

    Collider(Composition *aOwner, Space *aSpace);

    virtual void PhysicsInitialize() override { };

    glm::vec3 GetTranslation() const;
      
    glm::quat GetRotation() const;

    void SetTranslation(float aX, float aY, float aZ);
    void SetRotation(const glm::quat& aRotation);

    btCollisionObject* GetCollider() { return mCollider.get(); }
  protected:
    UniquePointer<btCollisionObject> mCollider;

    glm::vec3 mVelocity;

    bool mAffectedByGravity;
  };

  Collider* GetColliderFromObject(Composition* aOwner);
}

#endif
