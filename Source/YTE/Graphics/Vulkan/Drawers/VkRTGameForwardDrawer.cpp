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


  namespace detail
  {
    // Adapted from "Fast Extraction of Viewing Frustum Planes from the World-View-Projection Matrix"
    // https://www.gamedevs.org/uploads/fast-extraction-viewing-frustum-planes-from-world-view-projection-matrix.pdf
    struct Plane
    {
      float a, b, c, d;

      void Normalize()
      {
        float mag = sqrt((a * a) + (b * b) + (c * c));

        a = a / mag;
        b = b / mag;
        c = c / mag;
        d = d / mag;
      }
    };


    float DistanceToPoint(Plane const& aPlane, glm::vec4 const& aPoint)
    {
      return (aPlane.a * aPoint.x) + (aPlane.b * aPoint.y) + (aPlane.c * aPoint.z) + aPlane.d;
    }

    enum class Halfspace : u8
    {
      Negative = -1,
      OnPlane = 0,
      Positive = 1,
    };

    Halfspace ClassifyPoint(Plane const& aPlane, glm::vec4 const& aPoint)
    {
      float d = (aPlane.a * aPoint.x) + (aPlane.b * aPoint.y) + (aPlane.c * aPoint.z) + aPlane.d;

      if (d < 0) 
      {
        return Halfspace::Negative;
      }

      if (d > 0) 
      {
        return Halfspace::Positive;
      }

      return Halfspace::OnPlane;
    }

    // From Sascha Willems
    // https://github.com/SaschaWillems/Vulkan/blob/master/base/frustum.hpp
    class Frustum
    {
    public:
      enum side { LEFT = 0, RIGHT = 1, TOP = 2, BOTTOM = 3, BACK = 4, FRONT = 5 };
      std::array<glm::vec4, 6> planes;

      void update(glm::mat4 matrix)
      {
        planes[LEFT].x = matrix[0].w + matrix[0].x;
        planes[LEFT].y = matrix[1].w + matrix[1].x;
        planes[LEFT].z = matrix[2].w + matrix[2].x;
        planes[LEFT].w = matrix[3].w + matrix[3].x;

        planes[RIGHT].x = matrix[0].w - matrix[0].x;
        planes[RIGHT].y = matrix[1].w - matrix[1].x;
        planes[RIGHT].z = matrix[2].w - matrix[2].x;
        planes[RIGHT].w = matrix[3].w - matrix[3].x;

        planes[TOP].x = matrix[0].w - matrix[0].y;
        planes[TOP].y = matrix[1].w - matrix[1].y;
        planes[TOP].z = matrix[2].w - matrix[2].y;
        planes[TOP].w = matrix[3].w - matrix[3].y;

        planes[BOTTOM].x = matrix[0].w + matrix[0].y;
        planes[BOTTOM].y = matrix[1].w + matrix[1].y;
        planes[BOTTOM].z = matrix[2].w + matrix[2].y;
        planes[BOTTOM].w = matrix[3].w + matrix[3].y;

        planes[BACK].x = matrix[0].w + matrix[0].z;
        planes[BACK].y = matrix[1].w + matrix[1].z;
        planes[BACK].z = matrix[2].w + matrix[2].z;
        planes[BACK].w = matrix[3].w + matrix[3].z;

        planes[FRONT].x = matrix[0].w - matrix[0].z;
        planes[FRONT].y = matrix[1].w - matrix[1].z;
        planes[FRONT].z = matrix[2].w - matrix[2].z;
        planes[FRONT].w = matrix[3].w - matrix[3].z;

        for (auto i = 0; i < planes.size(); i++)
        {
          float length = sqrtf(planes[i].x * planes[i].x + planes[i].y * planes[i].y + planes[i].z * planes[i].z);
          planes[i] /= length;
        }
      }

      bool CheckSphere(glm::vec3 pos, float radius)
      {
        YTEProfileFunction();

        for (auto i = 0; i < planes.size(); i++)
        {
          if ((planes[i].x * pos.x) + (planes[i].y * pos.y) + (planes[i].z * pos.z) + planes[i].w <= -radius)
          {
            return false;
          }
        }
        return true;
      }
    };
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
    frustum.update(toClipSpace);

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
        auto range = mesh.second->mSubmeshes.equal_range(shader.first);

        for (auto it = range.first; it != range.second; ++it)
        {
          auto &submesh = it->second;

          for (auto &model : models)
          {
            auto position = model->GetUBOModelData().mModelMatrix * origin;
            auto visible = frustum.CheckSphere(glm::vec3(position), submesh->mMesh->mMesh->mDimension.GetRadius() * 2.0f);

            if ((visible == false) || (false == model->GetVisibility()))
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
                                        submesh->mVertexBuffer,
                                        submesh->mIndexBuffer,
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