/******************************************************************************/
/*!
 * \author Joshua T. Fisher
 * \date   2015-11-20
 *
 * \copyright All content 2016 DigiPen (USA) Corporation, all rights reserved.
*/
/******************************************************************************/

#include <algorithm>

#include "YTE/Core/Composition.hpp"
#include "YTE/Core/Engine.hpp"
#include "YTE/Core/Space.hpp"

#include "YTE/Physics/CollisionBody.h"
#include "YTE/Physics/GhostBody.h"
#include "YTE/Physics/DebugDraw.h"
#include "YTE/Physics/PhysicsSystem.h"
#include "YTE/Physics/RigidBody.h"
#include "YTE/Physics/Transform.h"

namespace YTE
{
  DefineType(PhysicsSystem)
  {
    YTERegisterType(PhysicsSystem);
    YTEAddFunction( &PhysicsSystem::ToggleDebugDrawOption, YTENoOverload, "ToggleDebugDrawOption", YTEParameterNames("aOption"));
    YTEAddFunction( &PhysicsSystem::ToggleDebugDraw, YTENoOverload, "ToggleDebugDraw", YTENoNames);
    YTEAddFunction( &PhysicsSystem::RayCast, YTENoOverload, "RayCast", YTEParameterNames("aPosition", "aDirection"));
  }

  PhysicsSystem::PhysicsSystem(Composition *aOwner, Space *aSpace, RSValue *aProperties)
    : Component(aOwner, aSpace)
  {
    mSpace->YTERegister(Events::LogicUpdate, this, &PhysicsSystem::OnLogicUpdate);


      // collision configuration contains default setup for memory , collision setup . Advanced
      // users can create their own configuration .
    mCollisionConfiguration = std::make_unique<btDefaultCollisionConfiguration>();
      
      // use the default collision dispatcher . For parallel processing you can use a diffent
      //  dispatcher(see Extras / BulletMultiThreaded)
    mDispatcher = std::make_unique<btCollisionDispatcher>(mCollisionConfiguration.get());
      
      // btDbvtBroadphase is a good general purpose broadphase . You can also try out
      // btAxis3Sweep .
    mOverlappingPairCache = std::make_unique<btDbvtBroadphase>();
       
      // the default constraint solver . For parallel processing you can use a different solver
      //   (see Extras / BulletMultiThreaded)
    mSolver = std::make_unique<btSequentialImpulseConstraintSolver>();
      
    mDynamicsWorld = std::make_unique<btDiscreteDynamicsWorld>(mDispatcher.get(),
                                                                mOverlappingPairCache.get(),
                                                                mSolver.get(),
                                                                mCollisionConfiguration.get());
      
    mDynamicsWorld->setGravity(btVector3(0, -10, 0));
  }


  void PhysicsSystem::Initialize()
  {
    mDebugDrawer = std::make_unique<DebugDrawer>();
    mDynamicsWorld->setDebugDrawer(mDebugDrawer.get());
  }

  void PhysicsSystem::ToggleDebugDraw()
  {
    mDebugDraw = !mDebugDraw;

      // Register for events.
    if (mDebugDraw == false)
    {

      //TODO (Josh): Reimplement the debug drawing functionality.
      //auto system = mOwner->GetUniverse()->GetComponent<Graphics::GraphicsSystem>();
      //system->YTERegister(Events::BeginDebugDrawUpdate, this, &PhysicsSystem::BeginDebugDrawUpdate);
      //system->YTERegister(Events::DebugDrawUpdate, this, &PhysicsSystem::DebugDrawUpdate);
      //system->YTERegister(Events::EndDebugDrawUpdate, this, &PhysicsSystem::EndDebugDrawUpdate);

      auto debugMode = mDebugDrawer->getDebugMode();
      debugMode |= 0 << DebugDrawer::DBG_NoDebug;
      mDebugDrawer->setDebugMode(debugMode);
    }
    else
    {
      //TODO (Josh): Reimplement the debug drawing functionality.
      //auto system = mOwner->GetUniverse()->GetComponent<Graphics::GraphicsSystem>();
      //system->YTEDeregister(Events::BeginDebugDrawUpdate, this, &PhysicsSystem::BeginDebugDrawUpdate);
      //system->YTEDeregister(Events::DebugDrawUpdate, this, &PhysicsSystem::DebugDrawUpdate);
      //system->YTEDeregister(Events::EndDebugDrawUpdate, this, &PhysicsSystem::EndDebugDrawUpdate);

      auto debugMode = mDebugDrawer->getDebugMode();
      debugMode &= ~(1 << DebugDrawer::DBG_NoDebug);
      mDebugDrawer->setDebugMode(debugMode);
    }
  }

  void PhysicsSystem::ToggleDebugDrawOption(int aOption)
  {
    auto debugMode = mDebugDrawer->getDebugMode();
    debugMode ^= 1 << aOption;
    mDebugDrawer->setDebugMode(debugMode);
  }

  void PhysicsSystem::BeginDebugDrawUpdate(LogicUpdate *aEvent)
  {
    mDebugDrawer->Begin();
  }

  void PhysicsSystem::DebugDrawUpdate(LogicUpdate *aEvent)
  {
    mDynamicsWorld->debugDrawWorld();
  }

  void PhysicsSystem::EndDebugDrawUpdate(LogicUpdate *aEvent)
  {
    mDebugDrawer->End();
  }

  void PhysicsSystem::OnLogicUpdate(LogicUpdate *aEvent)
  {
    mDynamicsWorld->stepSimulation(aEvent->Dt, 10);

    DispatchCollisionEvents();
  }

  void PhysicsSystem::DispatchCollisionEvents(void)
  {
    auto *dispatcher = mDynamicsWorld->getDispatcher( );
    int numManifolds = dispatcher->getNumManifolds( );
      
    for (int i = 0; i < numManifolds; ++i)
    {
      auto *manifold = dispatcher->getManifoldByIndexInternal( i );
      
      auto *objA = manifold->getBody0( );
      auto *objB = manifold->getBody1( );
      
      // Get each body's composition pointer
      Composition *compA = static_cast<Composition*>(objA->getUserPointer()),
                        *compB = static_cast<Composition*>(objB->getUserPointer());
        
      // Process the event for Entity A
      DispatchContactEvent(compA, compB, manifold );
      
      // Process the event for Entity B
      DispatchContactEvent(compB, compA, manifold );
    }
  }

  void PhysicsSystem::DispatchContactEvent(Composition *mainObject, Composition *otherObject, btPersistentManifold *manifold)
  {
    Body *body = mainObject->GetComponent<RigidBody>();

    if (body == nullptr) body = mainObject->GetComponent<GhostBody>();
    if (body == nullptr) body = mainObject->GetComponent<CollisionBody>();
      
    if (body)
    {
      body->AddCollidedThisFrame(otherObject);
    }
  }


  RayCollisionInfo PhysicsSystem::RayCast(glm::vec3 aPosition, glm::vec3 aDirection)
  {
    auto start = OurVec3ToBt(aPosition);
    auto end = OurVec3ToBt(aDirection);
    btCollisionWorld::ClosestRayResultCallback rayCallback(start, end);

    mDynamicsWorld->rayTest(start, end, rayCallback);

    RayCollisionInfo info;
    info.mCollided = rayCallback.hasHit();

    if (info.mCollided)
    {
      info.mObject = static_cast<Composition*>(rayCallback.m_collisionObject->getUserPointer());
      info.mDistance = (BtToOurVec3(rayCallback.m_hitPointWorld) - aPosition).length();
    }

    return info;
  }

  DefineType(RayCollisionInfo)
  {
    YTERegisterType(RayCollisionInfo);
    YTEBindField(&RayCollisionInfo::mObject, "Object", PropertyBinding::GetSet)->AddAttribute<EditorProperty>();
    YTEBindField(&RayCollisionInfo::mCollided, "Collided", PropertyBinding::GetSet)->AddAttribute<EditorProperty>();
    YTEBindField(&RayCollisionInfo::mDistance, "Distance", PropertyBinding::GetSet)->AddAttribute<EditorProperty>();
    YTEBindField(&RayCollisionInfo::mPosition, "Position", PropertyBinding::GetSet)->AddAttribute<EditorProperty>();
  }
} // namespace YTE