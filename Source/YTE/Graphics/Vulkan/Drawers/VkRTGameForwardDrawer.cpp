#include "YTE/Graphics/Vulkan/Drawers/VkRTGameForwardDrawer.hpp"
#include "YTE/Graphics/Vulkan/VkRenderer.hpp"
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


  namespace detail
  {
    // Derived from https://math.stackexchange.com/a/1463487
    float ExtractMaximumUniformScale(glm::mat4 const& aModelMatrix)
    {
      auto x = glm::length(aModelMatrix[0]);
      auto y = glm::length(aModelMatrix[1]);
      auto z = glm::length(aModelMatrix[2]);

      return std::max(x, std::max(y, z));
    }
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
    OPTICK_EVENT();

    Initialize();
  }

  VkRTGameForwardDrawer::~VkRTGameForwardDrawer()
  {
  }

  void VkRTGameForwardDrawer::RenderFull(std::unordered_map<std::string, std::unique_ptr<VkMesh>>& aMeshes)
  {
    OPTICK_EVENT();

    ++(*mCBOB);

    auto [graphicsCommandBuffer, graphicsFence] = **mCBOB;
    auto& cbo = graphicsCommandBuffer;

    cbo->begin(vk::CommandBufferUsageFlagBits::eRenderPassContinue, 
               mRenderPass,
               0,
               GetFrameBuffer());

    Render(cbo, aMeshes);

    cbo->end();
  }

  static void RunPipelines(std::shared_ptr<vkhlf::CommandBuffer>& aCBO,
                           std::vector<VkRTGameForwardDrawer::DrawData>& aShaders)
  {
    OPTICK_EVENT();
    auto number = std::to_string(aShaders.size());
    OPTICK_TAG("DrawDatas:", number.c_str());

    {

      std::vector<std::shared_ptr<vkhlf::Buffer>> vertexBuffersToBind;
      std::vector<u64> vertexBufferOffsets;
      std::shared_ptr<vkhlf::Pipeline> *lastPipeline{ nullptr };
      float lastLineWidth = 1.0f;

      for (auto &drawCall : aShaders)
      {
        if (lastPipeline != drawCall.mPipeline)
        {
          OPTICK_EVENT("CommandBuffer Recording: bindPipeline");

          aCBO->bindPipeline(vk::PipelineBindPoint::eGraphics,
                             *drawCall.mPipeline);

          lastPipeline = drawCall.mPipeline;
        }

        if (lastLineWidth != drawCall.mLineWidth)
        {
          OPTICK_EVENT("CommandBuffer Recording: setLineWidth");

          aCBO->setLineWidth(drawCall.mLineWidth);
          lastLineWidth = drawCall.mLineWidth;
        }

        {
          OPTICK_EVENT("CommandBuffer Recording: bindVertexBuffer");
          auto& vbd = *(drawCall.mVertexBufferData);

          if (vbd.mPositionBuffer) vertexBuffersToBind.emplace_back(GetBuffer(vbd.mPositionBuffer));
          if (vbd.mTextureCoordinatesBuffer) vertexBuffersToBind.emplace_back(GetBuffer(vbd.mTextureCoordinatesBuffer));
          if (vbd.mNormalBuffer) vertexBuffersToBind.emplace_back(GetBuffer(vbd.mNormalBuffer));
          if (vbd.mColorBuffer) vertexBuffersToBind.emplace_back(GetBuffer(vbd.mColorBuffer));
          if (vbd.mTangentBuffer) vertexBuffersToBind.emplace_back(GetBuffer(vbd.mTangentBuffer));
          if (vbd.mBinormalBuffer) vertexBuffersToBind.emplace_back(GetBuffer(vbd.mBinormalBuffer));
          if (vbd.mBitangentBuffer) vertexBuffersToBind.emplace_back(GetBuffer(vbd.mBitangentBuffer));
          if (vbd.mBoneWeightsBuffer) vertexBuffersToBind.emplace_back(GetBuffer(vbd.mBoneWeightsBuffer));
          if (vbd.mBoneWeights2Buffer) vertexBuffersToBind.emplace_back(GetBuffer(vbd.mBoneWeights2Buffer));
          if (vbd.mBoneIDsBuffer) vertexBuffersToBind.emplace_back(GetBuffer(vbd.mBoneIDsBuffer));
          if (vbd.mBoneIDs2Buffer) vertexBuffersToBind.emplace_back(GetBuffer(vbd.mBoneIDs2Buffer));

          vertexBufferOffsets.resize(vertexBuffersToBind.size(), 0);

          aCBO->bindVertexBuffers(
            0,
            vertexBuffersToBind,
            vertexBufferOffsets);
        }

        {
          OPTICK_EVENT("CommandBuffer Recording: bindIndexBuffer");

          aCBO->bindIndexBuffer(
            *drawCall.mIndexBuffer,
            0,
            vk::IndexType::eUint32);
        }

        {
          OPTICK_EVENT("CommandBuffer Recording: bindDescriptorSets");

          aCBO->bindDescriptorSets(
            vk::PipelineBindPoint::eGraphics,
            *drawCall.mPipelineLayout,
            0,
            *drawCall.mDescriptorSet,
            nullptr); 
        }

        {
          OPTICK_EVENT("CommandBuffer Recording: drawIndexed");

          aCBO->drawIndexed(
            drawCall.mIndexCount,
            drawCall.mInstanceCount,
            0,
            0,
            0);
        }

        vertexBuffersToBind.clear();
      }

      aShaders.clear();
    }
  }

  void VkRTGameForwardDrawer::Render(std::shared_ptr<vkhlf::CommandBuffer>& aCBO,
                                     std::unordered_map<std::string, std::unique_ptr<VkMesh>>& aMeshes)
  {
    OPTICK_EVENT();

    auto const& viewUbo = mParentViewData->mView->GetViewUBOData();
    auto toClipSpace = viewUbo.mProjectionMatrix *
                       viewUbo.mViewMatrix;

    auto& frustum = mParentViewData->mView->GetFrustum();

    glm::vec4 origin{ 0.f,0.f,0.f,1.f };
    float depth{ 0.0f };

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
        auto instantiatedModelsIt = instantiatedModels.find(mesh.second.get());
        
        // We can early out on this mesh if there are no models that use it.
        if (instantiatedModelsIt ==  instantiatedModels.end())
        {
          continue;
        }

        auto &models = instantiatedModelsIt->second;

        // We get the sub meshes that use the current shader
        auto range = mesh.second->mSubmeshMap.equal_range(shader.first);

        for (auto it = range.first; it != range.second; ++it)
        {
          auto &submesh = it->second;
          auto trueSubmesh = submesh->mSubmesh;

          for (auto &model : models)
          {
            auto submeshDimension = trueSubmesh->mData.mDimension;
            auto subMeshPosition = submeshDimension.GetCenter();
            auto modelMatrix = model->GetUBOModelData().mModelMatrix;
            modelMatrix[3][0] += subMeshPosition.x;
            modelMatrix[3][1] += subMeshPosition.y;
            modelMatrix[3][2] += subMeshPosition.z;

            auto position = modelMatrix * origin;
            auto radiusScale = detail::ExtractMaximumUniformScale(modelMatrix);

            bool visible = true;

            // TODO: Need to bugfix this.
            if (CompilerConfiguration::Debug())
            {
              visible = frustum.CheckSphere(glm::vec3(position), submeshDimension.GetRadius() * radiusScale);
            }

            if ((visible == false) || (false == model->GetVisibility()))
            {
              continue;
            }

            auto &data = model->mPipelineData[submesh];

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

                auto clipPosition = toClipSpace * model->GetUBOModelData().mModelMatrix * origin;
                depth = clipPosition.z;
                break;
              }
              default:
              {
                assert(false);
              }
            }

            toEmplaceInto->emplace_back(*toUseToDraw,
                                        trueSubmesh->mVertexBufferData,
                                        GetBuffer(trueSubmesh->mIndexBuffer),
                                        data.mPipelineLayout,
                                        data.mDescriptorSet,
                                        static_cast<u32>(trueSubmesh->mData.mIndexData.size()),
                                        model->mLineWidth,
                                        depth);
          }
        }
      }
    }

    // A naive alpha sorting algorithm that places objects in clip
    // space, then sorts their z (depth). It's not perfect, but solves
    // most naive issues (UI sorting issues, most particle issues).
    {
      OPTICK_EVENT("Sorting Alpha");

      std::sort(mAlphaBlendShader.begin(), 
                mAlphaBlendShader.end(), 
                [](DrawData const &aLeft, DrawData const &aRight)
      {
        return aLeft.mDepth > aRight.mDepth;
      });
    }

    {
      OPTICK_EVENT("Drawing Triangles");

      RunPipelines(aCBO, mTriangles);
    }

    {
      OPTICK_EVENT("Drawing Lines");

      RunPipelines(aCBO, mLines);
    }

    {
      OPTICK_EVENT("Drawing Curves");

      RunPipelines(aCBO, mCurves);
    }

    {
      OPTICK_EVENT("Drawing ShaderNoCull");

      RunPipelines(aCBO, mShaderNoCull);
    }

    {
      OPTICK_EVENT("Drawing AdditiveBlendShader");

      RunPipelines(aCBO, mAdditiveBlendShader);
    }

    {
      OPTICK_EVENT("Drawing AlphaBlendShader");

      RunPipelines(aCBO, mAlphaBlendShader);
    }
  }
}