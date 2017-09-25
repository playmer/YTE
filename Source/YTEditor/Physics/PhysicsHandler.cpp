#include "Bullet/btBulletCollisionCommon.h"
#include "Bullet/BulletCollision/CollisionDispatch/btGhostObject.h"

#include "YTE/Core/Space.hpp"

#include "YTE/Graphics/GraphicsView.hpp"
#include "YTE/Graphics/Mesh.hpp"
#include "YTE/Graphics/Model.hpp"

#include "YTE/Physics/PhysicsSystem.hpp"
#include "YTE/Physics/Transform.hpp"

#include "PhysicsHandler.hpp"

using namespace YTE;


void PickerObject::ChangedPositionAndRotation(TransformChanged *aEvent)
{
  btTransform transform;
  transform.setOrigin(OurVec3ToBt(aEvent->Position));
  transform.setRotation(OurQuatToBt(aEvent->Rotation));

  mGhostBody->setWorldTransform(transform);
}

void PickerObject::ChangedScale(TransformChanged *aEvent)
{

  btVector3 scale{ OurVec3ToBt(aEvent->Scale) };
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

void PhysicsHandler::Click(MouseButtonEvent *aEvent)
{
  auto view = mSpace->GetComponent<GraphicsView>();

  
  auto coords = aEvent->WorldCoordinates;
  glm::vec4 lRayStart_NDC(
    ((float)coords.x / (float)mWindow->GetWidth() - 0.5f) * 2.0f, // [0,1024] -> [-1,1]
    ((float)coords.y / (float)mWindow->GetHeight() - 0.5f) * 2.0f, // [0, 768] -> [-1,1]
    -1.0, // The near plane maps to Z=-1 in Normalized Device Coordinates
    1.0f
  );
  glm::vec4 lRayEnd_NDC(
    ((float)coords.x / (float)mWindow->GetWidth() - 0.5f) * 2.0f,
    ((float)coords.y / (float)mWindow->GetHeight() - 0.5f) * 2.0f,
    0.0,
    1.0f
  );

  btCollisionWorld::ClosestRayResultCallback RayCallback(
    btVector3(out_origin.x, out_origin.y, out_origin.z),
    btVector3(out_end.x, out_end.y, out_end.z)
  );
  mDynamicsWorld->rayTest(
    btVector3(out_origin.x, out_origin.y, out_origin.z),
    btVector3(out_end.x, out_end.y, out_end.z),
    RayCallback
  );

  if (RayCallback.hasHit()) {
    std::ostringstream oss;
    oss << "mesh " << (int)RayCallback.m_collisionObject->getUserPointer();
    message = oss.str();
  }
  else {
    message = "background";
  }
}

void PhysicsHandler::Add(YTE::Composition *aComposition)
{
  auto transform = aComposition->GetComponent<Transform>();

  btTransform bTransform;
  btVector3 origin{ 0.f, 0.f, 0.f };
  btVector3 scale{ 1.f, 1.f, 1.f };

  if (nullptr != transform)
  {
    scale = OurVec3ToBt(transform->GetScale());

    origin = OurVec3ToBt(transform->GetTranslation());
  }

  bTransform.setOrigin(origin);

  auto model = aComposition->GetComponent<Model>();

  auto obj = std::make_unique<PickerObject>();

  aComposition->YTERegister(Events::PositionChanged, obj.get(), &PickerObject::ChangedPositionAndRotation);
  aComposition->YTERegister(Events::RotationChanged, obj.get(), &PickerObject::ChangedPositionAndRotation);
  aComposition->YTERegister(Events::ScaleChanged, obj.get(),    &PickerObject::ChangedScale);

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

        for (size_t i = 0; i < submesh.mIndexBufferSize; i += 3)
        {
          auto i1 = submesh.mIndexBuffer.at(i + 0);
          auto i2 = submesh.mIndexBuffer.at(i + 1);
          auto i3 = submesh.mIndexBuffer.at(i + 2);

          submesh.mVertexBuffer.at(i1);

          obj->mTriangles.addTriangle(OurVec3ToBt(submesh.mVertexBuffer.at(i1).mPosition),
                                      OurVec3ToBt(submesh.mVertexBuffer.at(i2).mPosition),
                                      OurVec3ToBt(submesh.mVertexBuffer.at(i3).mPosition));
        }
      }
      
      obj->mTriangleMeshShape = std::make_unique<btBvhTriangleMeshShape>(&obj->mTriangles, true);

      obj->mShape = obj->mTriangleMeshShape.get();

      obj->mTriangleMeshShape->setLocalScaling(scale);
      //mTriangleMeshShape->buildOptimizedBvh();

      obj->mCollider = std::make_unique<btCollisionObject>();
      obj->mCollider->setCollisionShape(obj->mTriangleMeshShape.get());
      obj->mCollider->setWorldTransform(bTransform);
      obj->mCollider->setUserPointer(aComposition);
    }
  }
  else
  {
    obj->mBoxShape = std::make_unique<btBoxShape>(scale / 2.0f);

    obj->mCollider = std::make_unique<btCollisionObject>();
    obj->mCollider->setCollisionShape(obj->mBoxShape.get());
    obj->mCollider->setWorldTransform(bTransform);
    obj->mCollider->setUserPointer(aComposition);
  }

  obj->mGhostBody = std::make_unique<btGhostObject>();
  obj->mGhostBody->setCollisionShape(obj->mShape);

  //mGhostBody->activate(true);
  //mGhostBody->setActivationState(DISABLE_DEACTIVATION);
  obj->mGhostBody->setUserPointer(aComposition);

  btTransform translationAndRotation;
  translationAndRotation.setOrigin(OurVec3ToBt(transform->GetTranslation()));
  translationAndRotation.setRotation(OurQuatToBt(transform->GetRotation()));
  obj->mGhostBody->setWorldTransform(translationAndRotation);

  mDynamicsWorld->addCollisionObject(obj->mGhostBody.get());
}

void PhysicsHandler::Remove(YTE::Composition *aComposition)
{
  auto it = mObjects.find(aComposition);

  if (it == mObjects.end())
  {
    printf("o no\n");
  }

  mDynamicsWorld->removeCollisionObject(it->second->mCollider.get());

  mObjects.erase(it);
}


void PhysicsHandler::AddedComposition(YTE::CompositionAdded *aEvent)
{
  Add(aEvent->mComposition);
}

void PhysicsHandler::RemovedComposition(YTE::CompositionRemoved *aEvent)
{
  Remove(aEvent->mComposition);
}
