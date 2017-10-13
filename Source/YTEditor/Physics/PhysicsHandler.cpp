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

#include "PhysicsHandler.hpp"

using namespace YTE;


void PickerObject::ChangedPositionAndRotation(TransformChanged *aEvent)
{
  btTransform transform;
  transform.setOrigin(OurVec3ToBt(aEvent->WorldPosition));
  transform.setRotation(OurQuatToBt(aEvent->WorldRotation));

  mGhostBody->setWorldTransform(transform);
}

void PickerObject::ChangedScale(TransformChanged *aEvent)
{

  btVector3 scale{ OurVec3ToBt(aEvent->WorldScale) };
  if (nullptr != mTriangleMeshShape)
  {
    mTriangleMeshShape->setLocalScaling(scale);
  }
  else if (nullptr != mBoxShape)
  {
    mBoxShape->setLocalScaling(scale / 2.0f);
  }
}

PhysicsHandler::PhysicsHandler(Space *aSpace, Window *aWindow)
  : mSpace(aSpace)
  , mWindow(aWindow)
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

  aSpace->YTERegister(Events::CompositionAdded, this, &PhysicsHandler::AddedComposition);
  aSpace->YTERegister(Events::CompositionRemoved, this, &PhysicsHandler::RemovedComposition);
  aWindow->mMouse.YTERegister(Events::MousePress, this, &PhysicsHandler::Click);
}


btVector3 getRayTo(UBOView& aView,
                   btVector3& aRayFrom,
                   glm::i32vec2 aMouseCoordinates,
                   u32 aWidth,
                   u32 aHeight,
                   float aFar)
{
  UBOView& uboView = aView;

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

  return aRayFrom + OurVec3ToBt(rayDirectionWorld) * aFar;
}

void PhysicsHandler::Click(MouseButtonEvent *aEvent)
{
  if (aEvent->Button != Mouse_Buttons::Left)
  {
    return;
  }

  static size_t i = 0;

  auto view = mSpace->GetComponent<GraphicsView>();
  auto camera = view->GetLastCamera();
  auto owner = camera->GetOwner();
  auto transform = owner->GetComponent<Transform>();
  auto orientation = owner->GetComponent<Orientation>();
  auto rayFrom = OurVec3ToBt(transform->GetTranslation());
  auto cameraUp = OurVec3ToBt(orientation->GetUpVector());
  auto cameraForward = OurVec3ToBt(orientation->GetForwardVector());

  UBOView uboView = camera->ConstructUBOView();
  
  btVector3 rayTo = getRayTo(
    uboView, 
    rayFrom, aEvent->WorldCoordinates, 
    mWindow->GetWidth(), 
    mWindow->GetHeight(), 
    camera->GetFarPlane());

  //btVector3 rayTo = getRayTo(rayFrom,
  //                           cameraUp,
  //                           cameraForward,
  //                           aEvent->WorldCoordinates.x,
  //                           aEvent->WorldCoordinates.y,
  //                           mWindow->GetHeight(),
  //                           mWindow->GetWidth(),
  //                           camera->GetFarPlane(),
  //                           camera->GetFieldOfViewY());

  //auto rayTo = rayFrom + (cameraForward * 10000.f);
  
  btCollisionWorld::ClosestRayResultCallback rayCallback(rayFrom, rayTo);
  mDynamicsWorld->rayTest(rayFrom, rayTo, rayCallback);
  
  if (rayCallback.hasHit())
  {
    auto composition = static_cast<YTE::Composition*>(rayCallback.m_collisionObject->getUserPointer());

    std::cout << i++ << ": "<< composition->GetName().c_str() << "\n";
  }
}

void PhysicsHandler::Add(YTE::Composition *aComposition)
{
  // We don't want to click the camera.
  // TODO: But maybe one day?
  if (nullptr != aComposition->GetComponent<Camera>())
  {
    return;
  }

  auto transform = aComposition->GetComponent<Transform>();

  btTransform bTransform;
  btVector3 origin{ 0.f, 0.f, 0.f };
  btVector3 scale{ 1.f, 1.f, 1.f };
  btQuaternion rotation;

  if (nullptr != transform)
  {
    scale = OurVec3ToBt(transform->GetWorldScale());

    origin = OurVec3ToBt(transform->GetWorldTranslation());
    rotation = OurQuatToBt(transform->GetWorldRotation());
  }

  bTransform.setOrigin(origin);
  bTransform.setRotation(rotation);

  auto model = aComposition->GetComponent<Model>();

  auto uPtr = std::make_unique<PickerObject>();
  auto obj = uPtr.get();

  mObjects.emplace(aComposition, std::move(uPtr));

  aComposition->YTERegister(Events::PositionChanged, obj, &PickerObject::ChangedPositionAndRotation);
  aComposition->YTERegister(Events::RotationChanged, obj, &PickerObject::ChangedPositionAndRotation);
  aComposition->YTERegister(Events::ScaleChanged, obj,    &PickerObject::ChangedScale);

  if (nullptr != model)
  {
    auto inMesh = model->GetInstantiatedMesh();

    Mesh *mesh = inMesh->mMesh;

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
          
          obj->mTriangles.addTriangle(OurVec3ToBt(submesh.mVertexBuffer.at(i1).mPosition),
                                      OurVec3ToBt(submesh.mVertexBuffer.at(i2).mPosition),
                                      OurVec3ToBt(submesh.mVertexBuffer.at(i3).mPosition));
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
