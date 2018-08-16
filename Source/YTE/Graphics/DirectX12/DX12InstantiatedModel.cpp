///////////////////
// Author: Andrew Griffin
// YTE - Graphics - Vulkan
///////////////////

#include "YTE/Graphics/DirectX12/DX12VkInstantiatedModel.hpp"
#include "YTE/Graphics/DirectX12/DX12VkMesh.hpp"
#include "YTE/Graphics/DirectX12/DX12Dx12Renderer.hpp"
#include "YTE/Graphics/DirectX12/DX12Dx12RenderedSurface.hpp"
#include "YTE/Graphics/DirectX12/DX12VkTexture.hpp"
#include "YTE/Graphics/DirectX12/DX12VkDeviceInfo.hpp"

namespace YTE
{
  YTEDefineType(VkInstantiatedModel)
  {
    RegisterType<VkInstantiatedModel>();
    TypeBuilder<VkInstantiatedModel> builder;
  }

  UBOAnimation VkInstantiatedModel::cAnimation;

  VkInstantiatedModel::VkInstantiatedModel(std::string &aModelFile,
                                           Dx12RenderedSurface *aSurface,
                                           GraphicsView *aView)
    : InstantiatedModel{}
    , mSurface{aSurface}
    , mView{ aView }
    , mLoadUBOAnimation{ false }
    , mLoadUBOModel{ false }
    , mLoadUBOMaterial{ false }
  {
    mVkMesh = mSurface->GetRenderer()->CreateMesh(aModelFile);
    mMesh = mVkMesh->mMesh;
    Create();

    mView->RegisterEvent<&VkInstantiatedModel::SurfaceLostEvent>(Events::SurfaceLost, this);
    mView->RegisterEvent<&VkInstantiatedModel::SurfaceGainedEvent>(Events::SurfaceGained, this);
  }

  VkInstantiatedModel::VkInstantiatedModel(VkMesh *aMesh, 
                                           Dx12RenderedSurface *aSurface,
                                           GraphicsView *aView)
    : InstantiatedModel{}
    , mSurface{ aSurface }
    , mView{ aView }
    , mLoadUBOAnimation{false}
    , mLoadUBOModel{ false }
    , mLoadUBOMaterial{ false }
    , mVkMesh{aMesh}
  {
    mMesh = aMesh->mMesh;
    Create();

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
    mSurface = static_cast<Dx12Renderer*>(mView->GetRenderer())->GetSurface(mView->GetWindow());
    CreateShader();
    mSurface->AddModel(this);
  }

  void VkInstantiatedModel::Create()
  {
    auto allocator = mSurface->GetAllocator(AllocatorTypes::UniformBufferObject);
    auto &device = mSurface->GetDevice();

    if (false == mVkMesh->GetInstanced())
    {
      // create UBO Per Model buffer
      mUBOModel = device->createBuffer(sizeof(UBOModel),
                                       vk::BufferUsageFlagBits::eTransferDst |
                                       vk::BufferUsageFlagBits::eUniformBuffer,
                                       vk::SharingMode::eExclusive,
                                       nullptr,
                                       vk::MemoryPropertyFlagBits::eDeviceLocal,
                                       allocator);
      
      //// create UBO Per Model buffer
      mUBOModelMaterial = device->createBuffer(sizeof(UBOMaterial),
                                               vk::BufferUsageFlagBits::eTransferDst |
                                               vk::BufferUsageFlagBits::eUniformBuffer,
                                               vk::SharingMode::eExclusive,
                                               nullptr,
                                               vk::MemoryPropertyFlagBits::eDeviceLocal,
                                               allocator);
    }

    // Create UBO Animation Buffer.
    mUBOAnimation = device->createBuffer(sizeof(UBOAnimation),
                                         vk::BufferUsageFlagBits::eTransferDst |
                                         vk::BufferUsageFlagBits::eUniformBuffer,
                                         vk::SharingMode::eExclusive,
                                         nullptr,
                                         vk::MemoryPropertyFlagBits::eDeviceLocal,
                                         allocator);

    UpdateUBOAnimation(&cAnimation);

    UBOMaterial modelMaterial{};
    modelMaterial.mDiffuse = glm::vec4{ 1.0f, 1.0f, 1.0f, 1.0f };
    modelMaterial.mAmbient = glm::vec4{ 1.0f, 1.0f, 1.0f, 1.0f };
    modelMaterial.mSpecular = glm::vec4{ 0.0f, 0.0f, 0.0f, 1.0f };
    modelMaterial.mEmissive = glm::vec4{ 0.0f, 0.0f, 0.0f, 1.0f };
    modelMaterial.mShininess = 1.0f;

    bool isEditorObject = false;
    auto name = mMesh->mName;

    if (name == "Move_X.fbx"   || name == "Move_Y.fbx"   || name == "Move_Z.fbx" ||
        name == "Scale_X.fbx"  || name == "Scale_Y.fbx"  || name == "Scale_Z.fbx" ||
        name == "Rotate_X.fbx" || name == "Rotate_Y.fbx" || name == "Rotate_Z.fbx")
    {
      isEditorObject = true;
    }

    modelMaterial.mFlags = isEditorObject ? (u32)UBOMaterialFlags::IsGizmo : 0;

    UpdateUBOMaterial(&modelMaterial);

    mUBOModelData.mModelMatrix = glm::mat4(1.0f);


    // create descriptor sets
    for (auto[submesh, i] : enumerate(mVkMesh->mSubmeshes))
    {
      auto materialUBO = device->createBuffer(sizeof(UBOMaterial),
                                              vk::BufferUsageFlagBits::eTransferDst |
                                              vk::BufferUsageFlagBits::eUniformBuffer,
                                              vk::SharingMode::eExclusive,
                                              nullptr,
                                              vk::MemoryPropertyFlagBits::eDeviceLocal,
                                              allocator);

      auto &material = submesh->second->mSubmesh->mUBOMaterial;

      mUBOSubmeshMaterials.emplace_back(materialUBO, material);

      UpdateUBOSubmeshMaterial(&material, i);
    }

    CreateShader();
  }

  void VkInstantiatedModel::CreateShader()
  {
    mPipelineData.clear();

    // create descriptor sets
    for (auto[submesh, i] : enumerate(mVkMesh->mSubmeshes))
    {
      submesh->second->CreateShader(mView);

      CreateDescriptorSet(submesh->second.get(), i);
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


  // TODO (Josh): Change the name to be more representative. (This is really 
  //              just telling the object to register to be updated)
  void VkInstantiatedModel::UpdateUBOModel()
  {
    if (!mLoadUBOAnimation && 
        !mLoadUBOModel && 
        !mLoadUBOMaterial)
    {
      mSurface->RegisterEvent<&VkInstantiatedModel::GraphicsDataUpdateVk>(Events::GraphicsDataUpdateVk, this);
    }
  }

  void VkInstantiatedModel::UpdateUBOModel(UBOModel &aUBO)
  {
    mUBOModelData = aUBO;

    UpdateUBOModel();

    mLoadUBOModel = true;
  }

  void VkInstantiatedModel::UpdateUBOAnimation(UBOAnimation *aUBO)
  {
    mUBOAnimationData = aUBO;

    UpdateUBOModel();

    mLoadUBOAnimation = true;
  }

  void VkInstantiatedModel::UpdateUBOMaterial(UBOMaterial *aUBO)
  {
    mUBOModelMaterialData = *aUBO;

    UpdateUBOModel();

    mLoadUBOMaterial = true;
  }

  void VkInstantiatedModel::UpdateUBOSubmeshMaterial(UBOMaterial *aUBO, size_t aIndex)
  {
    mUBOSubmeshMaterials[aIndex].second = *aUBO;

    UpdateUBOModel();

    mLoadUBOMaterial = true;
  }


  void VkInstantiatedModel::SetDefaultAnimationOffset()
  {
    mUBOAnimationData = mMesh->mSkeleton.GetDefaultOffsets();

    UpdateUBOModel();

    mLoadUBOAnimation = true;
  }

  void VkInstantiatedModel::CreateDescriptorSet(Dx12Submesh *aSubMesh, size_t aIndex)
  {
    mPipelineData.emplace(aSubMesh, 
                          aSubMesh->CreatePipelineData(mUBOModel, 
                                                       mUBOAnimation,
                                                       mUBOModelMaterial,
                                                       mUBOSubmeshMaterials[aIndex].first,
                                                       mView));
  }

  bool VkInstantiatedModel::GetInstanced()
  {
    return mVkMesh->GetInstanced();
  }

  void VkInstantiatedModel::SetInstanced(bool aInstanced)
  {
    if (mMesh->CanAnimate())
    {
      printf("Currently don't support animating, instanced meshes.");
      return;
    }

    if (mVkMesh->GetInstanced() == aInstanced)
    {
      return;
    }

    mVkMesh->SetInstanced(mMesh);
  }


  void VkInstantiatedModel::GraphicsDataUpdateVk(YTE::GraphicsDataUpdateVk *aEvent)
  {
    mSurface->DeregisterEvent<&VkInstantiatedModel::GraphicsDataUpdateVk>(Events::GraphicsDataUpdateVk, this);

    auto update = aEvent->mCBO;

    if (mLoadUBOMaterial)
    {
      // TODO: We're updating all materials here, which is unfortunate. Maybe update at the submesh level?
      for (auto material : mUBOSubmeshMaterials)
      {
        material.first->update<UBOMaterial>(0, material.second, update);
      }

      mUBOModelMaterial->update<UBOMaterial>(0, mUBOModelMaterialData, update);

      mLoadUBOMaterial = false;
    }

    if (mLoadUBOAnimation)
    {
      mUBOAnimation->update<UBOAnimation>(0, *mUBOAnimationData, update);
      mLoadUBOAnimation = false;
    }

    if (mLoadUBOModel)
    {
      if (mVkMesh->GetInstanced())
      {
        Instance instance(mUBOModelData);

        mVkMesh->mInstanceManager.InstanceBuffer()->update<Instance>(mVkMesh->GetOffset(this), instance, update);
      }
      else
      {
        mUBOModel->update<UBOModel>(0, mUBOModelData, update);
      }

      mLoadUBOModel = false;
    }
  }
}

