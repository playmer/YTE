#pragma once

#include "Bullet/btBulletDynamicsCommon.h"

#include "YTE/Core/ForwardDeclarations.hpp"
#include "YTE/Core/Composition.hpp"
#include "YTE/Core/Utilities.hpp"

#include "YTE/Graphics/GraphicsView.hpp"
#include "YTE/Graphics/Model.hpp"

#include "YTE/Physics/Transform.hpp"
#include "YTE/Physics/RigidBody.hpp"

#include "YTE/Platform/Mouse.hpp"
#include "YTE/Platform/ForwardDeclarations.hpp"

namespace YTEditor
{

  class MainWindow;

  struct PickerObject : public YTE::EventHandler
  {
    void ChangedPositionAndRotation(YTE::TransformChanged *aEvent);
    void ChangedScale(YTE::TransformChanged *aEvent);

    btCollisionShape *mShape;
    std::unique_ptr<btGhostObject> mGhostBody;
    std::unique_ptr<YTE::MotionState> mMotionState;
    std::unique_ptr<btBoxShape> mBoxShape;
    std::unique_ptr<btBvhTriangleMeshShape> mTriangleMeshShape;
    btTriangleMesh mTriangles;
  };

  class PhysicsHandler : public YTE::EventHandler
  {
  public:
    PhysicsHandler(YTE::Space *aSpace, YTE::Window *aWindow, MainWindow *aMainWindow);

    void Add(YTE::Composition *aComposition);
    void Remove(YTE::Composition *aComposition);

    void Update();

    void OnMousePress(YTE::MouseButtonEvent *aEvent);
    void OnMousePersist(YTE::MouseButtonEvent *aEvent);
    void OnMouseRelease(YTE::MouseButtonEvent *aEvent);

    void AddedComposition(YTE::CompositionAdded *aEvent);
    void RemovedComposition(YTE::CompositionRemoved *aEvent);

    void OnModelChanged(YTE::ModelChanged *aEvent);

  private:
    YTE::UniquePointer<btDefaultCollisionConfiguration> mCollisionConfiguration;
    YTE::UniquePointer<btCollisionDispatcher> mDispatcher;
    YTE::UniquePointer<btBroadphaseInterface> mOverlappingPairCache;
    YTE::UniquePointer<btSequentialImpulseConstraintSolver> mSolver;
    YTE::UniquePointer<btDiscreteDynamicsWorld> mDynamicsWorld;

    std::unordered_map<YTE::Composition*, std::unique_ptr<PickerObject>> mObjects;

    YTE::Space *mSpace;
    YTE::Window *mWindow;
    MainWindow *mMainWindow;

    bool mIsHittingObject;
    bool mIsGizmoActive;
    YTE::Composition *mCurrentObj;
    YTE::Composition *mPickedObj;
    float mPickedDistance;
  };

  btVector3 getRayTo(YTE::UBOView& aView,
    btVector3& aRayFrom,
    glm::i32vec2 aMouseCoordinates,
    YTE::u32 aWidth,
    YTE::u32 aHeight,
    float aFar);

}
