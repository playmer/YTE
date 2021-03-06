///////////////////
// Author: Andrew Griffin
// YTE - Graphics - Vulkan
///////////////////
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
    : mDiffuseTexture(nullptr)
    , mSpecularTexture(nullptr)
    , mNormalTexture(nullptr)
    , mRenderer(aRenderer)
    , mMesh(aMesh)
    , mSubmesh(aSubmesh)
    , mIndexCount(0)
  {
    Create();
  }


  void VkSubmesh::Create()
  {
    // Shader Descriptions
    // TODO (Josh): We should be reflecting these.
    auto allocator = mRenderer->GetAllocator(AllocatorTypes::Mesh);

    auto device = mRenderer->mDevice;

    // Create Vertex, Index, and Material buffers.
    mVertexBuffer = allocator->CreateBuffer<Vertex>(mSubmesh->mVertexBuffer.size(),
                                                    GPUAllocation::BufferUsage::TransferDst |
                                                    GPUAllocation::BufferUsage::VertexBuffer,
                                                    GPUAllocation::MemoryProperty::DeviceLocal);

    mIndexBuffer = allocator->CreateBuffer<u32>(mSubmesh->mIndexBuffer.size(),
                                                GPUAllocation::BufferUsage::TransferDst |
                                                GPUAllocation::BufferUsage::IndexBuffer,
                                                GPUAllocation::MemoryProperty::DeviceLocal);

    mIndexCount = mSubmesh->mIndexBuffer.size();

    // Load Textures
    size_t samplers{ 0 };
    std::array<const char *, 3> samplerTypes;

    if (false == mSubmesh->mDiffuseMap.empty())
    {
      mDiffuseTexture = mRenderer->CreateTexture(mSubmesh->mDiffuseMap, Convert(mSubmesh->mDiffuseType));
      samplerTypes[samplers++] = "DIFFUSE";
    }
    if (false == mSubmesh->mSpecularMap.empty())
    {
      mSpecularTexture = mRenderer->CreateTexture(mSubmesh->mSpecularMap, Convert(mSubmesh->mSpecularType));
      samplerTypes[samplers++] = "SPECULAR";
    }
    if (false == mSubmesh->mNormalMap.empty())
    {
      mNormalTexture = mRenderer->CreateTexture(mSubmesh->mNormalMap, Convert(mSubmesh->mNormalType));
      samplerTypes[samplers++] = "NORMAL";
    }

    std::vector<vkhlf::DescriptorSetLayoutBinding> dslbs;
    u32 binding{ 0 };
    u32 uniformBuffers{ 0 };

    // View Buffer for Vertex shader.
    dslbs.emplace_back(binding,
                       vk::DescriptorType::eUniformBuffer,
                       vk::ShaderStageFlagBits::eVertex,
                       nullptr);
    mDescriptions.AddPreludeLine(fmt::format("#define UBO_VIEW_BINDING {}", binding));
    ++uniformBuffers;

    // Animation (Bone Array) Buffer for Vertex shader.
    dslbs.emplace_back(++binding,
                       vk::DescriptorType::eUniformBuffer,
                       vk::ShaderStageFlagBits::eVertex,
                       nullptr);
    mDescriptions.AddPreludeLine(fmt::format("#define UBO_ANIMATION_BONE_BINDING {}", binding));
    ++uniformBuffers;

    // Model Material Buffer for Fragment shader.
    dslbs.emplace_back(++binding,
                       vk::DescriptorType::eUniformBuffer,
                       vk::ShaderStageFlagBits::eFragment,
                       nullptr);
    mDescriptions.AddPreludeLine(fmt::format("#define UBO_MODEL_MATERIAL_BINDING {}", binding));
    ++uniformBuffers;
    
    // Submesh Material Buffer for Fragment shader.
    dslbs.emplace_back(++binding,
                       vk::DescriptorType::eUniformBuffer,
                       vk::ShaderStageFlagBits::eFragment,
                       nullptr);
    mDescriptions.AddPreludeLine(fmt::format("#define UBO_SUBMESH_MATERIAL_BINDING {}", binding));
    ++uniformBuffers;

    // Lights Buffer for Fragment shader.
    dslbs.emplace_back(++binding,
                       vk::DescriptorType::eUniformBuffer,
                       vk::ShaderStageFlagBits::eFragment,
                       nullptr);
    mDescriptions.AddPreludeLine(fmt::format("#define UBO_LIGHTS_BINDING {}", binding));
    ++uniformBuffers;

    // Illumination Buffer for Fragment shader.
    dslbs.emplace_back(++binding,
                       vk::DescriptorType::eUniformBuffer,
                       vk::ShaderStageFlagBits::eFragment,
                       nullptr);
    mDescriptions.AddPreludeLine(fmt::format("#define UBO_ILLUMINATION_BINDING {}", binding));
    ++uniformBuffers;

    // Water Information Buffer for Fragment shader.
    dslbs.emplace_back(++binding,
                       vk::DescriptorType::eUniformBuffer,
                       vk::ShaderStageFlagBits::eVertex,
                       nullptr);
    mDescriptions.AddPreludeLine(fmt::format("#define UBO_WATER_BINDING {}", binding));
    ++uniformBuffers;


    // Descriptions for the textures we support based on which maps we found above:
    //   Diffuse
    //   Specular
    //   Normal
    for (size_t i = 0; i < samplers; ++i)
    {
      dslbs.emplace_back(++binding,
                         vk::DescriptorType::eCombinedImageSampler,
                         vk::ShaderStageFlagBits::eFragment,
                         nullptr);
      mDescriptions.AddPreludeLine(fmt::format("#define UBO_{}_BINDING {}", samplerTypes[i], binding));
    }

    mDescriptions.AddBinding<Vertex>(vk::VertexInputRate::eVertex);
    mDescriptions.AddAttribute<glm::vec3>(vk::Format::eR32G32B32Sfloat);    //glm::vec3 mPosition;
    mDescriptions.AddAttribute<glm::vec3>(vk::Format::eR32G32B32Sfloat);    //glm::vec3 mTextureCoordinates;
    mDescriptions.AddAttribute<glm::vec3>(vk::Format::eR32G32B32Sfloat);    //glm::vec3 mNormal;
    mDescriptions.AddAttribute<glm::vec4>(vk::Format::eR32G32B32A32Sfloat); //glm::vec4 mColor;
    mDescriptions.AddAttribute<glm::vec3>(vk::Format::eR32G32B32Sfloat);    //glm::vec3 mTangent;
    mDescriptions.AddAttribute<glm::vec3>(vk::Format::eR32G32B32Sfloat);    //glm::vec3 mBinormal;
    mDescriptions.AddAttribute<glm::vec3>(vk::Format::eR32G32B32Sfloat);    //glm::vec3 mBitangent;
    mDescriptions.AddAttribute<glm::vec3>(vk::Format::eR32G32B32Sfloat);    //glm::vec4 mBoneWeights;
    mDescriptions.AddAttribute<glm::vec2>(vk::Format::eR32G32Sfloat);       //glm::vec2 mBoneWeights2;
    mDescriptions.AddAttribute<glm::ivec3>(vk::Format::eR32G32B32Sint);     //glm::ivec4 mBoneIDs;
    mDescriptions.AddAttribute<glm::ivec2>(vk::Format::eR32G32Sint);        //glm::ivec4 mBoneIDs;

    // Model Buffer for Vertex shader. (Non-instanced Meshes)
    // We do this one last so as to make the binding numbers easier
    // to set via #defines for the shader.
    dslbs.emplace_back(++binding,
                       vk::DescriptorType::eUniformBuffer,
                       vk::ShaderStageFlagBits::eVertex,
                       nullptr);
    ++uniformBuffers;

    // We need to tell the shaders where to find the UBOs::Model.
    mDescriptions.AddPreludeLine(fmt::format("#define UBO_MODEL_BINDING {}", binding));

    // Create the descriptor set and pipeline layouts.
    mDescriptorTypes.emplace_back(vk::DescriptorType::eUniformBuffer, uniformBuffers);

    if (0 != samplers)
    {
      mDescriptorTypes.emplace_back(vk::DescriptorType::eCombinedImageSampler, static_cast<u32>(samplers));
    }

    mDescriptorSetLayout = device->createDescriptorSetLayout(dslbs);
  }



  void VkSubmesh::CreateShader(GraphicsView *aView)
  {
    auto device = mRenderer->mDevice;
    auto pipelineLayout = device->createPipelineLayout(mDescriptorSetLayout, nullptr);
     
    // load shader passing our created pipeline layout
    mRenderer->GetSurface(aView->GetWindow())->CreateShader(mSubmesh->mShaderSetName,
                                               pipelineLayout,
                                               mDescriptions,
                                               aView);
  }


  std::shared_ptr<vkhlf::DescriptorPool> VkSubmesh::MakePool()
  {
    auto device = mRenderer->mDevice;

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
    pipelineData.mPipelineLayout = device->createPipelineLayout(mDescriptorSetLayout,
                                                                nullptr);
    pipelineData.mDescriptorSet = device->allocateDescriptorSet(MakePool(),
                                                                mDescriptorSetLayout);

    std::vector<vkhlf::WriteDescriptorSet> wdss;
    wdss.reserve(6);

    // Helper constants and variables.
    constexpr auto unibuf = vk::DescriptorType::eUniformBuffer;
    auto &ds = pipelineData.mDescriptorSet;
    u32 binding = 0;

    // Add Uniform Buffers

    // View Buffer for Vertex shader.
    vkhlf::DescriptorBufferInfo uboView{ GetBuffer(surface->GetUBOViewBuffer(aView)), 0, sizeof(UBOs::View) };
    wdss.emplace_back(ds, binding++, 0, 1, unibuf, nullptr, uboView);

    // Animation (Bone Array) Buffer for Vertex shader.
    vkhlf::DescriptorBufferInfo uboAnimation{ aUBOAnimation, 0, sizeof(UBOs::Animation) };
    wdss.emplace_back(ds, binding++, 0, 1, unibuf, nullptr, uboAnimation);

    // Model Material Buffer for Fragment shader.
    vkhlf::DescriptorBufferInfo uboModelMaterial{ aUBOModelMaterial, 0, sizeof(UBOs::Material) };
    wdss.emplace_back(ds, binding++, 0, 1, unibuf, nullptr, uboModelMaterial);

    // Submesh Material Buffer for Fragment shader.
    vkhlf::DescriptorBufferInfo uboSubmeshMaterial{ aUBOSubmeshMaterial, 0, sizeof(UBOs::Material) };
    wdss.emplace_back(ds, binding++, 0, 1, unibuf, nullptr, uboSubmeshMaterial);

    // Light manager Buffer for Fragment Shader
    vkhlf::DescriptorBufferInfo uboLights { GetBuffer(surface->GetLightManager(aView)->GetUBOLightBuffer()), 0, sizeof(UBOs::LightManager) };
    wdss.emplace_back(ds, binding++, 0, 1, unibuf, nullptr, uboLights);

    // Illumination Buffer for the Fragment Shader
    vkhlf::DescriptorBufferInfo uboIllumination { GetBuffer(surface->GetUBOIlluminationBuffer(aView)), 0, sizeof(UBOs::Illumination) };
    wdss.emplace_back(ds, binding++, 0, 1, unibuf, nullptr, uboIllumination);

    // Water Buffer for the Vertex Shader
    vkhlf::DescriptorBufferInfo uboWater{ GetBuffer(surface->GetWaterInfluenceMapManager(aView)->GetUBOMapBuffer()), 0, sizeof(UBOs::WaterInformationManager) };
    wdss.emplace_back(ds, binding++, 0, 1, unibuf, nullptr, uboWater);

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

    vkhlf::DescriptorImageInfo dTexInfo{ nullptr, nullptr, vk::ImageLayout::eShaderReadOnlyOptimal };
    vkhlf::DescriptorImageInfo sTexInfo{ nullptr, nullptr, vk::ImageLayout::eShaderReadOnlyOptimal };
    vkhlf::DescriptorImageInfo nTexInfo{ nullptr, nullptr, vk::ImageLayout::eShaderReadOnlyOptimal };

    addTS(mDiffuseTexture, dTexInfo);
    addTS(mSpecularTexture, sTexInfo);
    addTS(mNormalTexture, nTexInfo);

    vkhlf::DescriptorBufferInfo uboModel{ aUBOModel, 0, sizeof(UBOs::Model) };
    wdss.emplace_back(ds, binding++, 0, 1, unibuf, nullptr, uboModel);

    device->updateDescriptorSets(wdss, nullptr);

    return pipelineData;
  }

  void VkSubmesh::Destroy()
  {

  }

  VkSubmesh::~VkSubmesh()
  {

  }

  void VkSubmesh::LoadToVulkan()
  {
    YTEProfileFunction();

    auto &vertices = mSubmesh->mVertexBuffer;
    auto &indices = mSubmesh->mIndexBuffer;

    mVertexBuffer.Update(vertices.data(), vertices.size());
    mIndexBuffer.Update(indices.data(), indices.size());
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
      mSubmeshMap.emplace(submesh->mSubmesh->mShaderSetName, submesh.get());
      mSubmeshes.emplace_back(std::move(submesh));
    }

    LoadToVulkan();
  }

  void VkMesh::UpdateVertices(size_t aSubmeshIndex, std::vector<Vertex>& aVertices)
  {
    YTEProfileFunction();

    mMesh->UpdateVertices(aSubmeshIndex, aVertices);
    mSubmeshes[aSubmeshIndex]->LoadToVulkan();
  }

  void VkMesh::UpdateVerticesAndIndices(size_t aSubmeshIndex, std::vector<Vertex>& aVertices, std::vector<u32>& aIndices)
  {
    YTEProfileFunction();

    mMesh->UpdateVerticesAndIndices(aSubmeshIndex, aVertices, aIndices);
    mSubmeshes[aSubmeshIndex]->LoadToVulkan();
  }

  VkMesh::~VkMesh()
  {
  }

  void VkMesh::LoadToVulkan()
  {
    YTEProfileFunction();

    for (auto &submesh : mSubmeshes)
    {
      submesh->LoadToVulkan();
    }
  }
}
