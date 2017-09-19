/******************************************************************************/
/*!
 * \author Isaac Dayton
 * \date   2015-11-20
 *
 * \copyright All content 2016 DigiPen (USA) Corporation, all rights reserved.
*/
/******************************************************************************/

#ifndef YTE_Physics_PhysicsSystem_H
#define YTE_Physics_PhysicsSystem_H

#include "Bullet/btBulletDynamicsCommon.h"

#include "YTE/Core/Component.hpp"


#include "YTE/Physics/ForwardDeclarations.hpp"

namespace YTE
{
  enum SweepType
  {
    SWEEP_CLOSEST_HIT,
    SWEEP_ALL_HITS,
    SWEEP_NUM
  };

  struct SweepOutput
  {
    std::vector<glm::vec3> hit, normal;
    std::vector<float> time;
    std::vector<Composition*> entity;
  };

  class SweepClosestHitNotMeCallback
    : public btCollisionWorld::ClosestConvexResultCallback
  {
  public:

    btCollisionObject *m_me;
    btScalar m_allowedPenetration;
    btOverlappingPairCache *m_pairCache;
    btDispatcher *m_dispatcher;

    SweepClosestHitNotMeCallback(btCollisionObject *me, const btVector3 &from,
      const btVector3 &to, btOverlappingPairCache *pairCache,
      btDispatcher *dispatcher)
      : ClosestConvexResultCallback(from, to)
      , m_me(me)
      , m_allowedPenetration(0.0f)
      , m_pairCache(pairCache)
      , m_dispatcher(dispatcher)
    {
    }

    btScalar addSingleResult(btCollisionWorld::LocalConvexResult &convexResult, bool normalInWorldSpace) override
    {
      if (convexResult.m_hitCollisionObject == m_me)
        return 1.0f;

      // ignore result if there is no contact response
      if (!convexResult.m_hitCollisionObject->hasContactResponse())
        return 1.0f;

      btVector3 linVelA, linVelB;
      linVelA = m_convexToWorld - m_convexFromWorld;
      linVelB = btVector3(0.0f, 0.0f, 0.0f); // toB.getOrigin( ) - fromB.getOrigin( );

      btVector3 relativeVelocity = linVelA - linVelB;

      // don't report time of impact for motion away from the contact normal (or cause minor penetration)
      if (convexResult.m_hitNormalLocal.dot(relativeVelocity) >= -m_allowedPenetration)
        return 1.0f;

      return ClosestConvexResultCallback::addSingleResult(convexResult, normalInWorldSpace);
    }

    bool needsCollision(btBroadphaseProxy *proxy) const override
    {
      // don't collide with itself
      if (proxy->m_clientObject == m_me)
        return false;

      return true;

      // TODO: Add this support when collision filters are a thing
      // don't do CCD when the collision filters are not matching
      //if (!ClosestConvexResultCallback::needsCollision(proxy))
      //  return false;
      //
      //btCollisionObject *otherObj = (btCollisionObject*)proxy->m_clientObject;
      //
      //if (m_dispatcher->needsResponse(m_me, otherObj))
      //  return true;
      //
      //return false;
    }
  };

  class SweepAllHitNotMeCallback
    : public btCollisionWorld::ConvexResultCallback
  {
  public:
    btCollisionObject *m_me;
    btScalar m_allowedPenetration;
    btOverlappingPairCache *m_pairCache;
    btDispatcher *m_dispatcher;

    // Used to calculate hitPointWorld from hitFraction
    btVector3 m_convexFromWorld;
    btVector3 m_convexToWorld;

    std::vector<const btCollisionObject*> m_hitCollisionObject;
    std::vector<btVector3> m_hitNormalWorld;
    std::vector<btVector3> m_hitPointWorld;
    std::vector<btScalar> m_hitFraction;

    SweepAllHitNotMeCallback(btCollisionObject *me, const btVector3 &from,
      const btVector3 &to, btOverlappingPairCache *pairCache,
      btDispatcher *dispatcher)
      : m_convexFromWorld(from)
      , m_convexToWorld(to)
      , m_me(me)
      , m_allowedPenetration(0.0f)
      , m_pairCache(pairCache)
      , m_dispatcher(dispatcher)
    {

    }

    btScalar addSingleResult(btCollisionWorld::LocalConvexResult &convexResult, bool normalInWorldSpace) override
    {
      if (convexResult.m_hitCollisionObject == m_me)
        return m_closestHitFraction;

      // ignore result if there is no contact response
      if (!convexResult.m_hitCollisionObject->hasContactResponse())
        return m_closestHitFraction;

      btVector3 linVelA, linVelB;
      linVelA = m_convexToWorld - m_convexFromWorld;
      linVelB = btVector3(0.0f, 0.0f, 0.0f); // toB.getOrigin( ) - fromB.getOrigin( );

      btVector3 relativeVelocity = linVelA - linVelB;

      // don't report time of impact for motion away from the contact normal (or cause minor penetration)
      if (convexResult.m_hitNormalLocal.dot(relativeVelocity) >= -m_allowedPenetration)
        return m_closestHitFraction;

      if (m_closestHitFraction > convexResult.m_hitFraction)
        m_closestHitFraction = convexResult.m_hitFraction;

      m_hitCollisionObject.push_back(convexResult.m_hitCollisionObject);
      m_hitFraction.push_back(convexResult.m_hitFraction);

      if (normalInWorldSpace)
      {
        m_hitNormalWorld.push_back(convexResult.m_hitNormalLocal);
      }
      else
      {
        m_hitNormalWorld.push_back(
          convexResult.m_hitCollisionObject->getWorldTransform().getBasis() *
          convexResult.m_hitNormalLocal
        );
      }

      m_hitPointWorld.push_back(convexResult.m_hitPointLocal);

      return m_closestHitFraction;
    }

    bool needsCollision(btBroadphaseProxy *proxy) const override
    {
      // don't collide with itself
      if (proxy->m_clientObject == m_me)
        return false;

      return true;

      // TODO: Add this support when collision filters are a thing
      //if (!ConvexResultCallback::needsCollision(proxy))
      //  return false;
      //
      //btCollisionObject *otherObj = (btCollisionObject*)proxy->m_clientObject;
      //
      //if (m_dispatcher->needsResponse(m_me, otherObj))
      //  return true;
      //
      //return false;
    }
  };

  struct RayCollisionInfo
  {
    YTEDeclareType(RayCollisionInfo);

    glm::vec3 mPosition;
    Composition *mObject;
    float mDistance;
    bool mCollided;
  };


  class PhysicsSystem : public Component
  {
  public:
    YTEDeclareType(PhysicsSystem);

    PhysicsSystem(Composition *aOwner, Space *aSpace, RSValue *aProperties);

    void Initialize() override;

    bool GetDebugDraw() { return mDebugDraw; };
    void ToggleDebugDraw();

    void ToggleDebugDrawOption(int aOption);

    void BeginDebugDrawUpdate(LogicUpdate *aEvent);
    void DebugDrawUpdate(LogicUpdate *aEvent);
    void EndDebugDrawUpdate(LogicUpdate *aEvent);
    void OnLogicUpdate(LogicUpdate *aEvent);


    RayCollisionInfo RayCast(glm::vec3 aPosition, glm::vec3 aDirection);


    bool Sweep(RigidBody *aBody, const glm::vec3 &aVelocity, float aDt,
                SweepOutput &aOutput, SweepType aType, bool aSorted);

    bool Sweep(CollisionBody *aBody, const glm::vec3 &aVelocity, float dt,
                SweepOutput &aOutput, SweepType type, bool aSorted);

    // TODO Ghost
    //bool PhysicsSystem::Sweep(GhostBody *aBody, const glm::vec3 &aVelocity, float dt,
    //                          SweepOutput &aOutput, SweepType type, bool aSorted);

    bool Sweep(btCollisionObject *aBody, btCollisionShape *aShape, const glm::vec3 &aVelocity,
               float aDt, SweepOutput &aOutput, SweepType type, bool aSorted);

    btDiscreteDynamicsWorld *GetWorld() { return mDynamicsWorld.get(); };
  private:

    bool SweepInternal(btCollisionObject *aBody, btCollisionShape *aShape, const glm::vec3 &aVelocity,
                        float aDt, SweepOutput &aOutput, SweepType aType, bool aSorted);

    void DispatchCollisionEvents(void);

    void DispatchContactEvent(Composition *mainObject,
                              Composition *otherObject,
                              btPersistentManifold *manifold);

    UniquePointer<btDefaultCollisionConfiguration> mCollisionConfiguration;
    UniquePointer<btCollisionDispatcher> mDispatcher;
    UniquePointer<btBroadphaseInterface> mOverlappingPairCache;
    UniquePointer<btSequentialImpulseConstraintSolver> mSolver;
    UniquePointer<btDiscreteDynamicsWorld> mDynamicsWorld;
    UniquePointer<DebugDrawer> mDebugDrawer;

    bool mDebugDraw;
  };


  inline glm::quat BtToOurQuat(const btQuaternion &aQuat)
  {
    glm::vec3 axis{ aQuat.x(), aQuat.y(), aQuat.z() };

    return glm::quat(aQuat.w(), aQuat.x(), aQuat.y(), aQuat.z());
  }

  inline btQuaternion OurQuatToBt(const glm::quat &aQuat)
  {
    return btQuaternion(aQuat.x, aQuat.y, aQuat.z, aQuat.w);
  }

  inline btVector3 OurVec3ToBt(const glm::vec3 &aVector)
  {
    return btVector3(aVector.x, aVector.y, aVector.z);
  }


  inline glm::vec3 BtToOurVec3(const btVector3 &aVector)
  {
    return glm::vec3(aVector.x(), aVector.y(), aVector.z());
  }
}

#endif