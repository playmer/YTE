#pragma once

#ifndef YTE_Graphics_Vulkan_Drawers_VkRTGameForwardDrawer_hpp
#define YTE_Graphics_Vulkan_Drawers_VkRTGameForwardDrawer_hpp

#include "YTE/Graphics/DirectX12/Drawers/DX12RenderTarget.hpp"

namespace YTE
{
  class DX12RTGameForwardDrawer : public DX12RenderTarget
  {
  public:
    YTEDeclareType(DX12RTGameForwardDrawer);

    DX12RTGameForwardDrawer(Dx12RenderedSurface *aSurface,
                          vk::Format aColorFormat,
                          vk::Format aDepthFormat,
                          std::shared_ptr<vkhlf::Surface>& aVulkanSurface,
                          DX12ViewData* aView,
                          DrawerTypeCombination aCombination = DrawerTypeCombination::DefaultCombination);

    virtual ~DX12RTGameForwardDrawer() override;

    virtual void RenderFull(std::unordered_map<std::string, std::unique_ptr<DX12Mesh>>& aMeshes) override;
    void Render(std::shared_ptr<vkhlf::CommandBuffer>& aCBO,
                std::unordered_map<std::string, std::unique_ptr<DX12Mesh>>& aMeshes);


    struct DrawData
    {
      DrawData(//std::shared_ptr<vkhlf::Pipeline> &aPipeline,
               //std::shared_ptr<vkhlf::Buffer> &aVertexBuffer,
               //std::shared_ptr<vkhlf::Buffer> &aIndexBuffer,
               //std::shared_ptr<vkhlf::PipelineLayout> &aPipelineLayout,
               //std::shared_ptr<vkhlf::DescriptorSet> &aDescriptorSet,
               glm::mat4 &aModelMatrix,
               u32 aIndexCount,
               float aLineWidth)
        //: mPipeline{ &aPipeline}
        //, mVertexBuffer{ &aVertexBuffer}
        //, mIndexBuffer{ &aIndexBuffer}
        //, mPipelineLayout{ &aPipelineLayout}
        //, mDescriptorSet{ &aDescriptorSet }
        : mModelMatrix{ &aModelMatrix }
        , mIndexCount{aIndexCount}
        , mLineWidth(aLineWidth)
      {

      }

      //std::shared_ptr<vkhlf::Pipeline> *mPipeline;
      //std::shared_ptr<vkhlf::Buffer> *mVertexBuffer;
      //std::shared_ptr<vkhlf::Buffer> *mIndexBuffer;
      //std::shared_ptr<vkhlf::PipelineLayout> *mPipelineLayout;
      //std::shared_ptr<vkhlf::DescriptorSet> *mDescriptorSet;
      glm::mat4 *mModelMatrix;
      u32 mIndexCount;
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