#include "fmt/format.h"

#include "YTE/Graphics/Vulkan/VkDeviceInfo.hpp"
#include "YTE/Graphics/Vulkan/VkInstantiatedModel.hpp"
#include "YTE/Graphics/Vulkan/VkMesh.hpp"
#include "YTE/Graphics/Vulkan/VkRenderer.hpp"
#include "YTE/Graphics/Vulkan/VkRenderedSurface.hpp"
#include "YTE/Graphics/Vulkan/VkShader.hpp"
#include "YTE/Graphics/Vulkan/VkTexture.hpp"

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

  SubMeshPipelineData VkSubmesh::CreatePipelineData(InstantiatedModel* aModel,
                                                    GraphicsView *aView)
  {
    UnusedArguments(aView);
    auto device = mRenderer->mDevice;


    SubMeshPipelineData pipelineData;
    pipelineData.mPipelineLayout = mPipelineInfo->mPipelineLayout;
    
    if (mDescriptorTypes.empty())
    {
      mDescriptorTypes.emplace_back(
        vk::DescriptorType::eUniformBuffer, 
        mPipelineInfo->mDescriptions.CountDescriptorsOfType(vk::DescriptorType::eUniformBuffer));

      if (0 != mSubmesh->mData.mTextureData.size())
      {
        mDescriptorTypes.emplace_back(vk::DescriptorType::eCombinedImageSampler, static_cast<u32>(mSubmesh->mData.mTextureData.size()));
      }
    }

    pipelineData.mDescriptorSet = mRenderer->GetSurface(aView->GetWindow())->mPoolManager.AllocateDescriptorSet(
      mDescriptorTypes, 
      mPipelineInfo->mDescriptorSetLayout);

    // Add Uniform Buffers
    std::vector<VkShaderDescriptions::BufferOrImage> bufferOrImages;

    for (auto& buffer : aModel->GetBuffers())
    {
      bufferOrImages.emplace_back(GetBuffer(buffer));
    }
    
    //bufferOrImages.emplace_back(GetBuffer(aView->GetViewUBO()));
    //bufferOrImages.emplace_back(aUBOAnimation);
    //bufferOrImages.emplace_back(aUBOModelMaterial);
    //bufferOrImages.emplace_back(aUBOSubmeshMaterial);
    //bufferOrImages.emplace_back(GetBuffer(aView->GetLightManager()->GetUBOLightBuffer()));
    //bufferOrImages.emplace_back(GetBuffer(aView->GetIlluminationUBO()));
    //bufferOrImages.emplace_back(GetBuffer(aView->GetWaterInfluenceMapManager()->GetUBOMapBuffer()));
    //bufferOrImages.emplace_back(aUBOModel);

    for (auto texture : mTextures)
    {
      vkhlf::DescriptorImageInfo textureInfo{ 
        texture->mSampler,
        texture->mImageView,
        vk::ImageLayout::eShaderReadOnlyOptimal };

      bufferOrImages.emplace_back(textureInfo);
    }

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
