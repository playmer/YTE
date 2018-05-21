///////////////////
// Author: Andrew Griffin
// YTE - Graphics - Vulkan - Drawers
///////////////////

#include "YTE/Graphics/Vulkan/Drawers/VkRTGameForwardDrawer.hpp"
#include "YTE/Graphics/Vulkan/VkRenderedSurface.hpp"
#include "YTE/Graphics/Vulkan/VkInstantiatedModel.hpp"
#include "YTE/Graphics/Vulkan/VkMesh.hpp"
#include "YTE/Graphics/Vulkan/VkShader.hpp"

namespace YTE
{
  YTEDefineType(VkRTGameForwardDrawer)
  {
    RegisterType<VkRTGameForwardDrawer>();
    TypeBuilder<VkRTGameForwardDrawer> builder;
  }

  VkRTGameForwardDrawer::VkRTGameForwardDrawer(VkRenderedSurface *aSurface,
                                               vk::Format aColorFormat,
                                               vk::Format aDepthFormat,
                                               std::shared_ptr<vkhlf::Surface>& aVulkanSurface,
                                               ViewData* aView,
                                               DrawerTypeCombination aCombinationType)
    : VkRenderTarget(aSurface, 
                     aColorFormat,
                     aDepthFormat,
                     aVulkanSurface,
                     aView,
                     "VkRTGameForwardDrawer_" + aView->mName,
                     aCombinationType)
  {
    Initialize();
  }

  VkRTGameForwardDrawer::~VkRTGameForwardDrawer()
  {
  }

  void VkRTGameForwardDrawer::RenderFull(std::unordered_map<std::string, std::unique_ptr<VkMesh>>& aMeshes)
  {
    mCBOB->NextCommandBuffer();
    auto cbo = mCBOB->GetCurrentCBO();
    cbo->begin(vk::CommandBufferUsageFlagBits::eRenderPassContinue, 
               mRenderPass,
               0,
               GetFrameBuffer());
    Render(cbo, aMeshes);
    cbo->end();
  }

  void VkRTGameForwardDrawer::Render(std::shared_ptr<vkhlf::CommandBuffer>& aCBO,
                                     std::unordered_map<std::string, std::unique_ptr<VkMesh>>& aMeshes)
  {
    auto width = static_cast<float>(mData.mExtent.width);
    auto height = static_cast<float>(mData.mExtent.height);

    vk::Viewport viewport{ 0.0f, 0.0f, width, height, 0.0f,1.0f };
    aCBO->setViewport(0, viewport);
    vk::Rect2D scissor{ { 0, 0 }, mData.mExtent };
    aCBO->setScissor(0, scissor);
    aCBO->setLineWidth(1.0f);

    auto &instantiatedModels = mParentViewData->mInstantiatedModels;

    for (auto &shader : mParentViewData->mShaders)
    {
      for (auto &mesh : aMeshes)
      {
        auto &models = instantiatedModels[mesh.second.get()];
        
        // We can early out on this mesh if there are no models that use it.
        if (models.empty())
        {
          continue;
        }

        // We get the sub meshes that use the current shader
        auto range = mesh.second->mSubmeshes.equal_range(shader.first);

        for (auto it = range.first; it != range.second; ++it)
        {
          auto &submesh = it->second;

          for (auto &model : models)
          {
            if (false == model->GetVisibility())
            {
              continue;
            }

            auto &data = model->mPipelineData[submesh.get()];

            // Gather up all the data for the individual passes.
            std::shared_ptr<vkhlf::Pipeline> *toUseToDraw{ nullptr };
            std::vector<DrawData> *toEmplaceInto{ nullptr };

            switch (model->mType)
            {
              case ShaderType::Triangles:
              {
                toUseToDraw = &shader.second->mTriangles;
                toEmplaceInto = &mTriangles;
                break;
              }
              case ShaderType::Lines:
              {
                toUseToDraw = &shader.second->mLines;
                toEmplaceInto = &mLines;
                break;
              }
              case ShaderType::Curves:
              {
                toUseToDraw = &shader.second->mCurves;
                toEmplaceInto = &mCurves;
                break;
              }
              case ShaderType::Wireframe:
              {
                toUseToDraw = &shader.second->mCurves;
                toEmplaceInto = &mCurves;
                break;
              }
              case ShaderType::ShaderNoCull:
              {
                toUseToDraw = &shader.second->mShaderNoCull;
                toEmplaceInto = &mShaderNoCull;
                break;
              }
              case ShaderType::AdditiveBlendShader:
              {
                toUseToDraw = &shader.second->mAdditiveBlendShader;
                toEmplaceInto = &mAdditiveBlendShader;
                break;
              }
              case ShaderType::AlphaBlendShader:
              {
                toUseToDraw = &shader.second->mAlphaBlendShader;
                toEmplaceInto = &mAlphaBlendShader;
                break;
              }
              default:
              {
                assert(false);
              }
            }

            toEmplaceInto->emplace_back(*toUseToDraw,
                                        submesh->mVertexBuffer,
                                        submesh->mIndexBuffer,
                                        data.mPipelineLayout,
                                        data.mDescriptorSet,
                                        model->GetUBOModelData().mModelMatrix,
                                        static_cast<u32>(submesh->mIndexCount),
                                        model->mLineWidth);
          }
        }
      }
    }


    auto runPipelines = [](std::shared_ptr<vkhlf::CommandBuffer> &aCBO, 
                           std::vector<DrawData> &aShaders)
    {
      std::shared_ptr<vkhlf::Pipeline> *lastPipeline{ nullptr };
      float lastLineWidth = 1.0f;

      for (auto &drawCall : aShaders)
      {
        if (lastPipeline != drawCall.mPipeline)
        {
          aCBO->bindPipeline(vk::PipelineBindPoint::eGraphics,
                             *drawCall.mPipeline);

          lastPipeline = drawCall.mPipeline;
        }

        if (lastLineWidth != drawCall.mLineWidth)
        {
          aCBO->setLineWidth(drawCall.mLineWidth);
          lastLineWidth = drawCall.mLineWidth;
        }

        aCBO->bindVertexBuffer(0,
                               *drawCall.mVertexBuffer,
                               0);

        aCBO->bindIndexBuffer(*drawCall.mIndexBuffer,
                              0,
                              vk::IndexType::eUint32);

        aCBO->bindDescriptorSets(vk::PipelineBindPoint::eGraphics,
                                 *drawCall.mPipelineLayout,
                                 0,
                                 *drawCall.mDescriptorSet,
                                 nullptr);

        aCBO->drawIndexed(static_cast<u32>(drawCall.mIndexCount),
                          1,
                          0,
                          0,
                          0);
      }

      aShaders.clear();
    };


    // A naive alpha sorting algorithm that places objects in clip
    // space, then sorts their z (depth). It's not perfect, but solves
    // most naive issues (UI sorting issues, most particle issues).
    {
      YTEProfileBlock("Sorting Alpha");

      auto toClipSpace = mParentViewData->mViewUBOData.mProjectionMatrix *
                         mParentViewData->mViewUBOData.mViewMatrix;

      glm::vec4 origin{ 0.f,0.f,0.f,1.f };
      for (auto &data : mAlphaBlendShader)
      {
        auto position = toClipSpace * (*data.mModelMatrix) * origin;
        data.mDepth = position.z;
      }

      std::sort(mAlphaBlendShader.begin(), 
                mAlphaBlendShader.end(), 
                [](DrawData const &aLeft, DrawData const &aRight)
      {
        return aLeft.mDepth > aRight.mDepth;
      });
    }

    runPipelines(aCBO, mTriangles);
    runPipelines(aCBO, mLines);
    runPipelines(aCBO, mCurves);
    runPipelines(aCBO, mShaderNoCull);
    runPipelines(aCBO, mAdditiveBlendShader);


    runPipelines(aCBO, mAlphaBlendShader);
  }
}