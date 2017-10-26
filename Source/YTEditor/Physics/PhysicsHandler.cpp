#include "Bullet/btBulletCollisionCommon.h"
#include "Bullet/BulletCollision/CollisionDispatch/btGhostObject.h"

#include "YTE/Core/Space.hpp"
#include "YTE/Graphics/Camera.hpp"
#include "YTE/Graphics/GraphicsView.hpp"
#include "YTE/Graphics/Mesh.hpp"
#include "YTE/Graphics/Model.hpp"
#include "YTE/Physics/Orientation.hpp"
#include "YTE/Physics/PhysicsSystem.hpp"
#include "YTE/Physics/Transform.hpp"

#include "YTEditor/Gizmos/Gizmo.hpp"
#include "YTEditor/Gizmos/Axis.hpp"
#include "YTEditor/Gizmos/Translate.hpp"
#include "YTEditor/Gizmos/Scale.hpp"
#include "YTEditor/Gizmos/Rotate.hpp"
#include "YTEditor/MainWindow/MainWindow.hpp"
#include "YTEditor/ObjectBrowser/ObjectBrowser.hpp"
#include "YTEditor/Physics/PhysicsHandler.hpp"


namespace YTEditor
{

  void PickerObject::ChangedPositionAndRotation(YTE::TransformChanged *aEvent)
  {
    btTransform transform;
    transform.setOrigin(YTE::OurVec3ToBt(aEvent->WorldPosition));
    transform.setRotation(YTE::OurQuatToBt(aEvent->WorldRotation));

    mGhostBody->setWorldTransform(transform);
  }

  void PickerObject::ChangedScale(YTE::TransformChanged *aEvent)
  {

    btVector3 scale{ YTE::OurVec3ToBt(aEvent->WorldScale) };
    if (nullptr != mTriangleMeshShape)
    {
      mTriangleMeshShape->setLocalScaling(scale);
    }
    else if (nullptr != mBoxShape)
    {
      mBoxShape->setLocalScaling(scale / 2.0f);
    }
  }

  PhysicsHandler::PhysicsHandler(YTE::Space *aSpace, YTE::Window *aWindow, MainWindow *aMainWindow)
    : mSpace(aSpace)
    , mWindow(aWindow)
    , mMainWindow(aMainWindow)
    , mIsHittingObject(false)
    , mIsGizmoActive(false)
    , mCurrentObj(nullptr)
    , mPrevMousePos(0, 0, 0, 1)
  {
    // collision configuration contains default setup for memory , collision setup .
    // Advanced users can create their own configuration .
    mCollisionConfiguration = std::make_unique<btDefaultCollisionConfiguration>();

    // use the default collision dispatcher . For parallel processing you can use a different
    // dispatcher(see Extras / BulletMultiThreaded)
    mDispatcher = std::make_unique<btCollisionDispatcher>(mCollisionConfiguration.get());

    // btDbvtBroadphase is a good general purpose broad phase. You can also try out
    // btAxis3Sweep.
    mOverlappingPairCache = std::make_unique<btDbvtBroadphase>();

    // the default constraint solver . For parallel processing you can use a different
    // solver (see Extras / BulletMultiThreaded)
    mSolver = std::make_unique<btSequentialImpulseConstraintSolver>();

    mDynamicsWorld = std::make_unique<btDiscreteDynamicsWorld>(mDispatcher.get(),
      mOverlappingPairCache.get(),
      mSolver.get(),
      mCollisionConfiguration.get());

    aSpace->YTERegister(YTE::Events::CompositionAdded, this, &PhysicsHandler::AddedComposition);
    aSpace->YTERegister(YTE::Events::CompositionRemoved, this, &PhysicsHandler::RemovedComposition);
    aWindow->mMouse.YTERegister(YTE::Events::MousePress, this, &PhysicsHandler::OnMousePress);
    aWindow->mMouse.YTERegister(YTE::Events::MousePersist, this, &PhysicsHandler::OnMousePersist);
    aWindow->mMouse.YTERegister(YTE::Events::MouseRelease, this, &PhysicsHandler::OnMouseRelease);
  }


  btVector3 getRayTo(YTE::UBOView& aView,
    btVector3& aRayFrom,
    glm::i32vec2 aMouseCoordinates,
    YTE::u32 aWidth,
    YTE::u32 aHeight,
    float aFar)
  {
    YTE::UBOView& uboView = aView;

    glm::vec2 mouse = aMouseCoordinates;
    glm::vec2 screen(aWidth, aHeight);

    glm::vec4 rayFromNDC(
      0,
      0,
      -1.0f,
      1.0f
    );

    glm::vec4 rayToNDC(
      (mouse.x / screen.x - 0.5f) * 2.0f,
      (mouse.y / screen.y - 0.5f) * 2.0f,
      1.0f,
      1.0f
    );

    glm::mat4 invMat = glm::inverse(uboView.mProjectionMatrix * uboView.mViewMatrix);
    glm::vec4 rayFromWorld = invMat * rayFromNDC;
    rayFromWorld /= rayFromWorld.w;
    glm::vec4 rayToWorld = invMat * rayToNDC;
    rayToWorld /= rayToWorld.w;

    glm::vec3 rayDirectionWorld(rayToWorld - rayFromWorld);
    rayDirectionWorld = glm::normalize(rayDirectionWorld);

    // print out mouse click world coordinates
    //auto mc = BtToOurVec3(aRayFrom);
    //std::cout << "M Coords: (" << mc.x << "," << mc.y << "," << mc.z << ")" << std::endl;

    return aRayFrom + YTE::OurVec3ToBt(rayDirectionWorld) * aFar;
  }

  void PhysicsHandler::OnMousePress(YTE::MouseButtonEvent *aEvent)
  {
    if (aEvent->Button != YTE::Mouse_Buttons::Left)
    {
      return;
    }

    static size_t i = 0;

    auto view = mSpace->GetComponent<YTE::GraphicsView>();
    auto camera = view->GetLastCamera();
    auto owner = camera->GetOwner();
    auto transform = owner->GetComponent<YTE::Transform>();
    auto orientation = owner->GetComponent<YTE::Orientation>();
    auto rayFrom = YTE::OurVec3ToBt(transform->GetTranslation());
    auto cameraUp = YTE::OurVec3ToBt(orientation->GetUpVector());
    auto cameraForward = YTE::OurVec3ToBt(orientation->GetForwardVector());

    YTE::UBOView uboView = camera->ConstructUBOView();

    btVector3 rayTo = getRayTo(
      uboView,
      rayFrom, aEvent->WorldCoordinates,
      mWindow->GetWidth(),
      mWindow->GetHeight(),
      camera->GetFarPlane());

    btCollisionWorld::ClosestRayResultCallback rayCallback(rayFrom, rayTo);
    mDynamicsWorld->rayTest(rayFrom, rayTo, rayCallback);

    if (rayCallback.hasHit())
    {
      std::cout << "OBJECT HIT" << std::endl;
      auto obj = static_cast<YTE::Composition*>(rayCallback.m_collisionObject->getUserPointer());

      if (obj->GetName() == "X_Axis" || obj->GetName() == "Y_Axis" || obj->GetName() == "Z_Axis")
      {
        mIsGizmoActive = true;
        mCurrentAxis = obj;
      }
      else
      {
        mCurrentAxis = nullptr;
        mCurrentObj = obj;
      }

      auto& browser = mMainWindow->GetObjectBrowser();
      auto item = browser.FindItemByComposition(mCurrentObj);

      // TODO(Evan/Nick): change to setSelectedItem for drag select in future
      browser.setCurrentItem(reinterpret_cast<QTreeWidgetItem*>(item), 0);

      mIsHittingObject = true;
    }


    /*
    // debug printing for gizmo axis transform values
    {
      auto gizObj = mMainWindow->GetGizmo()->mGizmoObj;

      // x axis
      YTE::String name = "X_Axis";
      auto *axis = gizObj->FindFirstCompositionByName(name);
      auto t = axis->GetComponent<YTE::Transform>()->GetTranslation();
      auto wt = axis->GetComponent<YTE::Transform>()->GetWorldTranslation();
      std::cout << "X Trans: (" << t.x << "," << t.y << "," << t.z << ")" << std::endl;
      std::cout << "X WrldT: (" << wt.x << "," << wt.y << "," << wt.z << ")" << std::endl;

      // y axis
      name = "Y_Axis";
      axis = gizObj->FindFirstCompositionByName(name);
      t = axis->GetComponent<YTE::Transform>()->GetTranslation();
      wt = axis->GetComponent<YTE::Transform>()->GetWorldTranslation();
      std::cout << "Y Trans: (" << t.x << "," << t.y << "," << t.z << ")" << std::endl;
      std::cout << "Y WrldT: (" << wt.x << "," << wt.y << "," << wt.z << ")" << std::endl;

      // y axis
      name = "Z_Axis";
      axis = gizObj->FindFirstCompositionByName(name);
      t = axis->GetComponent<YTE::Transform>()->GetTranslation();
      wt = axis->GetComponent<YTE::Transform>()->GetWorldTranslation();
      std::cout << "Z Trans: (" << t.x << "," << t.y << "," << t.z << ")" << std::endl;
      std::cout << "Z WrldT: (" << wt.x << "," << wt.y << "," << wt.z << ")" << std::endl;

    }
    */
  }

  void PhysicsHandler::OnMousePersist(YTE::MouseButtonEvent * aEvent)
  {
    if (mIsHittingObject)
    {
      if (mIsGizmoActive)
      {
        Gizmo *giz = mMainWindow->GetGizmo();

        // get the change in position this frame for the mouse
        glm::i32vec2 pos = aEvent->Mouse->GetCursorPosition();
        glm::vec4 pos4 = glm::vec4(pos.x, pos.y, 0.0, 1.0);

        // convert the two positions to world coordinates
        auto view = mSpace->GetComponent<YTE::GraphicsView>();
        auto camera = view->GetLastCamera();
        YTE::UBOView uboView = camera->ConstructUBOView();

        auto camPos = camera->GetOwner()->GetComponent<YTE::Transform>()->GetWorldTranslation();

        // get distance between camera and object
        auto gizPos = giz->mGizmoObj->GetComponent<YTE::Transform>()->GetWorldTranslation();

        auto dist = glm::length(camPos - gizPos);

        auto btCamPos = YTE::OurVec3ToBt(camPos);

        auto gizmosFriendMouse = getRayTo(uboView, btCamPos, aEvent->WorldCoordinates, mWindow->GetWidth(), mWindow->GetHeight(), dist);

        auto realDelta = YTE::BtToOurVec3(gizmosFriendMouse) - gizPos;

        // print out mouse click world coordinates
        //auto mc = BtToOurVec3(gizmosFriendMouse);
        //std::cout << "M Coords: (" << mc.x << "," << mc.y << "," << mc.z << ")" << std::endl;

        // debug printing for gizmo axis transform values
        {
          auto gizObj = mMainWindow->GetGizmo()->mGizmoObj;

          /*
          // x axis
          YTE::String name = "X_Axis";
          auto *axis = gizObj->FindFirstCompositionByName(name);
          auto t = axis->GetComponent<YTE::Transform>()->GetTranslation();
          auto wt = axis->GetComponent<YTE::Transform>()->GetWorldTranslation();
          std::cout << "X Trans: (" << t.x << "," << t.y << "," << t.z << ")" << std::endl;
          std::cout << "X WrldT: (" << wt.x << "," << wt.y << "," << wt.z << ")" << std::endl;

          // y axis
          name = "Y_Axis";
          axis = gizObj->FindFirstCompositionByName(name);
          t = axis->GetComponent<YTE::Transform>()->GetTranslation();
          wt = axis->GetComponent<YTE::Transform>()->GetWorldTranslation();
          std::cout << "Y Trans: (" << t.x << "," << t.y << "," << t.z << ")" << std::endl;
          std::cout << "Y WrldT: (" << wt.x << "," << wt.y << "," << wt.z << ")" << std::endl;

          // y axis
          name = "Z_Axis";
          axis = gizObj->FindFirstCompositionByName(name);
          t = axis->GetComponent<YTE::Transform>()->GetTranslation();
          wt = axis->GetComponent<YTE::Transform>()->GetWorldTranslation();
          std::cout << "Z Trans: (" << t.x << "," << t.y << "," << t.z << ")" << std::endl;
          std::cout << "Z WrldT: (" << wt.x << "," << wt.y << "," << wt.z << ")" << std::endl;
          */
        }

        switch (giz->GetCurrentMode())
        {
        case Gizmo::Select:
        {
          break;
        }

        case Gizmo::Translate:
        {
          giz->mGizmoObj->FindFirstCompositionByName(mCurrentAxis->GetName())->GetComponent<Translate>()->MoveObject(realDelta);
          break;
        }

        case Gizmo::Scale:
        {
          giz->mGizmoObj->GetComponent<Scale>()->ScaleObject(realDelta);
          break;
        }

        case Gizmo::Rotate:
        {
          // need to change to send amount object should be rotated
          giz->mGizmoObj->GetComponent<Rotate>()->RotateObject(realDelta);
          break;
        }

        }

        mPrevMousePos = pos4;
      }
    }
  }

  void PhysicsHandler::OnMouseRelease(YTE::MouseButtonEvent * aEvent)
  {
    mIsHittingObject = false;
    mIsGizmoActive = false;
  }

  void PhysicsHandler::Add(YTE::Composition *aComposition)
  {
    // We don't want to click the camera.
    // TODO: But maybe one day?
    if (nullptr != aComposition->GetComponent<YTE::Camera>())
    {
      return;
    }

    auto transform = aComposition->GetComponent<YTE::Transform>();

    btTransform bTransform;
    btVector3 origin{ 0.f, 0.f, 0.f };
    btVector3 scale{ 1.f, 1.f, 1.f };
    btQuaternion rotation;

    if (nullptr != transform)
    {
      scale = YTE::OurVec3ToBt(transform->GetWorldScale());

      origin = YTE::OurVec3ToBt(transform->GetWorldTranslation());
      rotation = YTE::OurQuatToBt(transform->GetWorldRotation());
    }

    bTransform.setOrigin(origin);
    bTransform.setRotation(rotation);

    auto model = aComposition->GetComponent<YTE::Model>();

    auto uPtr = std::make_unique<PickerObject>();
    auto obj = uPtr.get();

    mObjects.emplace(aComposition, std::move(uPtr));

    aComposition->YTERegister(YTE::Events::PositionChanged, obj, &PickerObject::ChangedPositionAndRotation);
    aComposition->YTERegister(YTE::Events::RotationChanged, obj, &PickerObject::ChangedPositionAndRotation);
    aComposition->YTERegister(YTE::Events::ScaleChanged, obj, &PickerObject::ChangedScale);

    if (nullptr != model)
    {
      auto inMesh = model->GetInstantiatedMesh();

      YTE::Mesh *mesh = inMesh->mMesh;

      if (mesh != nullptr)
      {
        obj->mTriangles;

        for (auto &submesh : mesh->mParts)
        {
          auto indexSize = submesh.mIndexBuffer.size();

          DebugAssert((indexSize % 3) == 0, "Index buffer must be divisible by 3.");

          for (size_t i = 0; i < indexSize; i += 3)
          {
            auto i1 = submesh.mIndexBuffer.at(i + 0);
            auto i2 = submesh.mIndexBuffer.at(i + 1);
            auto i3 = submesh.mIndexBuffer.at(i + 2);

            obj->mTriangles.addTriangle(YTE::OurVec3ToBt(submesh.mVertexBuffer.at(i1).mPosition),
              YTE::OurVec3ToBt(submesh.mVertexBuffer.at(i2).mPosition),
              YTE::OurVec3ToBt(submesh.mVertexBuffer.at(i3).mPosition));
          }
        }

        obj->mTriangleMeshShape = std::make_unique<btBvhTriangleMeshShape>(&obj->mTriangles, true);

        obj->mShape = obj->mTriangleMeshShape.get();

        obj->mTriangleMeshShape->setLocalScaling(scale);
        //mTriangleMeshShape->buildOptimizedBvh();
      }
    }
    else
    {
      obj->mBoxShape = std::make_unique<btBoxShape>(scale / 2.0f);

      obj->mShape = obj->mBoxShape.get();
    }

    obj->mGhostBody = std::make_unique<btGhostObject>();
    obj->mGhostBody->setCollisionShape(obj->mShape);

    //mGhostBody->activate(true);
    //mGhostBody->setActivationState(DISABLE_DEACTIVATION);
    obj->mGhostBody->setUserPointer(aComposition);

    obj->mGhostBody->setWorldTransform(bTransform);

    mDynamicsWorld->addCollisionObject(obj->mGhostBody.get());
  }

  void PhysicsHandler::Remove(YTE::Composition *aComposition)
  {
    auto it = mObjects.find(aComposition);

    if (it != mObjects.end())
    {
      mDynamicsWorld->removeCollisionObject(it->second->mGhostBody.get());

      mObjects.erase(it);
    }
  }


  void PhysicsHandler::AddedComposition(YTE::CompositionAdded *aEvent)
  {
    Add(aEvent->mComposition);
  }

  void PhysicsHandler::RemovedComposition(YTE::CompositionRemoved *aEvent)
  {
    Remove(aEvent->mComposition);
  }

}
