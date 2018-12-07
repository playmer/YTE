///////////////////
// Author: Andrew Griffin
// YTE - Graphics - Vulkan
///////////////////

#include "YTE/Graphics/Vulkan/VkInstantiatedModel.hpp"
#include "YTE/Graphics/Vulkan/VkMesh.hpp"
#include "YTE/Graphics/Vulkan/VkRenderer.hpp"
#include "YTE/Graphics/Vulkan/VkRenderedSurface.hpp"
#include "YTE/Graphics/Vulkan/VkTexture.hpp"
#include "YTE/Graphics/Vulkan/VkDeviceInfo.hpp"

namespace YTE
{
  YTEDefineType(VkInstantiatedModel)
  {
    RegisterType<VkInstantiatedModel>();
    TypeBuilder<VkInstantiatedModel> builder;
  }

  VkInstantiatedModel::VkInstantiatedModel(std::string &aModelFile,
                                           VkRenderedSurface *aSurface,
                                           GraphicsView *aView)
    : InstantiatedModel{ aSurface->GetRenderer() }
    , mSurface{aSurface}
    , mView{ aView }
  {
    mVkMesh = mSurface->GetRenderer()->CreateMesh(aModelFile);
    mMesh = mVkMesh->mMesh;

    Create();

    CreateShader();

    mView->RegisterEvent<&VkInstantiatedModel::SurfaceLostEvent>(Events::SurfaceLost, this);
    mView->RegisterEvent<&VkInstantiatedModel::SurfaceGainedEvent>(Events::SurfaceGained, this);
  }

  VkInstantiatedModel::VkInstantiatedModel(VkMesh *aMesh, 
                                           VkRenderedSurface *aSurface,
                                           GraphicsView *aView)
    : InstantiatedModel{ aSurface->GetRenderer() }
    , mSurface{ aSurface }
    , mView{ aView }
    , mVkMesh{aMesh}
  {
    mMesh = mVkMesh->mMesh;

    Create();

    CreateShader();

    mView->RegisterEvent<&VkInstantiatedModel::SurfaceLostEvent>(Events::SurfaceLost, this);
    mView->RegisterEvent<&VkInstantiatedModel::SurfaceGainedEvent>(Events::SurfaceGained, this);
  }

  VkInstantiatedModel::~VkInstantiatedModel()
  {
    mSurface->DestroyModel(mView, this);
  }

  void VkInstantiatedModel::SurfaceLostEvent(ViewChanged *aEvent)
  {
    UnusedArguments(aEvent);
    mSurface->DestroyModel(mView, this);
  }

  void VkInstantiatedModel::SurfaceGainedEvent(ViewChanged *aEvent)
  {
    mView = aEvent->View;
    mSurface = static_cast<VkRenderer*>(mView->GetRenderer())->GetSurface(mView->GetWindow());
    CreateShader();
    mSurface->AddModel(this);
  }

  void VkInstantiatedModel::CreateShader()
  {
    mPipelineData.clear();

    // create descriptor sets
    for (auto[submeshIt, i] : enumerate(mVkMesh->mSubmeshes))
    {
      auto& submesh = *submeshIt;

      submesh->CreateShader(mView);

      CreateDescriptorSet(submesh.get(), i);
    }
  }

  void VkInstantiatedModel::UpdateMesh(size_t aIndex, 
                                       std::vector<Vertex>& aVertices)
  {
    mVkMesh->UpdateVertices(aIndex, aVertices);
  }

  void VkInstantiatedModel::UpdateMesh(size_t aIndex, 
                                       std::vector<Vertex>& aVertices,
                                       std::vector<u32>& aIndices)
  {
    mVkMesh->UpdateVerticesAndIndices(aIndex, aVertices, aIndices);
  }

  void VkInstantiatedModel::CreateDescriptorSet(VkSubmesh *aSubMesh, size_t aIndex)
  {
    auto& modelBuffer = static_cast<VkUBO*>(&mModelUBO.GetBase())->GetBuffer();
    auto& animationBuffer = static_cast<VkUBO*>(&mAnimationUBO.GetBase())->GetBuffer();
    auto& modelMaterialBuffer = static_cast<VkUBO*>(&mModelMaterialUBO.GetBase())->GetBuffer();
    auto& submeshMaterialBuffer = static_cast<VkUBO*>(&mSubmeshMaterialsUBO[aIndex].first.GetBase())->GetBuffer();
    mPipelineData.emplace(aSubMesh, 
                          aSubMesh->CreatePipelineData(modelBuffer,
                                                       animationBuffer,
                                                       modelMaterialBuffer,
                                                       submeshMaterialBuffer,
                                                       mView));
  }
}

