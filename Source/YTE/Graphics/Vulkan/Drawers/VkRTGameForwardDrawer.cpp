///////////////////
// Author: Andrew Griffin
// YTE - Graphics - Vulkan - Drawers
///////////////////

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
    // Adapted From Sascha Willems
    // https://github.com/SaschaWillems/Vulkan/blob/master/base/frustum.hpp
    class Frustum
    {
    public:
      enum side { LEFT = 0, RIGHT = 1, TOP = 2, BOTTOM = 3, BACK = 4, FRONT = 5 };
      std::array<glm::vec4, 6> mPlanes;
      glm::vec3 mCameraPosition;

      void Update(glm::mat4 const& aClipSpace, glm::vec4 const& aCameraPosition)
      {
        mCameraPosition = glm::vec3(aCameraPosition);

        mPlanes[LEFT].x = aClipSpace[0].w + aClipSpace[0].x;
        mPlanes[LEFT].y = aClipSpace[1].w + aClipSpace[1].x;
        mPlanes[LEFT].z = aClipSpace[2].w + aClipSpace[2].x;
        mPlanes[LEFT].w = aClipSpace[3].w + aClipSpace[3].x;

        mPlanes[RIGHT].x = aClipSpace[0].w - aClipSpace[0].x;
        mPlanes[RIGHT].y = aClipSpace[1].w - aClipSpace[1].x;
        mPlanes[RIGHT].z = aClipSpace[2].w - aClipSpace[2].x;
        mPlanes[RIGHT].w = aClipSpace[3].w - aClipSpace[3].x;

        mPlanes[TOP].x = aClipSpace[0].w - aClipSpace[0].y;
        mPlanes[TOP].y = aClipSpace[1].w - aClipSpace[1].y;
        mPlanes[TOP].z = aClipSpace[2].w - aClipSpace[2].y;
        mPlanes[TOP].w = aClipSpace[3].w - aClipSpace[3].y;

        mPlanes[BOTTOM].x = aClipSpace[0].w + aClipSpace[0].y;
        mPlanes[BOTTOM].y = aClipSpace[1].w + aClipSpace[1].y;
        mPlanes[BOTTOM].z = aClipSpace[2].w + aClipSpace[2].y;
        mPlanes[BOTTOM].w = aClipSpace[3].w + aClipSpace[3].y;

        mPlanes[BACK].x = aClipSpace[0].w + aClipSpace[0].z;
        mPlanes[BACK].y = aClipSpace[1].w + aClipSpace[1].z;
        mPlanes[BACK].z = aClipSpace[2].w + aClipSpace[2].z;
        mPlanes[BACK].w = aClipSpace[3].w + aClipSpace[3].z;

        mPlanes[FRONT].x = aClipSpace[0].w - aClipSpace[0].z;
        mPlanes[FRONT].y = aClipSpace[1].w - aClipSpace[1].z;
        mPlanes[FRONT].z = aClipSpace[2].w - aClipSpace[2].z;
        mPlanes[FRONT].w = aClipSpace[3].w - aClipSpace[3].z;

        for (auto i = 0; i < mPlanes.size(); i++)
        {
          float length = sqrtf(mPlanes[i].x * mPlanes[i].x + mPlanes[i].y * mPlanes[i].y + mPlanes[i].z * mPlanes[i].z);
          mPlanes[i] /= length;
        }
      }

      bool CheckSphere(glm::vec3 aPosition, float aRadius)
      {
        YTEProfileFunction();

        // Check to see if Camera is within the sphere, if so, just draw it.
        auto distance = glm::length(mCameraPosition - aPosition);

        if (distance < aRadius)
        {
          return true;
        }

        // Next check to see if the sphere is within the planes.
        for (auto i = 0; i < mPlanes.size(); i++)
        {
          if ((mPlanes[i].x * aPosition.x) + (mPlanes[i].y * aPosition.y) + (mPlanes[i].z * aPosition.z) + mPlanes[i].w <= -aRadius)
          {
            return false;
          }
        }
        return true;
      }
    };

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
    YTEProfileFunction();

    Initialize();
  }

  VkRTGameForwardDrawer::~VkRTGameForwardDrawer()
  {
  }

  void VkRTGameForwardDrawer::RenderFull(std::unordered_map<std::string, std::unique_ptr<VkMesh>>& aMeshes)
  {
    YTEProfileFunction();

    mCBOB->NextCommandBuffer();
    auto cbo = mCBOB->GetCurrentCBO();
    cbo->begin(vk::CommandBufferUsageFlagBits::eRenderPassContinue, 
               mRenderPass,
               0,
               GetFrameBuffer());
    Render(cbo, aMeshes);
    cbo->end();
  }

  static void RunPipelines(std::shared_ptr<vkhlf::CommandBuffer> &aCBO,
                           std::vector<VkRTGameForwardDrawer::DrawData> &aShaders)
  {
    YTEProfileFunction();

    {
      auto number = std::to_string(aShaders.size());
      YTEProfileBlock(number.c_str());

      std::shared_ptr<vkhlf::Pipeline> *lastPipeline{ nullptr };
      float lastLineWidth = 1.0f;

      for (auto &drawCall : aShaders)
      {
        if (lastPipeline != drawCall.mPipeline)
        {
          YTEProfileBlock("bindPipeline");

          aCBO->bindPipeline(vk::PipelineBindPoint::eGraphics,
                             *drawCall.mPipeline);

          lastPipeline = drawCall.mPipeline;
        }

        if (lastLineWidth != drawCall.mLineWidth)
        {
          YTEProfileBlock("setLineWidth");

          aCBO->setLineWidth(drawCall.mLineWidth);
          lastLineWidth = drawCall.mLineWidth;
        }

        {
          YTEProfileBlock("bindVertexBuffer");

          aCBO->bindVertexBuffer(0,
                                 *drawCall.mVertexBuffer,
                                 0);
        }

        {
          YTEProfileBlock("bindIndexBuffer");

          aCBO->bindIndexBuffer(*drawCall.mIndexBuffer,
                                0,
                                vk::IndexType::eUint32);
        }

        {
          YTEProfileBlock("bindDescriptorSets");

          aCBO->bindDescriptorSets(vk::PipelineBindPoint::eGraphics,
                                   *drawCall.mPipelineLayout,
                                   0,
                                   *drawCall.mDescriptorSet,
                                   nullptr); 
        }

        {
          YTEProfileBlock("drawIndexed");

          aCBO->drawIndexed(static_cast<u32>(drawCall.mIndexCount),
                            1,
                            0,
                            0,
                            0);
        }
      }

      aShaders.clear();
    }
  }

  void VkRTGameForwardDrawer::Render(std::shared_ptr<vkhlf::CommandBuffer>& aCBO,
                                     std::unordered_map<std::string, std::unique_ptr<VkMesh>>& aMeshes)
  {
    YTEProfileFunction();

    auto toClipSpace = mParentViewData->mViewUBOData.mProjectionMatrix *
                       mParentViewData->mViewUBOData.mViewMatrix;

    detail::Frustum frustum;
    frustum.Update(toClipSpace, mParentViewData->mViewUBOData.mCameraPosition);

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
        auto &models = instantiatedModels[mesh.second.get()];
        
        // We can early out on this mesh if there are no models that use it.
        if (models.empty())
        {
          continue;
        }

        // We get the sub meshes that use the current shader
        auto range = mesh.second->mSubmeshMap.equal_range(shader.first);

        for (auto it = range.first; it != range.second; ++it)
        {
          auto &submesh = it->second;

          for (auto &model : models)
          {
            auto submeshDimension = submesh->mSubmesh->mDimension;
            auto subMeshPosition = submeshDimension.GetCenter();
            auto modelMatrix = model->GetUBOModelData().mModelMatrix;
            modelMatrix[3][0] += subMeshPosition.x;
            modelMatrix[3][1] += subMeshPosition.y;
            modelMatrix[3][2] += subMeshPosition.z;

            auto position = modelMatrix * origin;
            auto radiusScale = detail::ExtractMaximumUniformScale(modelMatrix);

            auto visible = frustum.CheckSphere(glm::vec3(position), submeshDimension.GetRadius() * radiusScale);

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
                                        GetBuffer(submesh->mVertexBuffer),
                                        GetBuffer(submesh->mIndexBuffer),
                                        data.mPipelineLayout,
                                        data.mDescriptorSet,
                                        static_cast<u32>(submesh->mIndexCount),
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
      YTEProfileBlock("Sorting Alpha");

      std::sort(mAlphaBlendShader.begin(), 
                mAlphaBlendShader.end(), 
                [](DrawData const &aLeft, DrawData const &aRight)
      {
        return aLeft.mDepth > aRight.mDepth;
      });
    }

    {
      YTEProfileBlock("Triangles");

      RunPipelines(aCBO, mTriangles);
    }

    {
      YTEProfileBlock("Lines");

      RunPipelines(aCBO, mLines);
    }

    {
      YTEProfileBlock("Curves");

      RunPipelines(aCBO, mCurves);
    }

    {
      YTEProfileBlock("ShaderNoCull");

      RunPipelines(aCBO, mShaderNoCull);
    }

    {
      YTEProfileBlock("AdditiveBlendShader");

      RunPipelines(aCBO, mAdditiveBlendShader);
    }

    {
      YTEProfileBlock("AlphaBlendShader");

      RunPipelines(aCBO, mAlphaBlendShader);
    }
  }
}