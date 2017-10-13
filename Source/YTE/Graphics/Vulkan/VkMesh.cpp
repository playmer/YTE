///////////////////
// Author: Andrew Griffin
// YTE - Graphics - Vulkan
///////////////////

#include "YTE/Graphics/Vulkan/VkMesh.hpp"
#include "YTE/Graphics/Vulkan/VkRenderedSurface.hpp"
#include "YTE/Graphics/Vulkan/VkUtilities.hpp"


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



  VkSubmesh::VkSubmesh(Submesh *aSubmesh, std::shared_ptr<VkRenderedSurface> aSurface)
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
      mDiffuseTexture = aSurface->CreateTexture(mSubmesh->mDiffuseMap);
      ++samplers;
    }
    if (false == mSubmesh->mSpecularMap.empty())
    {
      mSpecularTexture = aSurface->CreateTexture(mSubmesh->mSpecularMap);
      ++samplers;
    }
    if (false == mSubmesh->mNormalMap.empty())
    {
      mNormalTexture = aSurface->CreateTexture(mSubmesh->mNormalMap);
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

    auto pipelineLayout = device->createPipelineLayout(descriptorSetLayout, nullptr);



    // load shader passing our created pipeline layout
    mShader = aSurface->CreateShader(mSubmesh->mShaderSetName, pipelineLayout);
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
                 std::shared_ptr<VkRenderedSurface> aSurface,
                 std::string &aFile,
                 CreateInfo *aCreateInfo)
    : Mesh(aWindow, aFile, aCreateInfo)
    , mSurface(aSurface)
  {
    for (unsigned i = 0; i < mParts.size(); ++i)
    {
      VkSubmesh* s = new VkSubmesh(&(mParts[i]), aSurface);
      std::shared_ptr<VkSubmesh> s2(s);
      mSubmeshes.push_back(s2);
    }

    aSurface->YTERegister(Events::GraphicsDataUpdateVk, this, &VkMesh::LoadToVulkan);
  }



  VkMesh::~VkMesh()
  {
    mSurface->DestroyMesh(mParts[0].mName);

  }



  void VkMesh::LoadToVulkan(GraphicsDataUpdateVk *aEvent)
  {
    mSurface->YTEDeregister(Events::GraphicsDataUpdateVk, this, &VkMesh::LoadToVulkan);

    for (auto sm : mSubmeshes)
    {
      sm->LoadToVulkan(aEvent);
    }
  }
}
