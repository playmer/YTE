#include "btBulletCollisionCommon.h"
#include "BulletCollision/CollisionDispatch/btGhostObject.h"

#include "YTE/Core/Space.hpp"
#include "YTE/Graphics/Camera.hpp"
#include "YTE/Graphics/GraphicsView.hpp"
#include "YTE/Graphics/Generics/Mesh.hpp"
#include "YTE/Graphics/Model.hpp"
#include "YTE/Graphics/Generics/Mesh.hpp"
#include "YTE/Physics/Orientation.hpp"
#include "YTE/Physics/PhysicsSystem.hpp"
#include "YTE/Physics/Transform.hpp"

#include "YTEditor/Gizmos/Gizmo.hpp"
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
    auto pos = aEvent->WorldPosition;
    btTransform transform;
    transform.setOrigin(YTE::ToBullet(aEvent->WorldPosition));
    transform.setRotation(YTE::ToBullet(aEvent->WorldRotation));

    mGhostBody->setWorldTransform(transform);
  }

  void PickerObject::ChangedScale(YTE::TransformChanged *aEvent)
  {

    btVector3 scale{ YTE::ToBullet(aEvent->WorldScale) };
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

    aSpace->RegisterEvent<&PhysicsHandler::AddedComposition>(YTE::Events::CompositionAdded, this);
    aSpace->RegisterEvent<&PhysicsHandler::RemovedComposition>(YTE::Events::CompositionRemoved, this);
    aWindow->mMouse.RegisterEvent<&PhysicsHandler::OnMousePress>(YTE::Events::MousePress, this);
    aWindow->mMouse.RegisterEvent<&PhysicsHandler::OnMousePersist>(YTE::Events::MousePersist, this);
    aWindow->mMouse.RegisterEvent<&PhysicsHandler::OnMouseRelease>(YTE::Events::MouseRelease, this);
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

    return aRayFrom + YTE::ToBullet(rayDirectionWorld) * aFar;
  }

  void PhysicsHandler::OnMousePress(YTE::MouseButtonEvent *aEvent)
  {
    auto imguiLayerComposition = mMainWindow->GetImguiLayer();
    
    if (imguiLayerComposition)
    {
      auto imguiLayer = imguiLayerComposition->GetComponent<YTE::ImguiLayer>();

      auto io = imguiLayer->GetIO();

      if (imguiLayer->IsOver() || io.WantCaptureMouse)
      {
        return;
      }
    }

    if (aEvent->Button != YTE::MouseButtons::Left)
    {
      return;
    }

    static size_t i = 0;

    auto view = mSpace->GetComponent<YTE::GraphicsView>();
    auto camera = view->GetActiveCamera();
    auto owner = camera->GetOwner();
    auto transform = owner->GetComponent<YTE::Transform>();
    auto orientation = owner->GetComponent<YTE::Orientation>();
    auto rayFrom = YTE::ToBullet(transform->GetTranslation());
    auto cameraUp = YTE::ToBullet(orientation->GetUpVector());
    auto cameraForward = YTE::ToBullet(orientation->GetForwardVector());

    YTE::UBOView uboView = camera->ConstructUBOView();

    btVector3 rayTo = getRayTo(
      uboView,
      rayFrom, aEvent->WorldCoordinates,
      mWindow->GetWidth(),
      mWindow->GetHeight(),
      camera->GetFarPlane());

    btCollisionWorld::ClosestRayResultCallback rayCallback(rayFrom, rayTo);
    mDynamicsWorld->rayTest(rayFrom, rayTo, rayCallback);

    auto previousObject = mCurrentObj;

    if (rayCallback.hasHit())
    {
      auto obj = mPickedObj = static_cast<YTE::Composition*>(rayCallback.m_collisionObject->getUserPointer());

      YTE::Transform *pickedTrans = mPickedObj->GetComponent<YTE::Transform>();

      mPickedDistance = (YTE::ToBullet(pickedTrans->GetWorldTranslation()) - rayFrom).length();
      
      mCurrentObj = obj;

      auto& browser = mMainWindow->GetObjectBrowser();
      auto item = browser.FindItemByComposition(mCurrentObj);

      // TODO(Evan/Nick): change to setSelectedItem for drag select in future
      browser.setCurrentItem(reinterpret_cast<QTreeWidgetItem*>(item), 0);

      auto model = mCurrentObj->GetComponent<YTE::Model>();

      if (model)
      {
        auto instanceModel = model->GetInstantiatedModel();

        if (instanceModel.size())
        {
          for (auto &mesh : instanceModel)
          {
            auto material = mesh->GetUBOMaterialData();
            material.mFlags |= 1u << (YTE::u32)YTE::UBOMaterialFlags::IsSelected / 2;
            mesh->UpdateUBOMaterial(&material);
          }
        }
      }

      mIsHittingObject = true;
    }

    if (nullptr != previousObject &&
        (false == rayCallback.hasHit() ||
        (mCurrentObj != previousObject && 
         false == mIsGizmoActive)))
    {
      auto model = previousObject->GetComponent<YTE::Model>();

      if (model)
      {
        auto instanceModel = model->GetInstantiatedModel();

        if (instanceModel.size())
        {
          for (auto &mesh : instanceModel)
          {
            auto material = mesh->GetUBOMaterialData();
            material.mFlags &= ~(1u << (YTE::u32)YTE::UBOMaterialFlags::IsSelected / 2);
            mesh->UpdateUBOMaterial(&material);
          }
        }
      }
    }
  }

  void PhysicsHandler::OnMousePersist(YTE::MouseButtonEvent *aEvent)
  {
  }

  void PhysicsHandler::OnMouseRelease(YTE::MouseButtonEvent *aEvent)
  {
    YTE::UnusedArguments(aEvent);

    mIsHittingObject = false;
    mIsGizmoActive = false;
    auto it = mObjects.find(mPickedObj);
    
    if (it != mObjects.end())
    {
      it->second->mGhostBody->setDeactivationTime(0.f);
      mDynamicsWorld->updateAabbs();
    }
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
      scale = YTE::ToBullet(transform->GetWorldScale());
      origin = YTE::ToBullet(transform->GetWorldTranslation());
      rotation = YTE::ToBullet(transform->GetWorldRotation());
    }

    bTransform.setOrigin(origin);
    bTransform.setRotation(rotation);

    auto model = aComposition->GetComponent<YTE::Model>();

    auto uPtr = std::make_unique<PickerObject>();
    auto obj = uPtr.get();

    mObjects.emplace(aComposition, std::move(uPtr));

    aComposition->RegisterEvent<&PickerObject::ChangedPositionAndRotation>(YTE::Events::PositionChanged, obj);
    aComposition->RegisterEvent<&PickerObject::ChangedPositionAndRotation>(YTE::Events::RotationChanged, obj);
    aComposition->RegisterEvent<&PickerObject::ChangedScale>(YTE::Events::ScaleChanged, obj);

    if (model)
    {
      aComposition->RegisterEvent<&PhysicsHandler::OnModelChanged>(YTE::Events::ModelChanged, this);
    }

    if (nullptr != model && model->GetMesh())
    {
      auto mesh = model->GetMesh();

      auto it = mShapeCache.find(model->GetMeshName());
      if (it != mShapeCache.end())
      {
        printf("Found in cache: %s\n", model->GetMeshName().c_str());
        obj->mTriangleMeshShape = std::make_unique<btBvhTriangleMeshShape>(&it->second, true);
      }
      else
      {
        auto& triangles = mShapeCache[model->GetMeshName()];

        for (auto &submesh : mesh->mParts)
        {
          auto indexSize = submesh.mIndexBuffer.size();

          DebugAssert((indexSize % 3) == 0, "Index buffer must be divisible by 3.");

          for (size_t i = 0; i < indexSize; i += 3)
          {
            auto i1 = submesh.mIndexBuffer.at(i + 0);
            auto i2 = submesh.mIndexBuffer.at(i + 1);
            auto i3 = submesh.mIndexBuffer.at(i + 2);

            triangles.addTriangle(YTE::ToBullet(submesh.mVertexBuffer.at(i1).mPosition),
                                  YTE::ToBullet(submesh.mVertexBuffer.at(i2).mPosition),
                                  YTE::ToBullet(submesh.mVertexBuffer.at(i3).mPosition));
          }
        }

        obj->mTriangleMeshShape = std::make_unique<btBvhTriangleMeshShape>(&triangles, true);
      }

      obj->mShape = obj->mTriangleMeshShape.get();

      obj->mTriangleMeshShape->setLocalScaling(scale);
    }
    else
    {
      //obj->mBoxShape = std::make_unique<btBoxShape>(scale / 2.0f);
      obj->mBoxShape = std::make_unique<btBoxShape>(btVector3(0.000001f, 0.000001f, 0.000001f));
      obj->mShape = obj->mBoxShape.get();
    }

    obj->mGhostBody = std::make_unique<btGhostObject>();
    obj->mGhostBody->setCollisionShape(obj->mShape);

    obj->mGhostBody->activate(true);
    obj->mGhostBody->setActivationState(DISABLE_DEACTIVATION);
    obj->mGhostBody->setUserPointer(aComposition);

    obj->mGhostBody->setWorldTransform(bTransform);

    mDynamicsWorld->addCollisionObject(obj->mGhostBody.get());
  }

  void PhysicsHandler::Remove(YTE::Composition *aComposition)
  {
    if (mCurrentObj == aComposition)
    {
      mCurrentObj = nullptr;
    }

    auto it = mObjects.find(aComposition);

    if (it != mObjects.end())
    {
      aComposition->DeregisterEvent<&PhysicsHandler::OnModelChanged>(YTE::Events::ModelChanged,  this);

      mDynamicsWorld->removeCollisionObject(it->second->mGhostBody.get());

      mObjects.erase(it);
    }
  }

  void PhysicsHandler::Update()
  {
    mDynamicsWorld->updateAabbs();
  }

  void PhysicsHandler::AddedComposition(YTE::CompositionAdded *aEvent)
  {
    Add(aEvent->mComposition);
  }

  void PhysicsHandler::RemovedComposition(YTE::CompositionRemoved *aEvent)
  {
    Remove(aEvent->mComposition);
  }

  void PhysicsHandler::OnModelChanged(YTE::ModelChanged *aEvent)
  {
    Remove(aEvent->Object);
    Add(aEvent->Object);
  }
}
