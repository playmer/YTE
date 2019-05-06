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
    : mDiffuseTexture{ nullptr }
    , mSpecularTexture{ nullptr }
    , mNormalTexture{ nullptr }
    , mRenderer{ aRenderer }
    , mMesh{ aMesh }
    , mSubmesh{ aSubmesh }
    , mPipelineInfo{nullptr}
    , mIndexCount{ 0 }
  {
    Create();
  }


  void VkSubmesh::Create()
  {
    mIndexCount = mSubmesh->mData.mIndexData.size();

    // Load Textures
    if (false == mSubmesh->mData.mDiffuseMap.empty())
    {
      mDiffuseTexture = mRenderer->CreateTexture(mSubmesh->mData.mDiffuseMap, Convert(mSubmesh->mData.mDiffuseType));
      mSamplerTypes.emplace_back("DIFFUSE");
    }
    if (false == mSubmesh->mData.mSpecularMap.empty())
    {
      mSpecularTexture = mRenderer->CreateTexture(mSubmesh->mData.mSpecularMap, Convert(mSubmesh->mData.mSpecularType));
      mSamplerTypes.emplace_back("SPECULAR");
    }
    if (false == mSubmesh->mData.mNormalMap.empty())
    {
      mNormalTexture = mRenderer->CreateTexture(mSubmesh->mData.mNormalMap, Convert(mSubmesh->mData.mNormalType));
      mSamplerTypes.emplace_back("NORMAL");
    }
  }

  VkShaderDescriptions VkSubmesh::CreateShaderDescriptions()
  {
    u32 binding{ 0 };
    VkShaderDescriptions descriptions;

    auto addUBO = [&descriptions, &binding](char const* aName, vk::DescriptorType aDescriptorType, vk::ShaderStageFlagBits aStage, size_t aBufferSize, size_t aBufferOffset = 0)
    {
      descriptions.AddPreludeLine(fmt::format("#define UBO_{}_BINDING {}", aName, binding++));
      descriptions.AddDescriptor(aDescriptorType, aStage, aBufferSize, aBufferOffset);
    };

    addUBO("VIEW", vk::DescriptorType::eUniformBuffer, vk::ShaderStageFlagBits::eVertex, sizeof(UBOs::View));
    addUBO("ANIMATION_BONE", vk::DescriptorType::eUniformBuffer, vk::ShaderStageFlagBits::eVertex, sizeof(UBOs::Animation));
    addUBO("MODEL_MATERIAL", vk::DescriptorType::eUniformBuffer, vk::ShaderStageFlagBits::eFragment, sizeof(UBOs::Material));
    addUBO("SUBMESH_MATERIAL", vk::DescriptorType::eUniformBuffer, vk::ShaderStageFlagBits::eFragment, sizeof(UBOs::Material));
    addUBO("LIGHTS", vk::DescriptorType::eUniformBuffer, vk::ShaderStageFlagBits::eFragment, sizeof(UBOs::LightManager));
    addUBO("ILLUMINATION", vk::DescriptorType::eUniformBuffer, vk::ShaderStageFlagBits::eFragment, sizeof(UBOs::Illumination));
    addUBO("WATER", vk::DescriptorType::eUniformBuffer, vk::ShaderStageFlagBits::eVertex, sizeof(UBOs::WaterInformationManager));


    // Descriptions for the textures we support based on which maps we found above:
    for (auto sampler : mSamplerTypes)
    {
      descriptions.AddPreludeLine(fmt::format("#define UBO_{}_BINDING {}", sampler, binding++));
      descriptions.AddTextureDescriptor(vk::DescriptorType::eCombinedImageSampler, vk::ImageLayout::eShaderReadOnlyOptimal, vk::ShaderStageFlagBits::eFragment);
    }

    descriptions.AddBinding<Vertex>(vk::VertexInputRate::eVertex);
    descriptions.AddAttribute<glm::vec3>(vk::Format::eR32G32B32Sfloat);    //glm::vec3 mPosition;
    descriptions.AddAttribute<glm::vec3>(vk::Format::eR32G32B32Sfloat);    //glm::vec3 mTextureCoordinates;
    descriptions.AddAttribute<glm::vec3>(vk::Format::eR32G32B32Sfloat);    //glm::vec3 mNormal;
    descriptions.AddAttribute<glm::vec4>(vk::Format::eR32G32B32A32Sfloat); //glm::vec4 mColor;
    descriptions.AddAttribute<glm::vec3>(vk::Format::eR32G32B32Sfloat);    //glm::vec3 mTangent;
    descriptions.AddAttribute<glm::vec3>(vk::Format::eR32G32B32Sfloat);    //glm::vec3 mBinormal;
    descriptions.AddAttribute<glm::vec3>(vk::Format::eR32G32B32Sfloat);    //glm::vec3 mBitangent;
    descriptions.AddAttribute<glm::vec3>(vk::Format::eR32G32B32Sfloat);    //glm::vec4 mBoneWeights;
    descriptions.AddAttribute<glm::vec2>(vk::Format::eR32G32Sfloat);       //glm::vec2 mBoneWeights2;
    descriptions.AddAttribute<glm::ivec3>(vk::Format::eR32G32B32Sint);     //glm::ivec4 mBoneIDs;
    descriptions.AddAttribute<glm::ivec2>(vk::Format::eR32G32Sint);        //glm::ivec4 mBoneIDs;

    // Model Buffer for Vertex shader. (Non-instanced Meshes)
    addUBO("MODEL", vk::DescriptorType::eUniformBuffer, vk::ShaderStageFlagBits::eVertex, sizeof(UBOs::Model));

    return descriptions;
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

    descriptions = CreateShaderDescriptions();

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
    bufferOrImages.emplace_back(GetBuffer(surface->GetLightManager(aView)->GetUBOLightBuffer()));
    bufferOrImages.emplace_back(GetBuffer(surface->GetUBOIlluminationBuffer(aView)));
    bufferOrImages.emplace_back(GetBuffer(surface->GetWaterInfluenceMapManager(aView)->GetUBOMapBuffer()));

    // Add Texture Samplers
    auto addTS = [&bufferOrImages](VkTexture *aData)
    {
      if (nullptr == aData)
      {
        return;
      }

      vkhlf::DescriptorImageInfo textureInfo{ 
        aData->mSampler, 
        aData->mImageView, 
        vk::ImageLayout::eShaderReadOnlyOptimal };

      bufferOrImages.emplace_back(textureInfo);
    };

    addTS(mDiffuseTexture);
    addTS(mSpecularTexture);
    addTS(mNormalTexture);

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
