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
    OPTICK_EVENT();

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
    OPTICK_EVENT();

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
    mBuffers.clear();
    mSurface->DestroyModel(mView, this);
  }

  void VkInstantiatedModel::SurfaceGainedEvent(ViewChanged *aEvent)
  {
    OPTICK_EVENT();

    mView = aEvent->View;
    mSurface = static_cast<VkRenderer*>(mView->GetRenderer())->GetSurface(mView->GetWindow());
    CreateShader();
    mSurface->AddModel(this);
  }

  void VkInstantiatedModel::CreateShader()
  {
    OPTICK_EVENT();

    mPipelineData.clear();

    // create descriptor sets
    for (auto[submeshIt, i] : enumerate(mVkMesh->mSubmeshes))
    {
      auto& submesh = *submeshIt;

      submesh->CreateShader(mView);

      CreateDescriptorSet(submesh.get(), i);
    }
  }

  void VkInstantiatedModel::CreateDescriptorSet(VkSubmesh *aSubMesh, size_t aIndex)
  {
    OPTICK_EVENT();

    if (0 == mBuffers.size())
    {
      AddBuffer(&mView->GetViewUBO());
      AddBuffer(&mAnimationUBO);
      AddBuffer(&mModelMaterialUBO);
      AddBuffer(&mSubmeshMaterialsUBO[aIndex].first);
      AddBuffer(&mView->GetLightManager()->GetUBOLightBuffer());
      AddBuffer(&mView->GetIlluminationUBO());
      AddBuffer(&mView->GetWaterInfluenceMapManager()->GetUBOMapBuffer());
      AddBuffer(&mModelUBO);
    }

    mPipelineData.emplace(aSubMesh, aSubMesh->CreatePipelineData(this, mView));
  }
}

