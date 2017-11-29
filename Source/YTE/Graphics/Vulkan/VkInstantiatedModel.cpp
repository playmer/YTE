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
                                           VkRenderedSurface *aSurface)
    : InstantiatedModel()
    , mSurface(aSurface)
    , mLoadUBOAnimation(false)
    , mLoadUBOModel(false)
  {
    mLoadedMesh = mSurface->CreateMesh(aModelFile);
    Create();
  }

  VkInstantiatedModel::VkInstantiatedModel(Mesh *aMesh, VkRenderedSurface *aSurface)
    : InstantiatedModel()
    , mSurface(aSurface)
  {
    mLoadedMesh = static_cast<VkMesh*>(aMesh);
    Create();
  }

  VkInstantiatedModel::~VkInstantiatedModel()
  {
    mSurface->DestroyModel(this);
  }

  void VkInstantiatedModel::Create()
  {
    mMesh = static_cast<Mesh*>(mLoadedMesh);

    // create UBO Per Model buffer
    auto allocator = mSurface->GetAllocator(AllocatorTypes::UniformBufferObject);

    mUBOModel = mSurface->GetDevice()->createBuffer(sizeof(UBOModel),
                                                    vk::BufferUsageFlagBits::eTransferDst |
                                                    vk::BufferUsageFlagBits::eUniformBuffer,
                                                    vk::SharingMode::eExclusive,
                                                    nullptr,
                                                    vk::MemoryPropertyFlagBits::eDeviceLocal,
                                                    allocator);

    mUBOAnimation = mSurface->GetDevice()->createBuffer(sizeof(UBOAnimation),
                                                        vk::BufferUsageFlagBits::eTransferDst |
                                                        vk::BufferUsageFlagBits::eUniformBuffer,
                                                        vk::SharingMode::eExclusive,
                                                        nullptr,
                                                        vk::MemoryPropertyFlagBits::eDeviceLocal,
                                                        allocator);

    // create descriptor sets
    for (auto& submesh : mLoadedMesh->mSubmeshes)
    {
      CreateDescriptorSet(submesh.second.get());
    }

    mUBOModelData.mModelMatrix = glm::mat4(1.0f);
  }

  void VkInstantiatedModel::UpdateUBOModel(UBOModel &aUBO)
  {
    mUBOModelData = aUBO;
   
    if (!mLoadUBOModel)
    {
      if (!mLoadUBOAnimation)
      {
        mSurface->YTERegister(Events::GraphicsDataUpdateVk, this,
                              &VkInstantiatedModel::GraphicsDataUpdateVk);
      }

      mLoadUBOModel = true;
    }
  }


  
  void VkInstantiatedModel::UpdateUBOAnimation(UBOAnimation *aUBO)
  {
    mUBOAnimationData = aUBO;
   
    if (!mLoadUBOAnimation)
    {
      if (!mLoadUBOModel)
      {
        mSurface->YTERegister(Events::GraphicsDataUpdateVk, this,
                              &VkInstantiatedModel::GraphicsDataUpdateVk);
      }

      mLoadUBOAnimation = true;
    }
  }



  void VkInstantiatedModel::SetDefaultAnimationOffset()
  {
    mUBOAnimationData = mMesh->mSkeleton.GetDefaultOffsets();

    if (!mLoadUBOAnimation)
    {
      if (!mLoadUBOModel)
      {
        mSurface->YTERegister(Events::AnimationUpdateVk, this,
                              &VkInstantiatedModel::GraphicsDataUpdateVk);
      }

      mLoadUBOAnimation = true;
    }
  }



  void VkInstantiatedModel::CreateDescriptorSet(VkSubmesh *aSubMesh)
  {
    auto device = mSurface->GetDevice();

    // load Textures
    u32 samplers = 0;

    if (nullptr != aSubMesh->mDiffuseTexture)
    {
      ++samplers;
    }
    if (nullptr != aSubMesh->mSpecularTexture)
    {
      ++samplers;
    }
    if (nullptr != aSubMesh->mNormalTexture)
    {
      ++samplers;
    }

    // init descriptor and pipeline layouts
    std::vector<vkhlf::DescriptorSetLayoutBinding> dslbs;
    dslbs.emplace_back(0,
                       vk::DescriptorType::eUniformBuffer,
                       vk::ShaderStageFlagBits::eVertex,
                       nullptr);
    dslbs.emplace_back(1,
                       vk::DescriptorType::eUniformBuffer,
                       vk::ShaderStageFlagBits::eVertex,
                       nullptr);
    dslbs.emplace_back(2,
                       vk::DescriptorType::eUniformBuffer,
                       vk::ShaderStageFlagBits::eVertex,
                       nullptr);
    dslbs.emplace_back(3,
                       vk::DescriptorType::eUniformBuffer,
                       vk::ShaderStageFlagBits::eFragment,
                       nullptr);

    std::shared_ptr<vkhlf::DescriptorPool> descriptorPool;

    if (0 != samplers)
    {
      std::vector<vk::DescriptorPoolSize> descriptorTypes;
      descriptorTypes.emplace_back(vk::DescriptorType::eUniformBuffer, 1);
      descriptorTypes.emplace_back(vk::DescriptorType::eUniformBuffer, 1);
      descriptorTypes.emplace_back(vk::DescriptorType::eUniformBuffer, 1);

      for (u32 i = 0; i < samplers; ++i)
      {
        dslbs.emplace_back(i + 4,
                           vk::DescriptorType::eCombinedImageSampler,
                           vk::ShaderStageFlagBits::eFragment,
                           nullptr);

        descriptorTypes.emplace_back(vk::DescriptorType::eCombinedImageSampler, 1);
      }

      descriptorPool = device->createDescriptorPool({}, 1, descriptorTypes);
    }
    else
    {
      descriptorPool = device->createDescriptorPool({},
        1,
        { { vk::DescriptorType::eUniformBuffer, 4 },
        });
    }

    auto descriptorSetLayout = device->createDescriptorSetLayout(dslbs);

    SubMeshPipelineData pipelineData;
    pipelineData.mPipelineLayout = device->createPipelineLayout(descriptorSetLayout, nullptr);
    pipelineData.mDescriptorSet = device->allocateDescriptorSet(descriptorPool, descriptorSetLayout);

    mPipelineData.emplace(aSubMesh, pipelineData);

    std::vector<vkhlf::WriteDescriptorSet> wdss;
    wdss.reserve(7);

    // Helper constants and variables.
    constexpr auto unibuf = vk::DescriptorType::eUniformBuffer;
    auto &ds = pipelineData.mDescriptorSet;
    u32 binding = 0;

    // Add Uniform Buffers
    vkhlf::DescriptorBufferInfo uboView{ mSurface->GetUBOViewBuffer(), 0, sizeof(UBOView) };
    vkhlf::DescriptorBufferInfo uboModel{ mUBOModel, 0, sizeof(UBOModel) };
    vkhlf::DescriptorBufferInfo uboAnimation{ mUBOAnimation, 0, sizeof(UBOAnimation) };
    vkhlf::DescriptorBufferInfo uboMaterial{ aSubMesh->mUBOMaterial, 0, sizeof(UBOMaterial) };

    wdss.emplace_back(ds, binding++, 0, 1, unibuf, nullptr, uboView);
    wdss.emplace_back(ds, binding++, 0, 1, unibuf, nullptr, uboModel);
    wdss.emplace_back(ds, binding++, 0, 1, unibuf, nullptr, uboAnimation);
    wdss.emplace_back(ds, binding++, 0, 1, unibuf, nullptr, uboMaterial);

    // Add Texture Samplers
    auto addTS = [&wdss, &binding, &ds](VkTexture *aData, 
                                        vkhlf::DescriptorImageInfo &aImageInfo)
    {
      constexpr auto imgsam = vk::DescriptorType::eCombinedImageSampler;

      if (nullptr == aData)
      {
        return;
      }

      aImageInfo.sampler = aData->mSampler;
      aImageInfo.imageView = aData->mImageView;
      wdss.emplace_back(ds, binding++, 0, 1, imgsam, aImageInfo, nullptr);
    };

    vkhlf::DescriptorImageInfo dTexInfo{ nullptr, nullptr, vk::ImageLayout::eGeneral };
    vkhlf::DescriptorImageInfo sTexInfo{ nullptr, nullptr, vk::ImageLayout::eGeneral };
    vkhlf::DescriptorImageInfo nTexInfo{ nullptr, nullptr, vk::ImageLayout::eGeneral };

    addTS(aSubMesh->mDiffuseTexture, dTexInfo);
    addTS(aSubMesh->mSpecularTexture, sTexInfo);
    addTS(aSubMesh->mNormalTexture, nTexInfo);

    device->updateDescriptorSets(wdss, nullptr);
  }



  void VkInstantiatedModel::GraphicsDataUpdateVk(YTE::GraphicsDataUpdateVk *aEvent)
  {
    mSurface->YTEDeregister(Events::GraphicsDataUpdateVk, this,
      &VkInstantiatedModel::GraphicsDataUpdateVk);

    auto update = aEvent->mCBO;

    if (mLoadUBOModel)
    {
      mUBOModel->update<UBOModel>(0, mUBOModelData, update);
      mLoadUBOModel = false;
    }

    if (mLoadUBOAnimation)
    {
      mUBOAnimation->update<UBOAnimation>(0, *mUBOAnimationData, update);
      mLoadUBOAnimation = false;
    }
  }
}

