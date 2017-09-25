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
#include "YTE/Physics/DebugDraw.hpp"

namespace YTE
{
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
    ~PhysicsSystem();

    void Initialize() override;

    bool GetDebugDraw() { return mDebugDraw; };
    void ToggleDebugDraw();

    void ToggleDebugDrawOption(int aOption);

    void BeginDebugDrawUpdate(LogicUpdate *aEvent);
    void DebugDrawUpdate(LogicUpdate *aEvent);
    void EndDebugDrawUpdate(LogicUpdate *aEvent);
    void OnLogicUpdate(LogicUpdate *aEvent);

    RayCollisionInfo RayCast(glm::vec3 aPosition, glm::vec3 aDirection);

    btDiscreteDynamicsWorld *GetWorld() { return mDynamicsWorld.get(); };
  private:
    void DispatchCollisionEvents(void);

    void DispatchContactEvent(Composition *mainObject,
                              Composition *otherObject,
                              btPersistentManifold *manifold);

    UniquePointer<btDefaultCollisionConfiguration> mCollisionConfiguration;
    UniquePointer<btCollisionDispatcher> mDispatcher;
    UniquePointer<btBroadphaseInterface> mOverlappingPairCache;
    UniquePointer<btSequentialImpulseConstraintSolver> mSolver;
    UniquePointer<btDiscreteDynamicsWorld> mDynamicsWorld;
    UniquePointer<YTE::DebugDrawer> mDebugDrawer;

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