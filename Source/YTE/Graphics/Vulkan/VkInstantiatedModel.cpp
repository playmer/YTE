///////////////////
// Author: Andrew Griffin
// YTE - Graphics - Vulkan
///////////////////

#include "YTE/Graphics/Vulkan/VkInstantiatedModel.hpp"
#include "YTE/Graphics/Vulkan/VkMesh.hpp"
#include "YTE/Graphics/Vulkan/VkRenderedSurface.hpp"
#include "YTE/Graphics/Vulkan/VkTexture.hpp"
#include "YTE/Graphics/Vulkan/VkUtilities.hpp"

namespace YTE
{
  YTEDefineType(VkInstantiatedModel)
  {
    YTERegisterType(VkInstantiatedModel);
  }


  VkInstantiatedModel::VkInstantiatedModel(std::string &aModelFile,
                                           std::shared_ptr<VkRenderedSurface> aSurface)
    : InstantiatedModel()
    , mSurface(aSurface)
  {
    mLoadedMesh = mSurface->CreateMesh(aModelFile);
    mMesh = std::dynamic_pointer_cast<Mesh>(mLoadedMesh);

    // create UBO Per Model buffer
    auto allocator = mSurface->GetAllocator(AllocatorTypes::UniformBufferObject);

    mUBOPerModel = mSurface->GetDevice()->createBuffer(sizeof(UBOPerModel),
      vk::BufferUsageFlagBits::eTransferDst |
      vk::BufferUsageFlagBits::eUniformBuffer,
      vk::SharingMode::eExclusive,
      nullptr,
      vk::MemoryPropertyFlagBits::eDeviceLocal,
      allocator);

    // create descriptor sets
    for (auto& mesh : mLoadedMesh->mSubmeshes)
    {
      CreateDescriptorSet(mesh);
    }

    UBOPerModel modelData;
    modelData.mModelMatrix = glm::mat4(1.0f);

    VkInstantiatedModel::UpdateUBOPerModel(modelData);
  }



  VkInstantiatedModel::~VkInstantiatedModel()
  {

  }



  void VkInstantiatedModel::UpdateUBOPerModel(UBOPerModel &aUBO)
  {
    mSurface->YTERegister(Events::GraphicsDataUpdateVk, this,
      &VkInstantiatedModel::GraphicsDataUpdateVk);

    mUBOPerModelData = aUBO;
  }



  void VkInstantiatedModel::CreateDescriptorSet(std::shared_ptr<VkSubmesh> &mesh)
  {
    auto device = mSurface->GetDevice();



    // load Textures
    u32 samplers = 0;

    if (nullptr != mesh->mDiffuseTexture)
    {
      ++samplers;
    }
    if (nullptr != mesh->mSpecularTexture)
    {
      ++samplers;
    }
    if (nullptr != mesh->mNormalTexture)
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
        dslbs.emplace_back(i + 3,
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
        { { vk::DescriptorType::eUniformBuffer, 3 },
        });
    }

    auto descriptorSetLayout = device->createDescriptorSetLayout(dslbs);

    mPipelineLayouts.emplace(mesh, device->createPipelineLayout(descriptorSetLayout, nullptr));



    // init descriptor set
    mDescriptorSets.emplace(mesh, device->allocateDescriptorSet(descriptorPool,
      descriptorSetLayout));
    std::vector<vkhlf::WriteDescriptorSet> wdss;
    wdss.reserve(6);

    // Helper constants and variables.
    constexpr auto unibuf = vk::DescriptorType::eUniformBuffer;
    auto &ds = mDescriptorSets[mesh];
    u32 binding = 0;

    // Add Uniform Buffers
    vkhlf::DescriptorBufferInfo uboView{ mSurface->GetUBOViewBuffer(), 0, sizeof(UBOView) };
    vkhlf::DescriptorBufferInfo uboModel{ mUBOPerModel, 0, sizeof(UBOPerModel) };
    vkhlf::DescriptorBufferInfo uboMaterial{ mesh->mUBOMaterial, 0, sizeof(UBOMaterial) };

    wdss.emplace_back(ds, binding++, 0, 1, unibuf, nullptr, uboView);
    wdss.emplace_back(ds, binding++, 0, 1, unibuf, nullptr, uboModel);
    wdss.emplace_back(ds, binding++, 0, 1, unibuf, nullptr, uboMaterial);

    // Add Texture Samplers
    auto addTS = [&wdss, &binding, &ds](std::shared_ptr<VkTexture> aData, vkhlf::DescriptorImageInfo &imageInfo)
    {
      constexpr auto imgsam = vk::DescriptorType::eCombinedImageSampler;

      if (nullptr == aData)
      {
        return;
      }

      imageInfo.sampler = aData->mSampler;
      imageInfo.imageView = aData->mImageView;
      wdss.emplace_back(ds, binding++, 0, 1, imgsam, imageInfo, nullptr);
    };

    vkhlf::DescriptorImageInfo dTexInfo{ nullptr, nullptr, vk::ImageLayout::eGeneral };
    vkhlf::DescriptorImageInfo sTexInfo{ nullptr, nullptr, vk::ImageLayout::eGeneral };
    vkhlf::DescriptorImageInfo nTexInfo{ nullptr, nullptr, vk::ImageLayout::eGeneral };

    addTS(mesh->mDiffuseTexture, dTexInfo);
    addTS(mesh->mSpecularTexture, sTexInfo);
    addTS(mesh->mNormalTexture, nTexInfo);

    device->updateDescriptorSets(wdss, nullptr);
  }



  void VkInstantiatedModel::GraphicsDataUpdateVk(YTE::GraphicsDataUpdateVk *aEvent)
  {
    mSurface->YTEDeregister(Events::GraphicsDataUpdateVk, this,
      &VkInstantiatedModel::GraphicsDataUpdateVk);

    auto update = aEvent->mCBO;

    mUBOPerModel->update<UBOPerModel>(0, mUBOPerModelData, update);
  }
}

