///////////////////
// Author: Andrew Griffin
// YTE - Graphics - Vulkan - Drawers
///////////////////

#include "YTE/Graphics/Vulkan/Drawers/VkRTWaterDrawer.hpp"
#include "YTE/Graphics/Vulkan/VkRenderedSurface.hpp"
#include "YTE/Graphics/Vulkan/VkInstantiatedModel.hpp"
#include "YTE/Graphics/Vulkan/VkMesh.hpp"
#include "YTE/Graphics/Vulkan/VkShader.hpp"

namespace YTE
{
  YTEDefineType(VkRTWaterDrawer)
  {
    YTERegisterType(VkRTWaterDrawer);
  }

  VkRTWaterDrawer::VkRTWaterDrawer(VkRenderedSurface *aSurface,
                                   vk::Format aColorFormat,
                                   vk::Format aDepthFormat,
                                   std::shared_ptr<vkhlf::Surface>& aVulkanSurface,
                                   ViewData* aView,
                                   std::string aName,
                                   YTEDrawerTypeCombination aCombinationType)
    : VkRenderTarget(aSurface,
                     aColorFormat,
                     aDepthFormat,
                     aVulkanSurface,
                     aView,
                     "VkRTGameForwardDrawer_" + aName,
                     aCombinationType)
  {
    Initialize();
  }

  VkRTWaterDrawer::VkRTWaterDrawer(VkRenderedSurface *aSurface,
                                   vk::Format aColorFormat,
                                   vk::Format aDepthFormat,
                                   std::shared_ptr<vkhlf::Surface>& aVulkanSurface,
                                   std::string aName,
                                   YTEDrawerTypeCombination aCombinationType)
    : VkRenderTarget(aSurface,
                     aColorFormat,
                     aDepthFormat,
                     aVulkanSurface,
                     "VkRTGameForwardDrawer_" + aName,
                     aCombinationType)
  {
    Initialize();
  }

  VkRTWaterDrawer::~VkRTWaterDrawer()
  {
    //VkRenderTarget::~VkRenderTarget();
  }

  void VkRTWaterDrawer::RenderFull(const vk::Extent2D& aExtent,
                                   std::unordered_map<std::string, std::unique_ptr<VkMesh>>& aMeshes)
  {
    mCBOB->NextCommandBuffer();
    auto cbo = mCBOB->GetCurrentCBO();
    cbo->begin(vk::CommandBufferUsageFlagBits::eRenderPassContinue, mRenderPass);
    RenderRefractive(cbo, aExtent, aMeshes);
    cbo->end();

    mReflectiveCBOB->NextCommandBuffer();
    auto reflectiveCBO = mReflectiveCBOB->GetCurrentCBO();
    reflectiveCBO->begin(vk::CommandBufferUsageFlagBits::eRenderPassContinue, mReflectiveRenderPass);
    RenderReflective(cbo, aExtent, aMeshes);
    reflectiveCBO->end();
  }


  /*
  void VkRTWaterDrawer::Render(std::shared_ptr<vkhlf::CommandBuffer>& aCBO, const vk::Extent2D& extent, std::unordered_map<std::string, std::unique_ptr<VkMesh>>& aMeshes)
  {
    auto width = static_cast<float>(extent.width);
    auto height = static_cast<float>(extent.height);

    vk::Viewport viewport{ 0.0f, 0.0f, width, height, 0.0f,1.0f };
    aCBO->setViewport(0, viewport);

    vk::Rect2D scissor{ { 0, 0 }, extent };
    aCBO->setScissor(0, scissor);

    auto &instantiatedModels = mParentViewData->mInstantiatedModels;

    for (auto &shader : mParentViewData->mShaders)
    {
      auto &pipeline = shader.second->mShader;

      aCBO->bindPipeline(vk::PipelineBindPoint::eGraphics, pipeline);
      for (auto &mesh : aMeshes)
      {
        auto &models = instantiatedModels[mesh.second.get()];

        // We can early out on this mesh if there are no models that use it.
        if (models.empty())
        {
          continue;
        }

        // We get the sub meshes that use the current shader, then draw them.
        auto range = mesh.second->mSubmeshes.equal_range(shader.first);

        if (mesh.second->GetInstanced())
        {
          aCBO->bindVertexBuffer(1,
                                 mesh.second->mInstanceManager.InstanceBuffer(),
                                 0);
        }

        for (auto it = range.first; it != range.second; ++it)
        {
          auto &submesh = it->second;

          aCBO->bindVertexBuffer(0,
                                 submesh->mVertexBuffer,
                                 0);

          aCBO->bindIndexBuffer(submesh->mIndexBuffer,
                                0,
                                vk::IndexType::eUint32);



          if (mesh.second->GetInstanced())
          {
            auto data = submesh->mPipelineData;
            aCBO->bindDescriptorSets(vk::PipelineBindPoint::eGraphics,
                                     data.mPipelineLayout,
                                     0,
                                     data.mDescriptorSet,
                                     nullptr);

            aCBO->drawIndexed(static_cast<u32>(submesh->mIndexCount),
                              1,
                              0,
                              0,
                              0);
          }
          else
          {
            for (auto &model : models)
            {
              if (model->mUseAdditiveBlending || model->mBackFaceCull == false)
              {
                continue;
              }
              auto &data = model->mPipelineData[submesh.get()];
              aCBO->bindDescriptorSets(vk::PipelineBindPoint::eGraphics,
                                       data.mPipelineLayout,
                                       0,
                                       data.mDescriptorSet,
                                       nullptr);

              aCBO->drawIndexed(static_cast<u32>(submesh->mIndexCount),
                                1,
                                0,
                                0,
                                0);
            }
          }
        }
      }
    }

    // pass for additive
    for (auto &shader : mParentViewData->mShaders)
    {
      auto &pipeline = shader.second->mAdditiveBlendShader;

      for (auto &mesh : aMeshes)
      {
        auto &models = instantiatedModels[mesh.second.get()];

        // We can early out on this mesh if there are no models that use it.
        if (models.empty())
        {
          continue;
        }

        // We get the sub meshes that use the current shader, then draw them.
        auto range = mesh.second->mSubmeshes.equal_range(shader.first);

        if (mesh.second->GetInstanced())
        {
          aCBO->bindVertexBuffer(1,
                                 mesh.second->mInstanceManager.InstanceBuffer(),
                                 0);
        }

        for (auto it = range.first; it != range.second; ++it)
        {
          auto &submesh = it->second;

          aCBO->bindVertexBuffer(0,
                                 submesh->mVertexBuffer,
                                 0);

          aCBO->bindIndexBuffer(submesh->mIndexBuffer,
                                0,
                                vk::IndexType::eUint32);



          if (mesh.second->GetInstanced())
          {
            auto data = submesh->mPipelineData;
            aCBO->bindDescriptorSets(vk::PipelineBindPoint::eGraphics,
                                     data.mPipelineLayout,
                                     0,
                                     data.mDescriptorSet,
                                     nullptr);

            aCBO->drawIndexed(static_cast<u32>(submesh->mIndexCount),
                              1,
                              0,
                              0,
                              0);
          }
          else
          {
            for (auto &model : models)
            {
              if (model->mUseAdditiveBlending == false)
              {
                continue;
              }


              aCBO->bindPipeline(vk::PipelineBindPoint::eGraphics, pipeline);

              auto &data = model->mPipelineData[submesh.get()];
              aCBO->bindDescriptorSets(vk::PipelineBindPoint::eGraphics,
                                       data.mPipelineLayout,
                                       0,
                                       data.mDescriptorSet,
                                       nullptr);

              aCBO->drawIndexed(static_cast<u32>(submesh->mIndexCount),
                                1,
                                0,
                                0,
                                0);
            }
          }
        }
      }
    }

    // pass for no cull
    for (auto &shader : mParentViewData->mShaders)
    {
      auto &pipeline = shader.second->mShaderNoCull;

      for (auto &mesh : aMeshes)
      {
        auto &models = instantiatedModels[mesh.second.get()];

        // We can early out on this mesh if there are no models that use it.
        if (models.empty())
        {
          continue;
        }

        // We get the sub meshes that use the current shader, then draw them.
        auto range = mesh.second->mSubmeshes.equal_range(shader.first);

        if (mesh.second->GetInstanced())
        {
          aCBO->bindVertexBuffer(1,
                                 mesh.second->mInstanceManager.InstanceBuffer(),
                                 0);
        }

        for (auto it = range.first; it != range.second; ++it)
        {
          auto &submesh = it->second;

          aCBO->bindVertexBuffer(0,
                                 submesh->mVertexBuffer,
                                 0);

          aCBO->bindIndexBuffer(submesh->mIndexBuffer,
                                0,
                                vk::IndexType::eUint32);



          if (mesh.second->GetInstanced())
          {
            auto data = submesh->mPipelineData;
            aCBO->bindDescriptorSets(vk::PipelineBindPoint::eGraphics,
                                     data.mPipelineLayout,
                                     0,
                                     data.mDescriptorSet,
                                     nullptr);

            aCBO->drawIndexed(static_cast<u32>(submesh->mIndexCount),
                              1,
                              0,
                              0,
                              0);
          }
          else
          {
            for (auto &model : models)
            {
              if (model->mBackFaceCull == true)
              {
                continue;
              }

              aCBO->bindPipeline(vk::PipelineBindPoint::eGraphics, pipeline);

              auto &data = model->mPipelineData[submesh.get()];
              aCBO->bindDescriptorSets(vk::PipelineBindPoint::eGraphics,
                                       data.mPipelineLayout,
                                       0,
                                       data.mDescriptorSet,
                                       nullptr);

              aCBO->drawIndexed(static_cast<u32>(submesh->mIndexCount),
                                1,
                                0,
                                0,
                                0);
            }
          }
        }
      }
    }
  }
  */

  void VkRTWaterDrawer::RenderRefractive(std::shared_ptr<vkhlf::CommandBuffer>& aCBO,
                                         const vk::Extent2D& extent,
                                         std::unordered_map<std::string, std::unique_ptr<VkMesh>>& aMeshes)
  {

  }

  void VkRTWaterDrawer::RenderReflective(std::shared_ptr<vkhlf::CommandBuffer>& aCBO,
                                         const vk::Extent2D& extent,
                                         std::unordered_map<std::string, std::unique_ptr<VkMesh>>& aMeshes)
  {

  }

  void VkRTWaterDrawer::Initialize()
  {

  }

  void VkRTWaterDrawer::Resize(vk::Extent2D& aExtent)
  {

  }

  void VkRTWaterDrawer::ExecuteCommands(std::shared_ptr<vkhlf::CommandBuffer>& aCBO)
  {

  }

  void VkRTWaterDrawer::ExecuteSecondaryEvent(std::shared_ptr<vkhlf::CommandBuffer>& aCBO)
  {

  }

  void VkRTWaterDrawer::MoveToNextEvent()
  {

  }

  void VkRTWaterDrawer::NotifyWaterComponent()
  {

  }

  void VkRTWaterDrawer::CreateReflectiveRenderPass()
  {

  }

  void VkRTWaterDrawer::CreateReflectiveFrameBuffer()
  {
  
  }

  void VkRTWaterDrawer::CreateRefractiveRenderPass()
  {

  }

  void VkRTWaterDrawer::CreateRefractiveFrameBuffer()
  {

  }
}