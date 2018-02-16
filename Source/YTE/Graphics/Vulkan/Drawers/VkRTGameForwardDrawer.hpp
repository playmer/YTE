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
                          std::string aName = "",
                          YTEDrawerTypeCombination aCombination = YTEDrawerTypeCombination::DefaultCombination);

    VkRTGameForwardDrawer(VkRenderedSurface *aSurface,
                          vk::Format aColorFormat,
                          vk::Format aDepthFormat,
                          std::shared_ptr<vkhlf::Surface>& aVulkanSurface,
                          std::string aName = "",
                          YTEDrawerTypeCombination aCombination = YTEDrawerTypeCombination::DefaultCombination);

    virtual ~VkRTGameForwardDrawer() override;

    virtual void RenderFull(const vk::Extent2D& aExtent,
                            std::unordered_map<std::string, std::unique_ptr<VkMesh>>& aMeshes) override;
    void RenderBegin(std::shared_ptr<vkhlf::CommandBuffer>& aCBO);
    void Render(std::shared_ptr<vkhlf::CommandBuffer>& aCBO,
                        const vk::Extent2D& extent,
                        std::unordered_map<std::string, std::unique_ptr<VkMesh>>& aMeshes);
    void RenderEnd(std::shared_ptr<vkhlf::CommandBuffer>& aCBO);
  };
}

#endif