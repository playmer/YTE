#include "fmt/format.h"

#include "YTE/Graphics/Vulkan/VkDeviceInfo.hpp"
#include "YTE/Graphics/Vulkan/VkInstantiatedModel.hpp"
#include "YTE/Graphics/Vulkan/VkMesh.hpp"
#include "YTE/Graphics/Vulkan/VkRenderer.hpp"
#include "YTE/Graphics/Vulkan/VkRenderedSurface.hpp"
#include "YTE/Graphics/Vulkan/VkShader.hpp"
#include "YTE/Graphics/Vulkan/VkTexture.hpp"
#include "YTE/Graphics/Vulkan/VkLightManager.hpp"

namespace YTE
{
  vk::ImageViewType Convert(TextureViewType aType)
  {
    switch (aType)
    {
      case TextureViewType::e2D:
      {
        return vk::ImageViewType::e2D;
      }
      case TextureViewType::eCube:
      {
        return vk::ImageViewType::eCube;
      }
    }

    return vk::ImageViewType{};
  }
  
  ///////////////////////////////////////////////////////////////////////////
  // Submesh
  ///////////////////////////////////////////////////////////////////////////
  YTEDefineType(VkSubmesh)
  {
    RegisterType<VkSubmesh>();
    TypeBuilder<VkSubmesh> builder;
  }

  VkSubmesh::VkSubmesh(VkMesh *aMesh, Submesh *aSubmesh, VkRenderer *aRenderer)
    : mRenderer{ aRenderer }
    , mMesh{ aMesh }
    , mSubmesh{ aSubmesh }
    , mPipelineInfo{nullptr}
  {
    Create();
  }


  void VkSubmesh::Create()
  {
    // Load Textures
    for (auto const& texture : mSubmesh->mData.mTextureData)
    {
      mTextures.emplace_back(mRenderer->CreateTexture(texture.mName, Convert(texture.mViewType)));
    }
  }

  void VkSubmesh::CreateShader(GraphicsView *aView)
  {
    auto device = mRenderer->mDevice;

    VkShaderDescriptions descriptions;

    mPipelineInfo = mRenderer->GetSurface(aView->GetWindow())->IfShaderExistsCreateOnView(mSubmesh->mData.mShaderSetName, aView);

    if (mPipelineInfo)
    {
      return;
    }

    descriptions = VkShaderDescriptions{ mSubmesh->CreateShaderDescriptions() };

    auto descriptorSetLayout = mRenderer->mDevice->createDescriptorSetLayout(descriptions.DescriptorSetLayout());
    auto pipelineLayout = device->createPipelineLayout(descriptorSetLayout, nullptr);
     
    // load shader passing our created pipeline layout
    mPipelineInfo = mRenderer->GetSurface(aView->GetWindow())->CreateShader(
      mSubmesh->mData.mShaderSetName,
      descriptorSetLayout,
      pipelineLayout,
      descriptions,
      aView);
  }


  std::shared_ptr<vkhlf::DescriptorPool> VkSubmesh::MakePool()
  {
    auto device = mRenderer->mDevice;

    // Create the descriptor set and pipeline layouts.
    mDescriptorTypes.emplace_back(
      vk::DescriptorType::eUniformBuffer, 
      mPipelineInfo->mDescriptions.CountDescriptorsOfType(vk::DescriptorType::eUniformBuffer));

    if (0 != mSamplerTypes.size())
    {
      mDescriptorTypes.emplace_back(vk::DescriptorType::eCombinedImageSampler, static_cast<u32>(mSamplerTypes.size()));
    }

    return device->createDescriptorPool({}, 1, mDescriptorTypes);
  }

  SubMeshPipelineData VkSubmesh::CreatePipelineData(std::shared_ptr<vkhlf::Buffer> &aUBOModel,
                                                    std::shared_ptr<vkhlf::Buffer> &aUBOAnimation,
                                                    std::shared_ptr<vkhlf::Buffer> &aUBOModelMaterial,
                                                    std::shared_ptr<vkhlf::Buffer> &aUBOSubmeshMaterial,
                                                    GraphicsView *aView)
  {
    auto device = mRenderer->mDevice;
    auto surface = mRenderer->GetSurface(aView->GetWindow());

    SubMeshPipelineData pipelineData;
    pipelineData.mPipelineLayout = mPipelineInfo->mPipelineLayout;
    pipelineData.mDescriptorSet = device->allocateDescriptorSet(
      MakePool(),
      mPipelineInfo->mDescriptorSetLayout);

    // Add Uniform Buffers
    std::vector<VkShaderDescriptions::BufferOrImage> bufferOrImages;

    bufferOrImages.emplace_back(GetBuffer(surface->GetUBOViewBuffer(aView)));
    bufferOrImages.emplace_back(aUBOAnimation);
    bufferOrImages.emplace_back(aUBOModelMaterial);
    bufferOrImages.emplace_back(aUBOSubmeshMaterial);
    bufferOrImages.emplace_back(GetBuffer(aView->GetLightManager()->GetUBOLightBuffer()));
    bufferOrImages.emplace_back(GetBuffer(surface->GetUBOIlluminationBuffer(aView)));
    bufferOrImages.emplace_back(GetBuffer(surface->GetWaterInfluenceMapManager(aView)->GetUBOMapBuffer()));

    for (auto texture : mTextures)
    {
      vkhlf::DescriptorImageInfo textureInfo{ 
        texture->mSampler,
        texture->mImageView,
        vk::ImageLayout::eShaderReadOnlyOptimal };

      bufferOrImages.emplace_back(textureInfo);
    }

    bufferOrImages.emplace_back(aUBOModel);
    auto writeDescriptorSets = mPipelineInfo->mDescriptions.MakeWriteDescriptorSet(
      &pipelineData.mDescriptorSet,
      bufferOrImages);

    device->updateDescriptorSets(writeDescriptorSets, nullptr);

    return pipelineData;
  }

  void VkSubmesh::Destroy()
  {

  }

  VkSubmesh::~VkSubmesh()
  {

  }


  ///////////////////////////////////////////////////////////////////////////
  // Mesh
  ///////////////////////////////////////////////////////////////////////////
  YTEDefineType(VkMesh)
  {
    RegisterType<VkMesh>();
    TypeBuilder<VkMesh> builder;
  }

  VkMesh::VkMesh(Mesh *aMesh,
                 VkRenderer *aRenderer)
    : mRenderer{ aRenderer }
    , mMesh{aMesh}
  {
    for (auto& part : aMesh->mParts)
    {
      auto submesh = std::make_unique<VkSubmesh>(this, &part, aRenderer);
      mSubmeshMap.emplace(submesh->mSubmesh->mData.mShaderSetName, submesh.get());
      mSubmeshes.emplace_back(std::move(submesh));
    }
  }

  VkMesh::~VkMesh()
  {
  }
}
