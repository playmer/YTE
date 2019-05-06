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
    RegisterType<MeshCollider>();
    TypeBuilder<MeshCollider> builder;

    std::vector<std::vector<Type*>> deps = { { TypeId<Transform>() } };

    GetStaticType()->AddAttribute<ComponentDependencies>(deps);
  }

  MeshCollider::MeshCollider(Composition *aOwner, Space *aSpace)
    : Collider(aOwner, aSpace)
  {
  }

  void MeshCollider::PhysicsInitialize()
  {
    // Get info from transform and feed that ish to the Bullet collider
    auto transform = mOwner->GetComponent<Transform>();
    auto translation = transform->GetTranslation();
    auto scale = transform->GetScale();
    auto rotation = transform->GetRotation();
    auto bulletRot = ToBullet(rotation);
    auto bulletTransform = btTransform(bulletRot, ToBullet(translation));
    

    Mesh *mesh{ nullptr };
    auto model = mOwner->GetComponent<Model>();
    
    if (model != nullptr)
    {
      mesh = model->GetMesh();
    }
    
    if (mesh != nullptr)
    {
      for (auto const& submesh : mesh->mParts)
      {
        auto const& data = submesh.mData;

        DebugAssert((data.mIndexData.size() % 3) == 0, "Index buffer must be divisible by 3.");

        auto indexSize = data.mIndexData.size();
      
        for (size_t i = 0; i < indexSize; i += 3)
        {
          auto i1 = data.mIndexData[i + 0];
          auto i2 = data.mIndexData[i + 1];
          auto i3 = data.mIndexData[i + 2];
      
          mTriangles.addTriangle(ToBullet(data.mVertexData[i1].mPosition),
                                 ToBullet(data.mVertexData[i2].mPosition),
                                 ToBullet(data.mVertexData[i3].mPosition));
        }
      }
    }
    
    mTriangleMeshShape = std::make_unique<btBvhTriangleMeshShape>(&mTriangles, true);
    
    mTriangleMeshShape->setLocalScaling(ToBullet(scale));
    //mTriangleMeshShape->buildOptimizedBvh();
    
    mCollider = std::make_unique<btCollisionObject>();
    mCollider->setCollisionShape(mTriangleMeshShape.get());
    mCollider->setWorldTransform(bulletTransform);
    mCollider->setUserPointer(mOwner);
  }


  void MeshCollider::ScaleUpdate(TransformChanged *aEvent)
  {
    if (mTriangleMeshShape)
    {
      mTriangleMeshShape->setLocalScaling(ToBullet(aEvent->WorldScale));
    }
  }
}
