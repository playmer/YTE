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

#include "YTE/Graphics/GraphicsSystem.hpp"
#include "YTE/Graphics/GraphicsView.hpp"
#include "YTE/Graphics/Generics/Renderer.hpp"

#include "YTE/Physics/CollisionBody.hpp"
#include "YTE/Physics/GhostBody.hpp"
#include "YTE/Physics/DebugDraw.hpp"
#include "YTE/Physics/PhysicsSystem.hpp"
#include "YTE/Physics/RigidBody.hpp"
#include "YTE/Physics/Transform.hpp"

namespace YTE
{
  YTEDefineType(RayCollisionInfo)
  {
    RegisterType<RayCollisionInfo>();
    YTEBindField(&RayCollisionInfo::mObject, "Object", PropertyBinding::GetSet)
      .AddAttribute<EditorProperty>()
      .AddAttribute<Serializable>();
    YTEBindField(&RayCollisionInfo::mCollided, "Collided", PropertyBinding::GetSet)
      .AddAttribute<EditorProperty>()
      .AddAttribute<Serializable>();
    YTEBindField(&RayCollisionInfo::mDistance, "Distance", PropertyBinding::GetSet)
      .AddAttribute<EditorProperty>()
      .AddAttribute<Serializable>();
    YTEBindField(&RayCollisionInfo::mPosition, "Position", PropertyBinding::GetSet)
      .AddAttribute<EditorProperty>()
      .AddAttribute<Serializable>();
  }

  YTEDefineType(PhysicsSystem)
  {
    RegisterType<PhysicsSystem>();

    GetStaticType()->AddAttribute<RunInEditor>();

    std::vector<std::vector<Type*>> deps = { { TypeId<GraphicsView>() } };

    GetStaticType()->AddAttribute<ComponentDependencies>(deps);

    YTEBindFunction(&PhysicsSystem::ToggleDebugDrawOption, YTENoOverload, "ToggleDebugDrawOption", YTEParameterNames("aOption"));
    YTEBindFunction(&PhysicsSystem::ToggleDebugDraw, YTENoOverload, "ToggleDebugDraw", YTENoNames);
    YTEBindFunction(&PhysicsSystem::RayCast, YTENoOverload, "RayCast", YTEParameterNames("aPosition", "aDirection"));
    YTEBindProperty(&PhysicsSystem::GetGravity, &PhysicsSystem::SetGravity, "Gravity")
      .AddAttribute<EditorProperty>()
      .AddAttribute<Serializable>();
  }

  PhysicsSystem::PhysicsSystem(Composition *aOwner, Space *aSpace, RSValue *aProperties)
    : Component(aOwner, aSpace)
    , mDebugDraw(false)
  {
    YTEUnusedArgument(aProperties);

    mSpace->RegisterEvent<&PhysicsSystem::OnPhysicsUpdate>(Events::PhysicsUpdate, this);

      // collision configuration contains default setup for memory , collision setup . Advanced
      // users can create their own configuration .
    mCollisionConfiguration = std::make_unique<btDefaultCollisionConfiguration>();
      
      // use the default collision dispatcher . For parallel processing you can use a different
      //  dispatcher(see Extras / BulletMultiThreaded)
    mDispatcher = std::make_unique<btCollisionDispatcher>(mCollisionConfiguration.get());
      
      // btDbvtBroadphase is a good general purpose broad phase . You can also try out
      // btAxis3Sweep .
    mOverlappingPairCache = std::make_unique<btDbvtBroadphase>();
       
      // the default constraint solver . For parallel processing you can use a different solver
      //   (see Extras / BulletMultiThreaded)
    mSolver = std::make_unique<btSequentialImpulseConstraintSolver>();
      
    mDynamicsWorld = std::make_unique<btDiscreteDynamicsWorld>(mDispatcher.get(),
                                                               mOverlappingPairCache.get(),
                                                               mSolver.get(),
                                                               mCollisionConfiguration.get());
      
    mDynamicsWorld->setGravity(OurVec3ToBt(mGravityAcceleration));
  }


  PhysicsSystem::~PhysicsSystem()
  {

  }


  void PhysicsSystem::Initialize()
  {
    auto renderer = mOwner->GetEngine()->GetComponent<GraphicsSystem>()->GetRenderer();
    mDebugDrawer = std::make_unique<DebugDrawer>(mOwner->GetGUID().ToString(),
                                                 renderer,
                                                 mOwner->GetComponent<GraphicsView>());
    mDynamicsWorld->setDebugDrawer(mDebugDrawer.get());
  }

  void PhysicsSystem::ToggleDebugDraw()
  {
    mDebugDraw = !mDebugDraw;

    std::cout << std::boolalpha << mDebugDraw << "\n";

      // Register for events.
    if (mDebugDraw)
    {
      mOwner->GetEngine()->RegisterEvent<&PhysicsSystem::DebugDrawUpdate>(Events::PreFrameUpdate, this);
    }
    else
    {
      mOwner->GetEngine()->DeregisterEvent<&PhysicsSystem::DebugDrawUpdate>(Events::PreFrameUpdate,  this);
      mDebugDrawer->clearLines();
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
    YTEUnusedArgument(aEvent);
  }

  void PhysicsSystem::DebugDrawUpdate(LogicUpdate *aEvent)
  {
    YTEUnusedArgument(aEvent);
    mDebugDrawer->Begin();
    mDynamicsWorld->debugDrawWorld();
    mDebugDrawer->End();
  }

  void PhysicsSystem::EndDebugDrawUpdate(LogicUpdate *aEvent)
  {
    YTEUnusedArgument(aEvent);
  }

  void PhysicsSystem::OnPhysicsUpdate(LogicUpdate *aEvent)
  {
    YTEUnusedArgument(aEvent);
    YTEProfileFunction();

    mDynamicsWorld->updateAabbs();
    mDynamicsWorld->stepSimulation(static_cast<float>(aEvent->Dt), 10);

    DispatchCollisionEvents();
  }

  void PhysicsSystem::DispatchCollisionEvents(void)
  {      
    auto numManifolds = mDispatcher->getNumManifolds();
    for (int i = 0; i < numManifolds; ++i)
    {
      auto *manifold = mDispatcher->getManifoldByIndexInternal(i);
      
      auto *objA = manifold->getBody0();
      auto *objB = manifold->getBody1();
      
      // Get each body's composition pointer
      Composition *compositionA = static_cast<Composition*>(objA->getUserPointer());
      Composition *compositionB = static_cast<Composition*>(objB->getUserPointer());

      int numContacts = manifold->getNumContacts();
      for (int j = 0; j < numContacts; j++)
      {
        btManifoldPoint& pt = manifold->getContactPoint(j);

        // The objects have collided.
        if (pt.getDistance() < 0.f)
        {
          // Process the event for Entity A
          DispatchContactEvent(compositionA, compositionB, manifold);

          // Process the event for Entity B
          DispatchContactEvent(compositionB, compositionA, manifold);
        }
      }
    }
  }

  void PhysicsSystem::DispatchContactEvent(Composition *aMainObject, Composition *aOtherObject, btPersistentManifold *aManifold)
  {
    YTEUnusedArgument(aManifold);

    if (aMainObject)
    {
      Body *body = aMainObject->GetComponent<RigidBody>();

      if (body == nullptr) body = aMainObject->GetComponent<GhostBody>();
      if (body == nullptr) body = aMainObject->GetComponent<CollisionBody>();
        
      if (body)
      {
        body->AddCollidedThisFrame(aOtherObject);
      }
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


      
      info.mDistance = glm::length(BtToOurVec3(rayCallback.m_hitPointWorld) - aPosition);
    }

    return info;
  }

  glm::vec3 PhysicsSystem::GetGravity()
  {
    return mGravityAcceleration;
  }

  void PhysicsSystem::SetGravity(glm::vec3 aAcceleration)
  {
    mGravityAcceleration = aAcceleration;
    btVector3 accel = OurVec3ToBt(aAcceleration);
    mDynamicsWorld->setGravity(accel);
  };

} // namespace YTE