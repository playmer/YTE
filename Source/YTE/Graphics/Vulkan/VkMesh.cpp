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
      return vk::ImageViewType::e2D;
      break;
    case TextureViewType::eCube:
      return vk::ImageViewType::eCube;
    }

    return vk::ImageViewType{};
  }



  ///////////////////////////////////////////////////////////////////////////
  // Instance Manager
  ///////////////////////////////////////////////////////////////////////////

  void InstanceManager::AddModel(VkInstantiatedModel *aModel)
  {
    auto index = FreeIndex();

    // Where to place the model.
    auto it = std::lower_bound(mModels.begin(), mModels.end(), aModel);
    mModels.emplace(it, aModel);

    // Where to place the index
    auto indexOfIndex = it - mModels.begin();
    auto indexOfIndexIt = mIndexes.begin() + indexOfIndex;

    mIndexes.emplace(indexOfIndexIt, index);
  }

  void InstanceManager::RemoveModel(VkInstantiatedModel *aModel)
  {
    auto it = std::lower_bound(mModels.begin(), mModels.end(), aModel);
    auto indexOfIndex = it - mModels.begin();

    auto indexBeingFreed = mIndexes[indexOfIndex];

    mModels.erase(it);
    mIndexes.erase(mIndexes.begin() + indexOfIndex);

    for (auto &index : mIndexes)
    {
      if (index > indexBeingFreed)
      {
        --index;
      }
    }
  }

  u32 InstanceManager::GetIndex(VkInstantiatedModel *aModel)
  {
    auto it = std::lower_bound(mModels.begin(), mModels.end(), aModel);
    auto indexOfIndex = it - mModels.begin();

    return mIndexes[indexOfIndex];
  }

  void InstanceManager::Clear()
  {
    mIndexes.clear();
    mModels.clear();
    mInstanceBuffer.reset();
    mInstances = 0;
  }

  std::shared_ptr<vkhlf::Buffer>& InstanceManager::InstanceBuffer()
  {
    return mInstanceBuffer;
  }

  u32 InstanceManager::Instances()
  {
    return static_cast<u32>(mIndexes.size());
  }

  void InstanceManager::GrowBuffer()
  {
    //u32 growTo = 2 * mInstances;
    //auto device = mSurface->GetDevice();
    //
    //auto allocator = mSurface->GetAllocator(AllocatorTypes::Mesh);
    //
    //auto buffer = device->createBuffer(sizeof(Instance),
    //  vk::BufferUsageFlagBits::eTransferDst |
    //  vk::BufferUsageFlagBits::eVertexBuffer,
    //  vk::SharingMode::eExclusive,
    //  nullptr,
    //  vk::MemoryPropertyFlagBits::eDeviceLocal,
    //  allocator);
    //
    //mInstances = growTo;
  }

  ///////////////////////////////////////////////////////////////////////////
  // Submesh
  ///////////////////////////////////////////////////////////////////////////
  YTEDefineType(VkSubmesh)
  {
    YTERegisterType(VkSubmesh);
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
    auto allocator = mRenderer->mAllocators[AllocatorTypes::Mesh];

    auto device = mRenderer->mDevice;

    // Create Vertex, Index, and Material buffers.
    auto vertexBufferSize = static_cast<u32>(mSubmesh->mVertexBuffer.size() * sizeof(Vertex));
    mVertexBuffer = device->createBuffer(vertexBufferSize,
                                         vk::BufferUsageFlagBits::eTransferDst |
                                         vk::BufferUsageFlagBits::eVertexBuffer,
                                         vk::SharingMode::eExclusive,
                                         nullptr,
                                         vk::MemoryPropertyFlagBits::eDeviceLocal,
                                         allocator);

    auto indexBufferSize = static_cast<u32>(mSubmesh->mIndexBuffer.size() * sizeof(u32));
    mIndexBuffer = device->createBuffer(indexBufferSize,
                                        vk::BufferUsageFlagBits::eTransferDst |
                                        vk::BufferUsageFlagBits::eIndexBuffer,
                                        vk::SharingMode::eExclusive,
                                        nullptr,
                                        vk::MemoryPropertyFlagBits::eDeviceLocal,
                                        allocator);

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

    if (mMesh->GetInstanced())
    {
      // Adding the Instance Vertex information
      mDescriptions.AddBinding<Instance>(vk::VertexInputRate::eInstance);
      mDescriptions.AddAttribute<glm::vec4>(vk::Format::eR32G32B32A32Sfloat); //glm::vec4 mMatrix0;
      mDescriptions.AddAttribute<glm::vec4>(vk::Format::eR32G32B32A32Sfloat); //glm::vec4 mMatrix1;
      mDescriptions.AddAttribute<glm::vec4>(vk::Format::eR32G32B32A32Sfloat); //glm::vec4 mMatrix2;
      mDescriptions.AddAttribute<glm::vec4>(vk::Format::eR32G32B32A32Sfloat); //glm::vec4 mMatrix3;

      // If we're instanced, we must tell the shaders.
      mDescriptions.AddPreludeLine("#define INSTANCING");
    }
    else
    {
      // Model Buffer for Vertex shader. (Non-instanced Meshes)
      // We do this one last so as to make the binding numbers easier
      // to set via #defines for the shader.
      dslbs.emplace_back(++binding,
                         vk::DescriptorType::eUniformBuffer,
                         vk::ShaderStageFlagBits::eVertex,
                         nullptr);
      ++uniformBuffers;

      // We need to tell the shaders where to find the UBOModel.
      mDescriptions.AddPreludeLine(fmt::format("#define UBO_MODEL_BINDING {}", binding));
    }

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
    auto mesh = static_cast<VkMesh*>(mMesh);

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
    vkhlf::DescriptorBufferInfo uboView{ surface->GetUBOViewBuffer(aView), 0, sizeof(UBOView) };
    wdss.emplace_back(ds, binding++, 0, 1, unibuf, nullptr, uboView);

    // Animation (Bone Array) Buffer for Vertex shader.
    vkhlf::DescriptorBufferInfo uboAnimation{ aUBOAnimation, 0, sizeof(UBOAnimation) };
    wdss.emplace_back(ds, binding++, 0, 1, unibuf, nullptr, uboAnimation);

    // Model Material Buffer for Fragment shader.
    vkhlf::DescriptorBufferInfo uboModelMaterial{ aUBOModelMaterial, 0, sizeof(UBOMaterial) };
    wdss.emplace_back(ds, binding++, 0, 1, unibuf, nullptr, uboModelMaterial);

    // Submesh Material Buffer for Fragment shader.
    vkhlf::DescriptorBufferInfo uboSubmeshMaterial{ aUBOSubmeshMaterial, 0, sizeof(UBOMaterial) };
    wdss.emplace_back(ds, binding++, 0, 1, unibuf, nullptr, uboSubmeshMaterial);

    // Light manager Buffer for Fragment Shader
    vkhlf::DescriptorBufferInfo uboLights { surface->GetLightManager(aView)->GetUBOLightBuffer(), 0, sizeof(UBOLightMan) };
    wdss.emplace_back(ds, binding++, 0, 1, unibuf, nullptr, uboLights);

    // Illumination Buffer for the Fragment Shader
    vkhlf::DescriptorBufferInfo uboIllumination { surface->GetUBOIlluminationBuffer(aView), 0, sizeof(UBOIllumination) };
    wdss.emplace_back(ds, binding++, 0, 1, unibuf, nullptr, uboIllumination);

    // Water Buffer for the Vertex Shader
    vkhlf::DescriptorBufferInfo uboWater{ surface->GetWaterInfluenceMapManager(aView)->GetUBOMapBuffer(), 0, sizeof(UBOWaterInformationMan) };
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

    vkhlf::DescriptorImageInfo dTexInfo{ nullptr, nullptr, vk::ImageLayout::eGeneral };
    vkhlf::DescriptorImageInfo sTexInfo{ nullptr, nullptr, vk::ImageLayout::eGeneral };
    vkhlf::DescriptorImageInfo nTexInfo{ nullptr, nullptr, vk::ImageLayout::eGeneral };

    addTS(mDiffuseTexture, dTexInfo);
    addTS(mSpecularTexture, sTexInfo);
    addTS(mNormalTexture, nTexInfo);

    // TODO (Josh, Andrew): Define the binding of all buffers via a shader preamble.
    // We do the model last for easier binding.
    if (false == mesh->GetInstanced())
    {
      vkhlf::DescriptorBufferInfo uboModel{ aUBOModel, 0, sizeof(UBOModel) };
      wdss.emplace_back(ds, binding++, 0, 1, unibuf, nullptr, uboModel);
    }

    device->updateDescriptorSets(wdss, nullptr);

    return pipelineData;
  }

  void VkSubmesh::Destroy()
  {

  }

  VkSubmesh::~VkSubmesh()
  {

  }

  void VkSubmesh::LoadToVulkan(GraphicsDataUpdateVk *aEvent)
  {
    auto update = aEvent->mCBO;

    mVertexBuffer->update<Vertex>(0, mSubmesh->mVertexBuffer, update);
    mIndexBuffer->update<u32>(0, mSubmesh->mIndexBuffer, update);
  }


  ///////////////////////////////////////////////////////////////////////////
  // Mesh
  ///////////////////////////////////////////////////////////////////////////
  YTEDefineType(VkMesh)
  {
    YTERegisterType(VkMesh);
  }

  VkMesh::VkMesh(Mesh *aMesh,
                 VkRenderer *aRenderer)
    : mRenderer{ aRenderer }
    , mMesh{aMesh}
  {
    for (unsigned i = 0; i < aMesh->mParts.size(); ++i)
    {
      auto submesh = std::make_unique<VkSubmesh>(this, &aMesh->mParts[i], aRenderer);
      mSubmeshes.emplace(submesh->mSubmesh->mShaderSetName, std::move(submesh));
    }

    mRenderer->YTERegister(Events::GraphicsDataUpdateVk, this, &VkMesh::LoadToVulkan);
  }

  void VkMesh::UpdateVertices(int aSubmeshIndex, std::vector<Vertex>& aVertices)
  {
    mMesh->UpdateVertices(aSubmeshIndex, aVertices);

    mRenderer->YTERegister(Events::GraphicsDataUpdateVk, this, &VkMesh::LoadToVulkan);
  }

  void VkMesh::UpdateVerticesAndIndices(int aSubmeshIndex, std::vector<Vertex>& aVertices, std::vector<u32>& aIndices)
  {
    mMesh->UpdateVerticesAndIndices(aSubmeshIndex, aVertices, aIndices);

    mRenderer->YTERegister(Events::GraphicsDataUpdateVk, this, &VkMesh::LoadToVulkan);
  }

  VkMesh::~VkMesh()
  {
  }

  void VkMesh::LoadToVulkan(GraphicsDataUpdateVk *aEvent)
  {
    mRenderer->YTEDeregister(Events::GraphicsDataUpdateVk, this, &VkMesh::LoadToVulkan);

    for (auto &submesh : mSubmeshes)
    {
      submesh.second->LoadToVulkan(aEvent);
    }
  }


  void RemoveOffset(VkInstantiatedModel *aModel)
  {
    YTEUnusedArgument(aModel);
  }

  void RequestOffset(VkInstantiatedModel *aModel)
  {
    YTEUnusedArgument(aModel);
  }

  void VkMesh::SetInstanced(bool aInstanced)
  {
    YTEUnusedArgument(aInstanced);
    //if (aInstanced == mInstanced)
    //{
    //  return;
    //}
    //
    //// We can clear our offsets if we're turning instancing off.
    //if (false == mInstanced)
    //{
    //  mInstanceManager.Clear();
    //}
    //
    //mInstanced = aInstanced;
    //
    //// Switching between instancing or not forces us to recompile shaders and changes some
    //// our descriptors.
    //for (auto &submesh : mSubmeshes)
    //{
    //  submesh.second->Destroy();
    //  submesh.second->Create();
    //}
    //
    //for (auto &viewIt : mSurface->GetViews())
    //{
    //  // Switching also forces us to recreate our DescriptorSets on every model.
    //  for (auto model : viewIt.second.mInstantiatedModels[this])
    //  {
    //    // If we're switching to instancing we need to generate offsets into the instance buffer
    //    // for each model.
    //    if (mInstanced)
    //    {
    //      mInstanceManager.AddModel(model);
    //    }
    //
    //    for (auto [submesh, i] : enumerate(mSubmeshes))
    //    {
    //      model->CreateDescriptorSet(submesh->second.get(), i );
    //    }
    //  } 
    //}
  }
}
