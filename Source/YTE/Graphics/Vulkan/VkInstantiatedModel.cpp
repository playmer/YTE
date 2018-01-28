///////////////////
// Author: Andrew Griffin
// YTE - Graphics - Vulkan
///////////////////

#include "YTE/Graphics/Vulkan/VkInstantiatedModel.hpp"
#include "YTE/Graphics/Vulkan/VkMesh.hpp"
#include "YTE/Graphics/Vulkan/VkRenderedSurface.hpp"
#include "YTE/Graphics/Vulkan/VkTexture.hpp"
#include "YTE/Graphics/Vulkan/VkDeviceInfo.hpp"

namespace YTE
{
  YTEDefineType(VkInstantiatedModel)
  {
    YTERegisterType(VkInstantiatedModel);
  }

  VkInstantiatedModel::VkInstantiatedModel(std::string &aModelFile,
                                           VkRenderedSurface *aSurface,
                                           GraphicsView *aView)
    : InstantiatedModel()
    , mSurface(aSurface)
    , mView(aView)
    , mLoadUBOAnimation(false)
    , mLoadUBOModel(false)
    , mLoadUBOMaterial(false)
  {
    mMesh = mSurface->CreateMesh(aModelFile);
    Create();
  }

  VkInstantiatedModel::VkInstantiatedModel(Mesh *aMesh, 
                                           VkRenderedSurface *aSurface,
                                           GraphicsView *aView)
    : InstantiatedModel()
    , mSurface(aSurface)
    , mView(aView)
    , mLoadUBOAnimation(false)
    , mLoadUBOModel(false)
  {
    mMesh = static_cast<VkMesh*>(aMesh);
    Create();
  }

  VkInstantiatedModel::~VkInstantiatedModel()
  {
    mSurface->DestroyModel(mView, this);
  }

  void VkInstantiatedModel::Create()
  {
    auto mesh = static_cast<VkMesh*>(mMesh);

    auto allocator = mSurface->GetAllocator(AllocatorTypes::UniformBufferObject);
    auto &device = mSurface->GetDevice();

    if (false == mesh->GetInstanced())
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
      //mUBOModelMaterial = device->createBuffer(sizeof(UBOMaterial),
      //                                         vk::BufferUsageFlagBits::eTransferDst |
      //                                         vk::BufferUsageFlagBits::eUniformBuffer,
      //                                         vk::SharingMode::eExclusive,
      //                                         nullptr,
      //                                         vk::MemoryPropertyFlagBits::eDeviceLocal,
      //                                         allocator);
    }

    // Create UBO Animation Buffer.
    mUBOAnimation = device->createBuffer(sizeof(UBOAnimation),
                                         vk::BufferUsageFlagBits::eTransferDst |
                                         vk::BufferUsageFlagBits::eUniformBuffer,
                                         vk::SharingMode::eExclusive,
                                         nullptr,
                                         vk::MemoryPropertyFlagBits::eDeviceLocal,
                                         allocator);

    // create descriptor sets
    for (auto [submesh, i] : enumerate(mesh->mSubmeshes))
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

      CreateDescriptorSet(submesh->second.get(), i);
    }

    mUBOModelData.mModelMatrix = glm::mat4(1.0f);
  }

  // TODO (Josh): Change the name to be more representative. (This is really 
  //              just telling the object to register to be updated)
  void VkInstantiatedModel::UpdateUBOModel()
  {
    if (!mLoadUBOAnimation && 
        !mLoadUBOModel && 
        !mLoadUBOMaterial)
    {
      mSurface->YTERegister(Events::GraphicsDataUpdateVk,
                            this,
                            &VkInstantiatedModel::GraphicsDataUpdateVk);
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

  void VkInstantiatedModel::CreateDescriptorSet(VkSubmesh *aSubMesh, size_t aIndex)
  {
    mPipelineData.emplace(aSubMesh, 
                          aSubMesh->CreatePipelineData(mUBOModel, 
                                                       mUBOAnimation,
                                                       mUBOSubmeshMaterials[aIndex].first,
                                                       mView));
  }

  bool VkInstantiatedModel::GetInstanced()
  {
    auto mesh = static_cast<VkMesh*>(mMesh);

    return mesh->GetInstanced();
  }

  void VkInstantiatedModel::SetInstanced(bool aInstanced)
  {
    auto mesh = static_cast<VkMesh*>(mMesh);

    if (mesh->CanAnimate())
    {
      printf("Currently don't support animating, instanced meshes.");
      return;
    }

    if (mesh->GetInstanced() == aInstanced)
    {
      return;
    }


    mesh->SetInstanced(mMesh);
  }


  void VkInstantiatedModel::GraphicsDataUpdateVk(YTE::GraphicsDataUpdateVk *aEvent)
  {
    auto mesh = static_cast<VkMesh*>(mMesh);

    mSurface->YTEDeregister(Events::GraphicsDataUpdateVk,
                            this,
                            &VkInstantiatedModel::GraphicsDataUpdateVk);

    auto update = aEvent->mCBO;

    if (mLoadUBOMaterial)
    {
      // TODO: We're updating all materials here, which is unfortunate. Maybe update at the submesh level?
      for (auto material : mUBOSubmeshMaterials)
      {
        material.first->update<UBOMaterial>(0, material.second, update);
      }

      mLoadUBOMaterial = false;
    }

    if (mLoadUBOAnimation)
    {
      mUBOAnimation->update<UBOAnimation>(0, *mUBOAnimationData, update);
      mLoadUBOAnimation = false;
    }

    if (mLoadUBOModel)
    {
      if (mesh->GetInstanced())
      {
        Instance instance(mUBOModelData);

        mesh->mInstanceManager.InstanceBuffer()->update<Instance>(mesh->GetOffset(this), instance, update);
      }
      else
      {
        mUBOModel->update<UBOModel>(0, mUBOModelData, update);
      }

      mLoadUBOModel = false;
    }
  }
}

