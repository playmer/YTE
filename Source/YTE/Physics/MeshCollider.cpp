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

#include "YTE/Graphics/Model.hpp"

#include "YTE/Physics/CollisionBody.h"
#include "YTE/Physics/GhostBody.h"
#include "YTE/Physics/MeshCollider.h"
#include "YTE/Physics/PhysicsSystem.h"
#include "YTE/Physics/RigidBody.h"
#include "YTE/Physics/Transform.h"

namespace YTE
{
  DefineType(MeshCollider)
  {
    YTERegisterType(MeshCollider);

    std::vector<std::vector<Type*>> deps = { { Transform::GetStaticType() },
                                             { RigidBody::GetStaticType(),
                                               CollisionBody::GetStaticType(), 
                                               GhostBody::GetStaticType(), } };

    GetStaticType()->AddAttribute<ComponentDependencies>(deps);
  }

  MeshCollider::MeshCollider(Composition *aOwner, Space *aSpace, RSValue *aProperties)
    : Collider(aOwner, aSpace)
  {
    DeserializeByType<MeshCollider*>(aProperties, this, MeshCollider::GetStaticType());
  }

  // TODO (Josh): Reimplement Mesh collider.
  void MeshCollider::PhysicsInitialize()
  {
    // Get info from transform and feed that ish to the Bullet collider
    auto transform = mOwner->GetComponent<Transform>();
    auto translation = transform->GetTranslation();
    auto scale = transform->GetScale();
    auto rotation = transform->GetRotation();
    auto bulletRot = OurQuatToBt(rotation);
    auto bulletTransform = btTransform(bulletRot, btVector3(translation.x, translation.y, translation.z));
    
    auto model = mOwner->GetComponent<Model>();
    
    //if (model != nullptr)
    //{
    //  mesh = model->GetMesh();
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
