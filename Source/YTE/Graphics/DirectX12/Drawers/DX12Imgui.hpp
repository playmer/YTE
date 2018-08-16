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

//#include "YTE/Graphics/DirectX12/DX12VkFunctionLoader.hpp"

#include "YTE/Graphics/Generics/InstantiatedModel.hpp"
#include "YTE/Graphics/Generics/Mesh.hpp"
#include "YTE/Graphics/GraphicsSystem.hpp"
#include "YTE/Graphics/GraphicsView.hpp"
#include "YTE/Graphics/DirectX12/DX12Mesh.hpp"
#include "YTE/Graphics/DirectX12/Drawers/DX12RenderTarget.hpp"
#include "YTE/Graphics/ImguiLayer.hpp"

namespace YTE
{
  class Dx12ImguiDrawer : public VkRenderTarget
  {
  public:
    YTEDeclareType(Dx12ImguiDrawer);

    Dx12ImguiDrawer(Dx12RenderedSurface *aSurface,
                  vk::Format aColorFormat,
                  vk::Format aDepthFormat,
                  std::shared_ptr<vkhlf::Surface>& aVulkanSurface,
                  ViewData* aView,
                  DrawerTypeCombination aCombination = DrawerTypeCombination::DefaultCombination);

    void Initialize();

    virtual ~Dx12ImguiDrawer() override;

    void PreFrameUpdate(LogicUpdate *aUpdate);

    void RenderFull(std::unordered_map<std::string, std::unique_ptr<VkMesh>> &aMeshes) override;
    void RenderBegin(std::shared_ptr<vkhlf::CommandBuffer> &aCBO);
    void Render(std::shared_ptr<vkhlf::CommandBuffer> &aCBO);
    void RenderEnd(std::shared_ptr<vkhlf::CommandBuffer> &aCBO);

    ImguiLayer *mContext{ nullptr };
    GraphicsView *mView;
    SubMeshPipelineData *mPipelineData;
    Dx12Submesh *mDx12Submesh;
    Dx12Shader *mShader;
    std::string mModelName;
    std::string mTextureName;
  };
}

#endif