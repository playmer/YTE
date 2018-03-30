// Based on the ImGui GLFW binding. The GLFW stuff needs to be rewritten in terms of YTE.

// ImGui GLFW binding with Vulkan + shaders
// FIXME: Changes of ImTextureID aren't supported by this binding! Please, someone add it!

// You can copy and use unmodified imgui_impl_* files in your project. See main.cpp for an example of using this.
// If you use this binding you'll need to call 5 functions: 
//   ImGui_ImplXXXX_Init()
//   ImGui_ImplXXXX_CreateFontsTexture()
//   ImGui_ImplXXXX_NewFrame()
//   ImGui_ImplXXXX_Render() 
//   ImGui_ImplXXXX_Shutdown()
// If you are new to ImGui, see examples/README.txt and documentation at the top of imgui.cpp.
// https://github.com/ocornut/imgui
#pragma once

#ifndef YTE_Graphics_Vulkan_Drawers_VkImgui_hpp
#define YTE_Graphics_Vulkan_Drawers_VkImgui_hpp

#include "imgui.h"

#include "YTE/Graphics/Vulkan/VkFunctionLoader.hpp"

#include "YTE/Graphics/Generics/InstantiatedModel.hpp"
#include "YTE/Graphics/Generics/Mesh.hpp"
#include "YTE/Graphics/GraphicsSystem.hpp"
#include "YTE/Graphics/GraphicsView.hpp"
#include "YTE/Graphics/Vulkan/VkMesh.hpp"
#include "YTE/Graphics/Vulkan/Drawers/VkRenderTarget.hpp"

namespace YTE
{
  class VkImguiDrawer : public VkRenderTarget
  {
  public:
    YTEDeclareType(VkImguiDrawer);

    VkImguiDrawer(VkRenderedSurface *aSurface,
                  vk::Format aColorFormat,
                  vk::Format aDepthFormat,
                  std::shared_ptr<vkhlf::Surface>& aVulkanSurface,
                  ViewData* aView,
                  std::string aName = "",
                  YTEDrawerTypeCombination aCombination = YTEDrawerTypeCombination::DefaultCombination);

    VkImguiDrawer(VkRenderedSurface *aSurface,
                  vk::Format aColorFormat,
                  vk::Format aDepthFormat,
                  std::shared_ptr<vkhlf::Surface>& aVulkanSurface,
                  std::string aName = "",
                  YTEDrawerTypeCombination aCombination = YTEDrawerTypeCombination::DefaultCombination);

    virtual ~VkImguiDrawer() override;

    void GetRenderData();

    virtual void RenderFull(const vk::Extent2D& aExtent,
                            std::unordered_map<std::string, std::unique_ptr<VkMesh>>& aMeshes) override;
    void RenderBegin(std::shared_ptr<vkhlf::CommandBuffer>& aCBO);
    void Render(std::shared_ptr<vkhlf::CommandBuffer>& aCBO,
                const vk::Extent2D& extent,
                std::unordered_map<std::string, std::unique_ptr<VkMesh>>& aMeshes);
    void RenderEnd(std::shared_ptr<vkhlf::CommandBuffer>& aCBO);

    ImGuiContext *mContext{ nullptr };
    Submesh mSubmesh;
    std::unique_ptr<InstantiatedModel> mInstantiatedModel;
    GraphicsView *mView;
    SubMeshPipelineData *mPipelineData;
    VkSubmesh *mVkSubmesh;
    VkShader *mShader;
  };
}

#endif