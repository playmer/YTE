/******************************************************************************/
/*!
* \author Isaac Dayton
* \date   2016-03-06
*
* \copyright All content 2016 DigiPen (USA) Corporation, all rights reserved.
*/
/******************************************************************************/
#include "YTE/Core/Composition.hpp"
#include "YTE/Core/Engine.hpp"
#include "YTE/Core/Space.hpp"

#include "CollisionBody.h"
#include "GhostBody.h"
#include "MeshCollider.h"
#include "PhysicsSystem.h"
#include "RigidBody.h"
#include "Transform.h"

namespace YTE
{
  DefineType(MeshCollider)
  {
    YTERegisterType(MeshCollider);
  }

  MeshCollider::MeshCollider(Composition *aOwner, Space *aSpace, RSValue *aProperties)
    : Collider(aOwner, aSpace)
  {
    DeserializeByType<MeshCollider*>(aProperties, this, MeshCollider::GetStaticType());
  }

  // TODO (Josh): Reimplement Mesh collider.
  void MeshCollider::PhysicsInitialize()
  {
    //DebugAssert(mOwner->GetComponent<RigidBody>()     == nullptr && 
    //            mOwner->GetComponent<CollisionBody>() == nullptr && 
    //            mOwner->GetComponent<GhostBody>()     == nullptr,
    //            "Colliders require a Body component of some sort, sorry!\n ObjectName: %s", mOwner->GetName().c_str());
    //
    //// Get info from transform and feed that ish to the Bullet collider
    //auto transform = mOwner->GetComponent<Transform>();
    //auto translation = transform->GetTranslation();
    //auto scale = transform->GetScale();
    //auto rotation = transform->GetRotation();
    //auto bulletRot = btQuaternion(rotation.mVector.x, rotation.mVector.y, rotation.mVector.z, rotation.mScalar);
    //auto bulletTransform = btTransform(bulletRot, btVector3(translation.x, translation.y, translation.z));
    //
    //
    //Graphics::Mesh *mesh = nullptr;
    //
    //auto figure = mOwner->GetComponent<Graphics::Figure>();
    //
    //if (figure != nullptr)
    //{
    //  mesh = figure->GetMesh();
    //}
    //else
    //{
    //  auto invisFigure = mOwner->GetComponent<Graphics::InvisibleFigure>();
    //
    //  if (invisFigure != nullptr)
    //  {
    //    mesh = invisFigure->GetMesh();
    //  }
    //}
    //
    //if (mesh != nullptr)
    //{
    //  for (auto &face : mesh->GetFaces())
    //  {
    //    mTriangles.addTriangle(OurVec3ToBt(mesh->GetVertex(face.mIndex1)), 
    //                           OurVec3ToBt(mesh->GetVertex(face.mIndex2)), 
    //                           OurVec3ToBt(mesh->GetVertex(face.mIndex3)));
    //  }
    //}
    //
    //mTriangleMeshShape = std::make_unique<btBvhTriangleMeshShape>(&mTriangles, true);
    //
    //mTriangleMeshShape->setLocalScaling(OurVec3ToBt(scale));
    ////mTriangleMeshShape->buildOptimizedBvh();
    //
    //mCollider = std::make_unique<btCollisionObject>();
    //mCollider->setCollisionShape(mTriangleMeshShape.get());
    //mCollider->setWorldTransform(bulletTransform);
    //mCollider->setUserPointer(mOwner);
  }
}
