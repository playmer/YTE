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

#include "YTE/Graphics/Generics/InstantiatedModel.hpp"
#include "YTE/Graphics/Generics/Mesh.hpp"
#include "YTE/Graphics/Model.hpp"

#include "YTE/Physics/CollisionBody.hpp"
#include "YTE/Physics/GhostBody.hpp"
#include "YTE/Physics/MeshCollider.hpp"
#include "YTE/Physics/PhysicsSystem.hpp"
#include "YTE/Physics/RigidBody.hpp"
#include "YTE/Physics/Transform.hpp"

namespace YTE
{
  YTEDefineType(MeshCollider)
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
    

    Mesh *mesh{ nullptr };
    auto model = mOwner->GetComponent<Model>();
    
    if (model != nullptr)
    {
      mesh = model->GetMesh();
    }
    
    if (mesh != nullptr)
    {
      for (auto &submesh : mesh->mColliderParts)
      {
        DebugAssert((submesh.mIndexBuffer.size() % 3) == 0, "Index buffer must be divisible by 3.");

        auto indexSize = submesh.mIndexBuffer.size();
      
        for (size_t i = 0; i < indexSize; i += 3)
        {
          auto i1 = submesh.mIndexBuffer.at(i + 0);
          auto i2 = submesh.mIndexBuffer.at(i + 1);
          auto i3 = submesh.mIndexBuffer.at(i + 2);
      
          submesh.mColliderVertexBuffer.at(i1);
      
          mTriangles.addTriangle(OurVec3ToBt(submesh.mColliderVertexBuffer.at(i1)),
                                 OurVec3ToBt(submesh.mColliderVertexBuffer.at(i2)),
                                 OurVec3ToBt(submesh.mColliderVertexBuffer.at(i3)));
        }
      }
    }
    
    mTriangleMeshShape = std::make_unique<btBvhTriangleMeshShape>(&mTriangles, true);
    
    mTriangleMeshShape->setLocalScaling(OurVec3ToBt(scale));
    //mTriangleMeshShape->buildOptimizedBvh();
    
    mCollider = std::make_unique<btCollisionObject>();
    mCollider->setCollisionShape(mTriangleMeshShape.get());
    mCollider->setWorldTransform(bulletTransform);
    mCollider->setUserPointer(mOwner);
  }
}
