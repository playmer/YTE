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

#include "YTE/Event/StandardEvents.h"

#include "YTE/Core/Component.hpp"

#include "Bullet/btBulletDynamicsCommon.h"

namespace YTE
{
  DeclareEvent(CollisionStarted);
  DeclareEvent(CollisionPersisted);
  DeclareEvent(CollisionEnded);
  class CollisionEvent : public Event
  {
  public:
    DeclareType(CollisionEvent);
    Composition *OtherObject;
  };

  class CollisionStarted : public Event
  {
  public:
    DeclareType(CollisionStarted);
    Composition *OtherObject;
    String Name;
    Composition* Object;
  };

  class CollisionPersisted : public Event
  {
  public:
    DeclareType(CollisionPersisted);
    Composition *OtherObject;

    String Name;
    Composition* Object;
  };

  class CollisionEnded : public Event
  {
  public:
    DeclareType(CollisionEnded);
    Composition *OtherObject;

    String Name;
    Composition* Object;
  };

  enum class CollisionStatus {None, Started, Persisted};

  class Collider : public Component
  {
  public:
    DeclareType(Collider);

    Collider(Composition *aOwner, Space *aSpace);

    virtual void PhysicsInitialize() {};

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
