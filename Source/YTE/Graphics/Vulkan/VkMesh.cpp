///////////////////
// Author: Andrew Griffin
// YTE - Graphics - Vulkan
///////////////////

#include "YTE/Graphics/Vulkan/VkDeviceInfo.hpp"
#include "YTE/Graphics/Vulkan/VkInstantiatedModel.hpp"
#include "YTE/Graphics/Vulkan/VkMesh.hpp"
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
    u32 growTo = 2 * mInstances;
    auto device = mSurface->GetDevice();

    auto allocator = mSurface->GetAllocator(AllocatorTypes::Mesh);

    auto buffer = device->createBuffer(sizeof(Instance),
      vk::BufferUsageFlagBits::eTransferDst |
      vk::BufferUsageFlagBits::eVertexBuffer,
      vk::SharingMode::eExclusive,
      nullptr,
      vk::MemoryPropertyFlagBits::eDeviceLocal,
      allocator);

    mInstances = growTo;
  }

  ///////////////////////////////////////////////////////////////////////////
  // Submesh
  ///////////////////////////////////////////////////////////////////////////
  YTEDefineType(VkSubmesh)
  {
    YTERegisterType(VkSubmesh);
  }

  VkSubmesh::VkSubmesh(VkMesh *aMesh, Submesh *aSubmesh, VkRenderedSurface *aSurface)
    : mDiffuseTexture(nullptr)
    , mSpecularTexture(nullptr)
    , mNormalTexture(nullptr)
    , mShader(nullptr)
    , mSurface(aSurface)
    , mMesh(aMesh)
    , mSubmesh(aSubmesh)
    , mIndexCount(0)
  {
    Create();
  }


  void VkSubmesh::Create()
  {
    auto device = mSurface->GetDevice();

    auto allocator = mSurface->GetAllocator(AllocatorTypes::Mesh);

    // Create Vertex, Index, and Material buffers.
    mVertexBuffer = device->createBuffer(mSubmesh->mVertexBufferSize,
                                         vk::BufferUsageFlagBits::eTransferDst |
                                         vk::BufferUsageFlagBits::eVertexBuffer,
                                         vk::SharingMode::eExclusive,
                                         nullptr,
                                         vk::MemoryPropertyFlagBits::eDeviceLocal,
                                         allocator);

    mIndexBuffer = device->createBuffer(mSubmesh->mIndexBufferSize,
                                        vk::BufferUsageFlagBits::eTransferDst |
                                        vk::BufferUsageFlagBits::eIndexBuffer,
                                        vk::SharingMode::eExclusive,
                                        nullptr,
                                        vk::MemoryPropertyFlagBits::eDeviceLocal,
                                        allocator);

    mUBOMaterial = device->createBuffer(sizeof(UBOMaterial),
                                        vk::BufferUsageFlagBits::eTransferDst |
                                        vk::BufferUsageFlagBits::eUniformBuffer,
                                        vk::SharingMode::eExclusive,
                                        nullptr,
                                        vk::MemoryPropertyFlagBits::eDeviceLocal,
                                        allocator);

    mIndexCount = mSubmesh->mIndexBuffer.size();

    // Load Textures
    u32 samplers{ 0 };

    if (false == mSubmesh->mDiffuseMap.empty())
    {
      mDiffuseTexture = mSurface->CreateTexture(mSubmesh->mDiffuseMap, Convert(mSubmesh->mDiffuseType));
      ++samplers;
    }
    if (false == mSubmesh->mSpecularMap.empty())
    {
      mSpecularTexture = mSurface->CreateTexture(mSubmesh->mSpecularMap, Convert(mSubmesh->mSpecularType));
      ++samplers;
    }
    if (false == mSubmesh->mNormalMap.empty())
    {
      mNormalTexture = mSurface->CreateTexture(mSubmesh->mNormalMap, Convert(mSubmesh->mNormalType));
      ++samplers;
    }

    std::vector<vkhlf::DescriptorSetLayoutBinding> dslbs;
    u32 binding{ 0 };
    u32 uniformBuffers{ 0 };

    // View Buffer for Vertex shader.
    dslbs.emplace_back(binding,
                       vk::DescriptorType::eUniformBuffer,
                       vk::ShaderStageFlagBits::eVertex,
                       nullptr);
    ++uniformBuffers;

    // Animation (Bone Array) Buffer for Vertex shader.
    dslbs.emplace_back(++binding,
                       vk::DescriptorType::eUniformBuffer,
                       vk::ShaderStageFlagBits::eVertex,
                       nullptr);
    ++uniformBuffers;

    // Material Buffer for Fragment shader.
    dslbs.emplace_back(++binding,
                       vk::DescriptorType::eUniformBuffer,
                       vk::ShaderStageFlagBits::eFragment,
                       nullptr);
    ++uniformBuffers;

    // Descriptions for the textures we support based on which maps we found above:
    //   Diffuse
    //   Specular
    //   Normal
    for (u32 i = 0; i < samplers; ++i)
    {
      dslbs.emplace_back(++binding,
                         vk::DescriptorType::eCombinedImageSampler,
                         vk::ShaderStageFlagBits::eFragment,
                         nullptr);
    }

    // Shader Descriptions
    // TODO (Josh): We should be reflecting these.
    VkShaderDescriptions descriptions;
    descriptions.AddBinding<Vertex>(vk::VertexInputRate::eVertex);
    descriptions.AddAttribute<glm::vec3>(vk::Format::eR32G32B32Sfloat); //glm::vec3 mPosition;
    descriptions.AddAttribute<glm::vec3>(vk::Format::eR32G32B32Sfloat); //glm::vec3 mTextureCoordinates;
    descriptions.AddAttribute<glm::vec3>(vk::Format::eR32G32B32Sfloat); //glm::vec3 mNormal;
    descriptions.AddAttribute<glm::vec3>(vk::Format::eR32G32B32Sfloat); //glm::vec3 mColor;
    descriptions.AddAttribute<glm::vec3>(vk::Format::eR32G32B32Sfloat); //glm::vec3 mTangent;
    descriptions.AddAttribute<glm::vec3>(vk::Format::eR32G32B32Sfloat); //glm::vec3 mBinormal;
    descriptions.AddAttribute<glm::vec3>(vk::Format::eR32G32B32Sfloat); //glm::vec3 mBitangent;
    descriptions.AddAttribute<glm::vec3>(vk::Format::eR32G32B32Sfloat); //glm::vec4 mBoneWeights;
    descriptions.AddAttribute<glm::vec2>(vk::Format::eR32G32Sfloat);    //glm::vec2 mBoneWeights2;
    descriptions.AddAttribute<glm::ivec3>(vk::Format::eR32G32B32Sint);  //glm::ivec4 mBoneIDs;
    descriptions.AddAttribute<glm::ivec2>(vk::Format::eR32G32Sint);     //glm::ivec4 mBoneIDs;

    // Defines needed for shader compilation.
    std::string defines;

    if (mMesh->GetInstanced())
    {
      // Adding the Instance Vertex information
      descriptions.AddBinding<Instance>(vk::VertexInputRate::eInstance);
      descriptions.AddAttribute<glm::vec4>(vk::Format::eR32G32B32A32Sfloat); //glm::vec4 mMatrix0;
      descriptions.AddAttribute<glm::vec4>(vk::Format::eR32G32B32A32Sfloat); //glm::vec4 mMatrix1;
      descriptions.AddAttribute<glm::vec4>(vk::Format::eR32G32B32A32Sfloat); //glm::vec4 mMatrix2;
      descriptions.AddAttribute<glm::vec4>(vk::Format::eR32G32B32A32Sfloat); //glm::vec4 mMatrix3;

      // If we're instanced, we must tell the shaders.
      defines = "#define INSTANCING\n";
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
      defines = "#define UBO_MODEL_BINDING ";
      defines += std::to_string(binding);
      defines += "\n";
    }

    // Create the descriptor set and pipeline layouts.
    mDescriptorTypes.emplace_back(vk::DescriptorType::eUniformBuffer, uniformBuffers);
    mDescriptorTypes.emplace_back(vk::DescriptorType::eCombinedImageSampler, samplers);

    mDescriptorSetLayout = device->createDescriptorSetLayout(dslbs);

    auto pipelineLayout = device->createPipelineLayout(mDescriptorSetLayout, nullptr);

    if (nullptr != mShader)
    {
      // Update relevant members, then load.
      mShader->mDescriptions = descriptions;
      mShader->mPipelineLayout = pipelineLayout;
      mShader->mDefines = defines;
      mShader->Load();
    }
    else
    {
      // load shader passing our created pipeline layout
      mShader = mSurface->CreateShader(mSubmesh->mShaderSetName,
                                       pipelineLayout,
                                       descriptions,
                                       mSubmesh->mCullBackFaces,
                                       defines);
    }
  }


  std::shared_ptr<vkhlf::DescriptorPool> VkSubmesh::MakePool()
  {
    auto device = mSurface->GetDevice();

    return device->createDescriptorPool({}, 1, mDescriptorTypes);
  }

  SubMeshPipelineData VkSubmesh::CreatePipelineData(std::shared_ptr<vkhlf::Buffer> aUBOModel,
                                                    std::shared_ptr<vkhlf::Buffer> aUBOAnimation)
  {
    auto mesh = static_cast<VkMesh*>(mMesh);

    auto device = mSurface->GetDevice();

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
    vkhlf::DescriptorBufferInfo uboView{ mSurface->GetUBOViewBuffer(), 0, sizeof(UBOView) };
    wdss.emplace_back(ds, binding++, 0, 1, unibuf, nullptr, uboView);

    // Animation (Bone Array) Buffer for Vertex shader.
    vkhlf::DescriptorBufferInfo uboAnimation{ aUBOAnimation, 0, sizeof(UBOAnimation) };
    wdss.emplace_back(ds, binding++, 0, 1, unibuf, nullptr, uboAnimation);

    // Material Buffer for Fragment shader.
    vkhlf::DescriptorBufferInfo uboMaterial{ mUBOMaterial, 0, sizeof(UBOMaterial) };
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
    mUBOMaterial->update<UBOMaterial>(0, mSubmesh->mUBOMaterial, update);
  }


  ///////////////////////////////////////////////////////////////////////////
  // Mesh
  ///////////////////////////////////////////////////////////////////////////
  YTEDefineType(VkMesh)
  {
    YTERegisterType(VkMesh);
  }

  VkMesh::VkMesh(Window *aWindow,
                 VkRenderedSurface *aSurface,
                 std::string &aFile,
                 CreateInfo *aCreateInfo)
    : Mesh(aWindow, aFile, aCreateInfo)
    , mSurface(aSurface)
  {
    for (unsigned i = 0; i < mParts.size(); ++i)
    {
      auto submesh = std::make_unique<VkSubmesh>(this, &mParts[i], aSurface);
      mSubmeshes.emplace(submesh->mShader, std::move(submesh));
    }

    aSurface->YTERegister(Events::GraphicsDataUpdateVk, this, &VkMesh::LoadToVulkan);
  }


  VkMesh::VkMesh(Window *aWindow,
                 VkRenderedSurface *aSurface,
                 std::string &aFile,
                 std::vector<Submesh> &aSubmeshes)
    : Mesh(aWindow, aFile, aSubmeshes)
    , mSurface(aSurface)
  {
    for (unsigned i = 0; i < mParts.size(); ++i)
    {
      auto submesh = std::make_unique<VkSubmesh>(this, &mParts[i], aSurface);
      mSubmeshes.emplace(submesh->mShader, std::move(submesh));
    }

    aSurface->YTERegister(Events::GraphicsDataUpdateVk, this, &VkMesh::LoadToVulkan);
  }

  VkMesh::~VkMesh()
  {
  }

  void VkMesh::LoadToVulkan(GraphicsDataUpdateVk *aEvent)
  {
    mSurface->YTEDeregister(Events::GraphicsDataUpdateVk, this, &VkMesh::LoadToVulkan);

    for (auto &submesh : mSubmeshes)
    {
      submesh.second->LoadToVulkan(aEvent);
    }
  }


  void RemoveOffset(VkInstantiatedModel *aModel)
  {

  }

  void RequestOffset(VkInstantiatedModel *aModel)
  {

  }

  void VkMesh::SetInstanced(bool aInstanced)
  {
    if (aInstanced == mInstanced)
    {
      return;
    }

    // We can clear our offsets if we're turning instancing off.
    if (false == mInstanced)
    {
      mInstanceManager.Clear();
    }

    mInstanced = aInstanced;

    // Switching between instancing or not forces us to recompile shaders and changes some
    // our descriptors.
    for (auto &submesh : mSubmeshes)
    {
      submesh.second->Destroy();
      submesh.second->Create();
    }

    auto models = mSurface->GetInstantiatedModels(this);

    u32 offset{ 0 };

    // Switching also forces us to recreate our DescriptorSets on every model.
    for (auto model : models)
    {
      // If we're switching to instancing we need to generate offsets into the instance buffer
      // for each model.
      if (mInstanced)
      {
        mInstanceManager.AddModel(model);
      }

      for (auto &submesh : mSubmeshes)
      {
        model->CreateDescriptorSet(submesh.second.get());
      }
    }
  }
}
