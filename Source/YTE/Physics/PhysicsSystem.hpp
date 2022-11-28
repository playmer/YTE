#ifndef YTE_Physics_PhysicsSystem_hpp
#define YTE_Physics_PhysicsSystem_hpp

#include "btBulletDynamicsCommon.h"

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

    YTE_Shared PhysicsSystem(Composition *aOwner, Space *aSpace);
    YTE_Shared ~PhysicsSystem();

    YTE_Shared void Initialize() override;

    bool GetDebugDraw() { return mDebugDraw; };
    YTE_Shared void ToggleDebugDraw();

    YTE_Shared void ToggleDebugDrawOption(int aOption);

    YTE_Shared void BeginDebugDrawUpdate(LogicUpdate *aEvent);
    YTE_Shared void DebugDrawUpdate(LogicUpdate *aEvent);
    YTE_Shared void EndDebugDrawUpdate(LogicUpdate *aEvent);
    YTE_Shared void OnPhysicsUpdate(LogicUpdate *aEvent);

    YTE_Shared RayCollisionInfo RayCast(glm::vec3 aPosition, glm::vec3 aDirection);

    btDiscreteDynamicsWorld *GetWorld() { return mDynamicsWorld.get(); };

    YTE_Shared glm::vec3 GetGravity();
    YTE_Shared void SetGravity(glm::vec3 aAcceleration);

  private:
    YTE_Shared void DispatchCollisionEvents(void);

    YTE_Shared void DispatchContactEvent(Composition *mainObject,
                                Composition *otherObject,
                                btPersistentManifold *manifold);

    std::unique_ptr<btDefaultCollisionConfiguration> mCollisionConfiguration;
    std::unique_ptr<btCollisionDispatcher> mDispatcher;
    std::unique_ptr<btBroadphaseInterface> mOverlappingPairCache;
    std::unique_ptr<btSequentialImpulseConstraintSolver> mSolver;
    std::unique_ptr<btDiscreteDynamicsWorld> mDynamicsWorld;
    std::unique_ptr<DebugDrawer> mDebugDrawer;

    bool mDebugDraw;
    glm::vec3 mGravityAcceleration;
  };


  inline glm::quat ToGlm(const btQuaternion &aQuat)
  {
    glm::vec3 axis{ aQuat.x(), aQuat.y(), aQuat.z() };

    return glm::quat(aQuat.w(), aQuat.x(), aQuat.y(), aQuat.z());
  }

  inline btQuaternion ToBullet(const glm::quat &aQuat)
  {
    return btQuaternion(aQuat.x, aQuat.y, aQuat.z, aQuat.w);
  }

  inline btVector3 ToBullet(const glm::vec3 &aVector)
  {
    return btVector3(aVector.x, aVector.y, aVector.z);
  }


  inline glm::vec3 ToGlm(const btVector3 &aVector)
  {
    return glm::vec3(aVector.x(), aVector.y(), aVector.z());
  }
}

#endif