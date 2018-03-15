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
    : mRenderer(aRenderer)
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

    mIndexCount = mSubmesh->mIndexBuffer.size();

    // Load Textures
    size_t samplers{ 0 };
    std::vector<std::string> samplerTypes;
    samplerTypes.resize(mSubmesh->mFrameBuffers.size() + mSubmesh->mTextures.size());

    for (size_t i = 0; i < mSubmesh->mTextures.size(); ++i)
    {
      mVkTextures.emplace_back(mRenderer->CreateTexture(mSubmesh->mTextures[i].mFileName, Convert(mSubmesh->mTextures[i].mViewType)));
      samplerTypes[samplers++] = mSubmesh->mTextures[i].mTypeID;
    }

    for (size_t i = 0; i < mSubmesh->mFrameBuffers.size(); ++i)
    {
      samplerTypes[samplers++] = mSubmesh->mFrameBuffers[i].mTypeID;
    }

    std::vector<vkhlf::DescriptorSetLayoutBinding> dslbs;
    u32 binding{ 0 };
    u32 uniformBuffers{ 0 };

    for (size_t i = 0; i < mSubmesh->mUBOs.size(); ++i)
    {
      vk::ShaderStageFlags flags;

      if (mSubmesh->mUBOs[i].mShaderUsage.mVertexShader == true)
      {
        flags = vk::ShaderStageFlagBits::eVertex;
        if (mSubmesh->mUBOs[i].mShaderUsage.mFragmentShader == true)
        {
          flags = vk::ShaderStageFlagBits::eFragment | vk::ShaderStageFlagBits::eVertex;
        }
      }
      else if (mSubmesh->mUBOs[i].mShaderUsage.mFragmentShader == true)
      {
        flags = vk::ShaderStageFlagBits::eFragment;
      }

      // View Buffer for Vertex shader.
      dslbs.emplace_back(binding,
                         vk::DescriptorType::eUniformBuffer,
                         flags,
                         nullptr);
      mDescriptions.AddPreludeLine(fmt::format("#define UBO_{}_BINDING {}", mSubmesh->mUBOs[i].mTypeID, binding));
      ++uniformBuffers;
      ++binding;
    }

    // Descriptions for the textures we support based on which maps we found above:
    for (size_t i = 0; i < samplers; ++i)
    {
      dslbs.emplace_back(binding,
                         vk::DescriptorType::eCombinedImageSampler,
                         vk::ShaderStageFlagBits::eFragment,
                         nullptr);
      mDescriptions.AddPreludeLine(fmt::format("#define UBO_{}_BINDING {}", samplerTypes[i], binding));
      ++binding;
    }

    // vertex
    mDescriptions.AddBinding<Vertex>(vk::VertexInputRate::eVertex);
    mDescriptions.AddAttribute<glm::vec3>(vk::Format::eR32G32B32Sfloat); //glm::vec3 mPosition;
    mDescriptions.AddAttribute<glm::vec3>(vk::Format::eR32G32B32Sfloat); //glm::vec3 mTextureCoordinates;
    mDescriptions.AddAttribute<glm::vec3>(vk::Format::eR32G32B32Sfloat); //glm::vec3 mNormal;
    mDescriptions.AddAttribute<glm::vec3>(vk::Format::eR32G32B32Sfloat); //glm::vec3 mColor;
    mDescriptions.AddAttribute<glm::vec3>(vk::Format::eR32G32B32Sfloat); //glm::vec3 mTangent;
    mDescriptions.AddAttribute<glm::vec3>(vk::Format::eR32G32B32Sfloat); //glm::vec3 mBinormal;
    mDescriptions.AddAttribute<glm::vec3>(vk::Format::eR32G32B32Sfloat); //glm::vec3 mBitangent;
    mDescriptions.AddAttribute<glm::vec3>(vk::Format::eR32G32B32Sfloat); //glm::vec4 mBoneWeights;
    mDescriptions.AddAttribute<glm::vec2>(vk::Format::eR32G32Sfloat);    //glm::vec2 mBoneWeights2;
    mDescriptions.AddAttribute<glm::ivec3>(vk::Format::eR32G32B32Sint);  //glm::ivec4 mBoneIDs;
    mDescriptions.AddAttribute<glm::ivec2>(vk::Format::eR32G32Sint);     //glm::ivec4 mBoneIDs;

    // UBO Model is made above, this is old
    //if (mMesh->GetInstanced())
    //{
    //  // Adding the Instance Vertex information
    //  mDescriptions.AddBinding<Instance>(vk::VertexInputRate::eInstance);
    //  mDescriptions.AddAttribute<glm::vec4>(vk::Format::eR32G32B32A32Sfloat); //glm::vec4 mMatrix0;
    //  mDescriptions.AddAttribute<glm::vec4>(vk::Format::eR32G32B32A32Sfloat); //glm::vec4 mMatrix1;
    //  mDescriptions.AddAttribute<glm::vec4>(vk::Format::eR32G32B32A32Sfloat); //glm::vec4 mMatrix2;
    //  mDescriptions.AddAttribute<glm::vec4>(vk::Format::eR32G32B32A32Sfloat); //glm::vec4 mMatrix3;
    //
    //  // If we're instanced, we must tell the shaders.
    //  mDescriptions.AddPreludeLine("#define INSTANCING");
    //}
    //else
    //{
    //  // Model Buffer for Vertex shader. (Non-instanced Meshes)
    //  // We do this one last so as to make the binding numbers easier
    //  // to set via #defines for the shader.
    //  dslbs.emplace_back(binding,
    //                     vk::DescriptorType::eUniformBuffer,
    //                     vk::ShaderStageFlagBits::eVertex,
    //                     nullptr);
    //  ++uniformBuffers;
    //
    //  // We need to tell the shaders where to find the UBOModel.
    //  mDescriptions.AddPreludeLine(fmt::format("#define UBO_MODEL_BINDING {}", binding));
    //  ++binding;
    //}

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

  SubMeshPipelineData VkSubmesh::CreatePipelineData(std::vector<std::shared_ptr<vkhlf::Buffer>> aBuffers,
                                                    std::vector<std::shared_ptr<vkhlf::Sampler>*> aSamplers,
                                                    std::vector<std::shared_ptr<vkhlf::ImageView>*> aImageViews)
  {
    auto device = mRenderer->mDevice;

    SubMeshPipelineData pipelineData;
    pipelineData.mPipelineLayout = device->createPipelineLayout(mDescriptorSetLayout,
                                                                nullptr);
    pipelineData.mDescriptorSet = device->allocateDescriptorSet(MakePool(),
                                                                mDescriptorSetLayout);

    std::vector<vkhlf::WriteDescriptorSet> wdss;
    wdss.reserve(aBuffers.size());

    // Helper constants and variables.
    constexpr auto unibuf = vk::DescriptorType::eUniformBuffer;
    auto &ds = pipelineData.mDescriptorSet;
    u32 binding = 0;

    // Add Uniform Buffers

    for (size_t i = 0; i < aBuffers.size(); ++i)
    {
      vkhlf::DescriptorBufferInfo uboView{ aBuffers[i], 0, mSubmesh->mUBOs[i].mSize };
      wdss.emplace_back(ds, binding++, 0, 1, unibuf, nullptr, uboView);
    }

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

    for (size_t i = 0; i < mVkTextures.size(); ++i)
    {
      vkhlf::DescriptorImageInfo dTexInfo{ nullptr, nullptr, vk::ImageLayout::eGeneral };
      addTS(mVkTextures[i], dTexInfo);
    }


    // Add Framebuffer Samplers
    auto addFBS = [&wdss, &binding, &ds](std::shared_ptr<vkhlf::Sampler>* aSampler,
                                        std::shared_ptr<vkhlf::ImageView> *aImageView,
                                        vkhlf::DescriptorImageInfo &aImageInfo)
    {
      constexpr auto imgsam = vk::DescriptorType::eCombinedImageSampler;

      aImageInfo.sampler = *aSampler;
      aImageInfo.imageView = *aImageView;
      wdss.emplace_back(ds, binding++, 0, 1, imgsam, aImageInfo, nullptr);
    };

    for (size_t i = 0; i < aSamplers.size(); ++i)
    {
      vkhlf::DescriptorImageInfo dTexInfo{ nullptr, nullptr, vk::ImageLayout::eGeneral };
      addFBS(aSamplers[i], aImageViews[i], dTexInfo);
    }

    //// TODO (Josh, Andrew): Define the binding of all buffers via a shader preamble.
    //// We do the model last for easier binding.
    //if (false == mesh->GetInstanced())
    //{
    //  vkhlf::DescriptorBufferInfo uboModel{ aUBOModel, 0, sizeof(UBOModel) };
    //  wdss.emplace_back(ds, binding++, 0, 1, unibuf, nullptr, uboModel);
    //}

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

  VkMesh::VkMesh(VkRenderer *aRenderer,
                 std::string &aFile,
                 CreateInfo *aCreateInfo)
    : Mesh(aFile, aCreateInfo)
    , mRenderer{ aRenderer }
  {
    for (unsigned i = 0; i < mParts.size(); ++i)
    {
      auto submesh = std::make_unique<VkSubmesh>(this, &mParts[i], aRenderer);
      mSubmeshes.emplace(submesh->mSubmesh->mShaderSetName, std::move(submesh));
    }

    mRenderer->YTERegister(Events::GraphicsDataUpdateVk, this, &VkMesh::LoadToVulkan);
  }


  VkMesh::VkMesh(VkRenderer *aRenderer,
                 std::string &aFile,
                 std::vector<Submesh> &aSubmeshes)
    : Mesh(aFile, aSubmeshes)
    , mRenderer{aRenderer}
  {
    for (unsigned i = 0; i < mParts.size(); ++i)
    {
      auto submesh = std::make_unique<VkSubmesh>(this, &mParts[i], aRenderer);
      mSubmeshes.emplace(submesh->mSubmesh->mShaderSetName, std::move(submesh));
    }

    mRenderer->YTERegister(Events::GraphicsDataUpdateVk, this, &VkMesh::LoadToVulkan);
  }

  void VkMesh::UpdateVertices(int aSubmeshIndex, std::vector<Vertex>& aVertices)
  {
    Mesh::UpdateVertices(aSubmeshIndex, aVertices);

    mRenderer->YTERegister(Events::GraphicsDataUpdateVk, this, &VkMesh::LoadToVulkan);
  }

  void VkMesh::UpdateVerticesAndIndices(int aSubmeshIndex, std::vector<Vertex>& aVertices, std::vector<u32>& aIndices)
  {
    Mesh::UpdateVerticesAndIndices(aSubmeshIndex, aVertices, aIndices);

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
