///////////////////
// Author: Andrew Griffin
// YTE - Graphics - Vulkan - Drawers
///////////////////

#pragma once

#ifndef YTE_Graphics_Vulkan_Drawers_VkRTGameForwardDrawer_hpp
#define YTE_Graphics_Vulkan_Drawers_VkRTGameForwardDrawer_hpp

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
    void RenderBegin(std::shared_ptr<vkhlf::CommandBuffer>& aCBO);
    void Render(std::shared_ptr<vkhlf::CommandBuffer>& aCBO,
                std::unordered_map<std::string, std::unique_ptr<VkMesh>>& aMeshes);
    void RenderEnd(std::shared_ptr<vkhlf::CommandBuffer>& aCBO);


    struct DrawData
    {
      DrawData(std::shared_ptr<vkhlf::Pipeline> &aPipeline,
               std::shared_ptr<vkhlf::Buffer> &aVertexBuffer,
               std::shared_ptr<vkhlf::Buffer> &aIndexBuffer,
               std::shared_ptr<vkhlf::PipelineLayout> &aPipelineLayout,
               std::shared_ptr<vkhlf::DescriptorSet> &aDescriptorSet,
               u32 aIndexCount,
               float aLineWidth)
        : mPipeline{aPipeline}
        , mVertexBuffer{aVertexBuffer}
        , mIndexBuffer{aIndexBuffer}
        , mPipelineLayout{aPipelineLayout}
        , mDescriptorSet{aDescriptorSet}
        , mIndexCount{aIndexCount}
        , mLineWidth(aLineWidth)
      {

      }

      std::shared_ptr<vkhlf::Pipeline> &mPipeline;
      std::shared_ptr<vkhlf::Buffer> &mVertexBuffer;
      std::shared_ptr<vkhlf::Buffer> &mIndexBuffer;
      std::shared_ptr<vkhlf::PipelineLayout> &mPipelineLayout;
      std::shared_ptr<vkhlf::DescriptorSet> &mDescriptorSet;
      u32 mIndexCount;
      float mLineWidth;
    };

    std::vector<DrawData> mTriangles;
    std::vector<DrawData> mLines;
    std::vector<DrawData> mCurves;
    std::vector<DrawData> mShaderNoCull;
    std::vector<DrawData> mAdditiveBlendShader;
  };
}

#endif