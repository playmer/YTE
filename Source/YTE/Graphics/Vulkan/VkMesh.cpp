///////////////////
// Author: Andrew Griffin
// YTE - Graphics - Vulkan
///////////////////

#include "YTE/Graphics/Vulkan/VkMesh.hpp"
#include "YTE/Graphics/Vulkan/VkRenderedSurface.hpp"
#include "YTE/Graphics/Vulkan/VkDeviceInfo.hpp"


namespace YTE
{
  YTEDefineType(VkSubmesh)
  {
    YTERegisterType(VkSubmesh);
  }

  YTEDefineType(VkMesh)
  {
    YTERegisterType(VkMesh);
  }

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


  VkSubmesh::VkSubmesh(Submesh *aSubmesh, VkRenderedSurface *aSurface)
    : mDiffuseTexture(nullptr)
    , mSpecularTexture(nullptr)
    , mNormalTexture(nullptr)
    , mShader(nullptr)
    , mSubmesh(nullptr)
    , mIndexCount(0)
  {
    mSubmesh = aSubmesh;

    auto device = aSurface->GetDevice();

    auto allocator = aSurface->GetAllocator(AllocatorTypes::Mesh);

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

    // load Textures
    u32 samplers = 0;

    if (false == mSubmesh->mDiffuseMap.empty())
    {
      mDiffuseTexture = aSurface->CreateTexture(mSubmesh->mDiffuseMap, Convert(mSubmesh->mDiffuseType));
      ++samplers;
    }
    if (false == mSubmesh->mSpecularMap.empty())
    {
      mSpecularTexture = aSurface->CreateTexture(mSubmesh->mSpecularMap, Convert(mSubmesh->mSpecularType));
      ++samplers;
    }
    if (false == mSubmesh->mNormalMap.empty())
    {
      mNormalTexture = aSurface->CreateTexture(mSubmesh->mNormalMap, Convert(mSubmesh->mNormalType));
      ++samplers;
    }

    // init descriptor and pipeline layouts (needed to load shaders)
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

    auto pipelineLayout = device->createPipelineLayout(descriptorSetLayout, nullptr);


    // TODO (Josh): We should be reflecting these.
    VkShaderDescriptions descriptions;
    descriptions.AddBinding<Vertex>(vk::VertexInputRate::eVertex);

    //glm::vec3 mPosition;
    descriptions.AddAttribute<glm::vec3>(vk::Format::eR32G32B32Sfloat);

    //glm::vec3 mTextureCoordinates;
    descriptions.AddAttribute<glm::vec3>(vk::Format::eR32G32B32Sfloat);

    //glm::vec3 mNormal;
    descriptions.AddAttribute<glm::vec3>(vk::Format::eR32G32B32Sfloat);

    //glm::vec3 mColor;
    descriptions.AddAttribute<glm::vec3>(vk::Format::eR32G32B32Sfloat);

    //glm::vec3 mTangent;
    descriptions.AddAttribute<glm::vec3>(vk::Format::eR32G32B32Sfloat);

    //glm::vec3 mBinormal;
    descriptions.AddAttribute<glm::vec3>(vk::Format::eR32G32B32Sfloat);

    //glm::vec3 mBitangent;
    descriptions.AddAttribute<glm::vec3>(vk::Format::eR32G32B32Sfloat);

    //glm::vec4 mBoneWeights;
    descriptions.AddAttribute<glm::vec3>(vk::Format::eR32G32B32Sfloat);

    //glm::vec2 mBoneWeights2;
    descriptions.AddAttribute<glm::vec2>(vk::Format::eR32G32Sfloat);

    //glm::ivec4 mBoneIDs;
    descriptions.AddAttribute<glm::ivec3>(vk::Format::eR32G32B32Sint);

    //glm::ivec4 mBoneIDs;
    descriptions.AddAttribute<glm::ivec2>(vk::Format::eR32G32Sint);

    // load shader passing our created pipeline layout
    mShader = aSurface->CreateShader(mSubmesh->mShaderSetName, pipelineLayout, descriptions);
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



  VkMesh::VkMesh(Window *aWindow,
                 VkRenderedSurface *aSurface,
                 std::string &aFile,
                 CreateInfo *aCreateInfo)
    : Mesh(aWindow, aFile, aCreateInfo)
    , mSurface(aSurface)
  {
    for (unsigned i = 0; i < mParts.size(); ++i)
    {
      auto submesh = std::make_unique<VkSubmesh>(&mParts[i], aSurface);
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
      auto submesh = std::make_unique<VkSubmesh>(&mParts[i], aSurface);
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
}
