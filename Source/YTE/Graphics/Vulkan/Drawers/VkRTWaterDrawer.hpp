///////////////////
// Author: Andrew Griffin
// YTE - Graphics - Vulkan - Drawers
///////////////////

#pragma once

#ifndef YTE_Graphics_Vulkan_Drawers_VkRTWaterDrawer_hpp
#define YTE_Graphics_Vulkan_Drawers_VkRTWaterDrawer_hpp

#include "YTE/Graphics/Vulkan/Drawers/VkRenderTarget.hpp"

namespace YTE
{
  class VkRTWaterDrawer : public VkRenderTarget
  {
  public:
    YTEDeclareType(VkRTWaterDrawer);

    VkRTWaterDrawer(VkRenderedSurface *aSurface,
                    vk::Format aColorFormat,
                    vk::Format aDepthFormat,
                    std::shared_ptr<vkhlf::Surface>& aVulkanSurface,
                    ViewData* aView,
                    std::string aName = "",
                    YTEDrawerTypeCombination aCombination = YTEDrawerTypeCombination::DefaultCombination);

    VkRTWaterDrawer(VkRenderedSurface *aSurface,
                    vk::Format aColorFormat,
                    vk::Format aDepthFormat,
                    std::shared_ptr<vkhlf::Surface>& aVulkanSurface,
                    std::string aName = "",
                    YTEDrawerTypeCombination aCombination = YTEDrawerTypeCombination::DefaultCombination);

    virtual ~VkRTWaterDrawer() override;

    virtual void Initialize() override;
    virtual void Resize(vk::Extent2D& aExtent) override;
    virtual void ExecuteCommands(std::shared_ptr<vkhlf::CommandBuffer>& aCBO) override;
    virtual void ExecuteSecondaryEvent(std::shared_ptr<vkhlf::CommandBuffer>& aCBO) override;
    virtual void MoveToNextEvent() override;


    virtual void RenderFull(const vk::Extent2D& aExtent,
                            std::unordered_map<std::string, std::unique_ptr<VkMesh>>& aMeshes) override;
    void RenderRefractive(std::shared_ptr<vkhlf::CommandBuffer>& aCBO,
                          const vk::Extent2D& aExtent,
                          std::unordered_map<std::string, std::unique_ptr<VkMesh>>& aMeshes);
    void RenderReflective(std::shared_ptr<vkhlf::CommandBuffer>& aCBO,
                          const vk::Extent2D& aExtent,
                          std::unordered_map<std::string, std::unique_ptr<VkMesh>>& aMeshes);

    void SetWaterComponent(FFT_WaterSimulation* aComponent)
    {
      mWaterComponent = aComponent;
    }

    FFT_WaterSimulation* GetWaterComponent()
    {
      return mWaterComponent;
    }

  protected:
    struct DrawDataW
    {
      DrawDataW(std::shared_ptr<vkhlf::Pipeline> &aPipeline,
                std::shared_ptr<vkhlf::Buffer> &aVertexBuffer,
                std::shared_ptr<vkhlf::Buffer> &aIndexBuffer,
                std::shared_ptr<vkhlf::PipelineLayout> &aPipelineLayout,
                std::shared_ptr<vkhlf::DescriptorSet> &aDescriptorSet,
                u32 aIndexCount,
                float aLineWidth)
        : mPipeline{ aPipeline }
        , mVertexBuffer{ aVertexBuffer }
        , mIndexBuffer{ aIndexBuffer }
        , mPipelineLayout{ aPipelineLayout }
        , mDescriptorSet{ aDescriptorSet }
        , mIndexCount{ aIndexCount }
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
    std::vector<DrawDataW> mTriangles;
    std::vector<DrawDataW> mLines;
    std::vector<DrawDataW> mCurves;
    std::vector<DrawDataW> mShaderNoCull;
    std::vector<DrawDataW> mAdditiveBlendShader;









    void Render(std::shared_ptr<vkhlf::CommandBuffer>& aCBO, const vk::Extent2D& aExtent, std::unordered_map<std::string, std::unique_ptr<VkMesh>>& aMeshes);
    void NotifyWaterComponent();  // tells water component about new samplers

    void CreateReflectiveRenderPass();
    void CreateReflectiveFrameBuffer();
    void CreateRefractiveRenderPass();
    void CreateRefractiveFrameBuffer();

    FFT_WaterSimulation* mWaterComponent;
    
    // mRenderPass is the refractive
    std::shared_ptr<vkhlf::RenderPass> mReflectiveRenderPass;
    // mData is the refractive
    RenderTargetData mReflectiveData;
    // mCBOB and mCBEB are for refractive
    std::unique_ptr<VkCBOB<3, true>> mReflectiveCBOB;
    std::unique_ptr<VkCBEB<3 >> mReflectiveCBEB;
    vk::Extent2D extent;
  };
}

#endif