#pragma once

#ifndef YTE_Graphics_Vulkan_Drawers_VkRTGameForwardDrawer_hpp
#define YTE_Graphics_Vulkan_Drawers_VkRTGameForwardDrawer_hpp

#include "YTE/Graphics/Generics/Mesh.hpp"

#include "YTE/Graphics/Vulkan/Drawers/VkRenderTarget.hpp"

namespace YTE
{
  class VkRTGameForwardDrawer : public VkRenderTarget
  {
  public:
    YTEDeclareType(VkRTGameForwardDrawer);

    VkRTGameForwardDrawer(VkRenderedSurface *aSurface,
                          vk::Format aColorFormat,
                          vk::Format aDepthFormat,
                          std::shared_ptr<vkhlf::Surface>& aVulkanSurface,
                          ViewData* aView,
                          DrawerTypeCombination aCombination = DrawerTypeCombination::DefaultCombination);

    virtual ~VkRTGameForwardDrawer() override;

    virtual void RenderFull(std::unordered_map<std::string, std::unique_ptr<VkMesh>>& aMeshes) override;
    void Render(std::shared_ptr<vkhlf::CommandBuffer>& aCBO,
                std::unordered_map<std::string, std::unique_ptr<VkMesh>>& aMeshes);


    struct DrawData
    {
      DrawData(std::shared_ptr<vkhlf::Pipeline>& aPipeline,
               VertexBufferData& aVertexBuffer,
               std::shared_ptr<vkhlf::Buffer>& aIndexBuffer,
               std::shared_ptr<vkhlf::PipelineLayout>& aPipelineLayout,
               std::shared_ptr<vkhlf::DescriptorSet>& aDescriptorSet,
               u32 aIndexCount,
               float aLineWidth,
               float aDepth )
        : mPipeline{ &aPipeline}
        , mVertexBufferData{ &aVertexBuffer }
        , mIndexBuffer{ &aIndexBuffer }
        , mPipelineLayout{ &aPipelineLayout }
        , mDescriptorSet{ &aDescriptorSet }
        , mIndexCount{aIndexCount}
        , mLineWidth(aLineWidth)
        , mDepth{ aDepth }
      {

      }

      bool operator<(const DrawData& aRight)
      {

        return mPipeline < aRight.mPipeline&&
          mPipelineLayout < aRight.mPipelineLayout&&
          mDescriptorSet < aRight.mDescriptorSet&&
          mVertexBufferData < aRight.mVertexBufferData&&
          mIndexBuffer < aRight.mIndexBuffer&&
          mInstanceBuffer < aRight.mInstanceBuffer;
      }

      std::shared_ptr<vkhlf::Pipeline>* mPipeline;
      VertexBufferData* mVertexBufferData;
      std::shared_ptr<vkhlf::Buffer>* mIndexBuffer;
      std::shared_ptr<vkhlf::Buffer>* mInstanceBuffer = nullptr;
      std::shared_ptr<vkhlf::PipelineLayout> *mPipelineLayout;
      std::shared_ptr<vkhlf::DescriptorSet> *mDescriptorSet;
      glm::mat4 *mModelMatrix;
      u32 mIndexCount;
      u32 mInstanceCount = 1;
      float mLineWidth;
      float mDepth;
    };

    std::vector<DrawData> mTriangles;
    std::vector<DrawData> mLines;
    std::vector<DrawData> mCurves;
    std::vector<DrawData> mShaderNoCull;
    std::vector<DrawData> mAdditiveBlendShader;
    std::vector<DrawData> mAlphaBlendShader;
  };
}

#endif