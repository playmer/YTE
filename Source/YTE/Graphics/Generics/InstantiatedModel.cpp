///////////////////
// Author: Andrew Griffin
// YTE - Graphics - Generics
///////////////////

#include "YTE/Graphics/Generics/Mesh.hpp"
#include "YTE/Graphics/Generics/InstantiatedModel.hpp"
#include "YTE/Graphics/Generics/Renderer.hpp"

namespace YTE
{
  YTEDefineType(InstantiatedModel)
  {
    RegisterType<InstantiatedModel>();
    TypeBuilder<InstantiatedModel> builder;
  }

  UBOs::Animation InstantiatedModel::cAnimation;

  InstantiatedModel::InstantiatedModel(Renderer* aRenderer)
    : mRenderer{ aRenderer }
    , mMesh{ nullptr }
    , mModelUBO{ aRenderer->CreateUBO<UBOs::Model>() }
    , mAnimationUBO{ aRenderer->CreateUBO<UBOs::Animation>() }
    , mModelMaterialUBO{ aRenderer->CreateUBO<UBOs::Material>() }
  {
  }

  void InstantiatedModel::Create()
  {
    UpdateUBOAnimation(&cAnimation);

    UBOs::Material modelMaterial{};
    modelMaterial.mDiffuse = glm::vec4{ 1.0f, 1.0f, 1.0f, 1.0f };
    modelMaterial.mAmbient = glm::vec4{ 1.0f, 1.0f, 1.0f, 1.0f };
    modelMaterial.mSpecular = glm::vec4{ 0.0f, 0.0f, 0.0f, 1.0f };
    modelMaterial.mEmissive = glm::vec4{ 0.0f, 0.0f, 0.0f, 1.0f };
    modelMaterial.mShininess = 1.0f;

    UpdateUBOMaterial(&modelMaterial);

    mUBOModelData.mModelMatrix = glm::mat4(1.0f);


    // create descriptor sets
    for (auto[submeshIt, i] : enumerate(mMesh->mParts))
    {
      auto& submesh = *submeshIt;

      mSubmeshMaterialsUBO.emplace_back(mRenderer->CreateUBO<UBOs::Material>(), 
                                        submesh.mUBOMaterial);

      UpdateUBOSubmeshMaterial(&submesh.mUBOMaterial, i);
    }
  }

  void InstantiatedModel::SetDefaultAnimationOffset()
  {
    UpdateUBOAnimation(mMesh->mSkeleton.GetDefaultOffsets());
  }
}
