#include <array>
#include <filesystem>

#include "YTE/Core/AssetLoader.hpp"
#include "YTE/Core/Engine.hpp"
#include "YTE/Core/PrivateImplementation.hpp"

#include "YTE/Graphics/Vulkan/VkFunctionLoader.hpp"

#include "YTE/Graphics/GraphicsSystem.hpp"
#include "YTE/Graphics/Model.hpp"
#include "YTE/Graphics/Mesh.hpp"
#include "YTE/Graphics/ShaderDescriptions.hpp"
#include "YTE/Graphics/Texture.hpp"
#include "YTE/Graphics/View.hpp"
#include "YTE/Graphics/Vulkan/VkModel.hpp"
#include "YTE/Graphics/Vulkan/VkRenderer.hpp"

#include "YTE/Utilities/Utilities.h"

namespace YTE
{
    MeshToRender::MeshToRender(std::shared_ptr<vkhlf::PipelineLayout> const *aPipelineLayout,
      std::shared_ptr<vkhlf::DescriptorSet> const *aDescriptorSet,
      std::shared_ptr<vkhlf::Buffer> const *aVertexBuffer,
      std::shared_ptr<vkhlf::Buffer> const *aIndexBuffer,
      u64 aIndexCount)
      : mPipelineLayout(aPipelineLayout),
      mDescriptorSet(aDescriptorSet),
      mVertexBuffer(aVertexBuffer),
      mIndexBuffer(aIndexBuffer),
      mIndexCount(aIndexCount)
    {

    }

    MeshToRender::MeshToRender(const MeshToRender &aRight)
      : mPipelineLayout(aRight.mPipelineLayout),
      mDescriptorSet(aRight.mDescriptorSet),
      mVertexBuffer(aRight.mVertexBuffer),
      mIndexBuffer(aRight.mIndexBuffer),
      mIndexCount(aRight.mIndexCount)
    {

    }

    MeshToRender& MeshToRender::operator=(const MeshToRender &aRight)
    {
      mPipelineLayout = aRight.mPipelineLayout;
      mDescriptorSet = aRight.mDescriptorSet;
      mVertexBuffer = aRight.mVertexBuffer;
      mIndexBuffer = aRight.mIndexBuffer;
      mIndexCount = aRight.mIndexCount;

      return *this;
    }

    PipelineData::PipelineData(std::shared_ptr<vkhlf::Pipeline> aPipeline)
      : mPipeline(aPipeline)
    {

    }

    void InstantiatedMeshRendererData::UpdateUniformBuffer(InstantiatedMesh & aModel)
    {
      UBOModel model;

      model.mModelMatrix = glm::translate(model.mModelMatrix, aModel.mPosition);
      model.mModelMatrix = model.mModelMatrix * glm::toMat4(aModel.mRotation);
      model.mModelMatrix = glm::scale(model.mModelMatrix, aModel.mScale);

      auto update = mSurface->mCommandPool->allocateCommandBuffer();

      update->begin();
      mUBOModel->update<UBOModel>(0, model, update);
      update->end();

      vkhlf::submitAndWait(mSurface->mGraphicsQueue, update);
    }

    void VkRenderer::UpdateModelTransformation(Model *aModel)
    {
      auto data = aModel->GetInstantiatedMesh()->mData.Get<InstantiatedMeshRendererData>();
      data->UpdateUniformBuffer(*aModel->GetInstantiatedMesh());
    }

    std::unique_ptr<InstantiatedMesh> VkRenderer::AddModel(Window *aWindow,
      std::string &aMeshFile)
    {
      auto surfaceIt = mSurfaces.find(aWindow);

      if (surfaceIt == mSurfaces.end())
      {
        DebugObjection(true, "We can't find a surface corresponding to the provided window.");
        return nullptr;
      }

      auto surface = surfaceIt->second.get();

      auto allocator = mAllocators[surface->mDevice.get()][AllocatorTypes::UniformBufferObject];
      auto iMesh = std::make_unique<InstantiatedMesh>();

      iMesh->mMesh = AddMesh(surface, aMeshFile);

      auto iMeshData = iMesh->mData.ConstructAndGet<InstantiatedMeshRendererData>(this, iMesh->mMesh, aWindow);

      iMeshData->mSurface = surface;
      iMeshData->mUBOModel = surface->mDevice->createBuffer(sizeof(UBOModel),
        vk::BufferUsageFlagBits::eTransferDst |
        vk::BufferUsageFlagBits::eUniformBuffer,
        vk::SharingMode::eExclusive,
        nullptr,
        vk::MemoryPropertyFlagBits::eDeviceLocal,
        allocator);

      iMeshData->UpdateUniformBuffer(*iMesh);

      auto &pipelines = surface->mPipelines;

      auto numberOfSubmeshes = iMesh->mMesh->mParts.size();

      iMeshData->mSubmeshPipelineData.reserve(numberOfSubmeshes);

      for (size_t i = 0; i < numberOfSubmeshes; ++i)
      {
        auto &subMesh = iMesh->mMesh->mParts[i];

        auto subMeshData = subMesh.mRendererData.Get<MeshRendererData>();

        iMeshData->mSubmeshPipelineData.push_back(InstantiatedMeshRendererData::SubmeshPipelineData{});

        auto &submeshPipelineData = iMeshData->mSubmeshPipelineData[i];

        submeshPipelineData.mId = mMeshIdCounter++;

        AddDescriptorSet(surface,
          iMesh.get(),
          &subMesh,
          &submeshPipelineData);

        auto it = pipelines.find(subMesh.mShaderSetName);

        if (it == pipelines.end())
        {
          auto pipeline = AddPipeline(surface,
            iMesh.get(),
            &subMesh,
            &submeshPipelineData);

          PipelineData pipelineData{ pipeline };

          it = pipelines.emplace(subMesh.mShaderSetName, pipelineData).first;
        }

        MeshToRender toRender{ &submeshPipelineData.mPipelineLayout,
          &submeshPipelineData.mDescriptorSet,
          &subMeshData->mVertexBuffer,
          &subMeshData->mIndexBuffer,
          subMesh.mIndexBuffer.size() };


        auto &models = it->second.mModels;

        models.Emplace(submeshPipelineData.mId, toRender);
      }

      return std::move(iMesh);
    }
}
