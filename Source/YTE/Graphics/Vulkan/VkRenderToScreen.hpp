///////////////////
// Author: Andrew Griffin
// YTE - Graphics - Vulkan
///////////////////

#pragma once

#ifndef YTE_Graphics_Vulkan_VkRenderToScreen_hpp
#define YTE_Graphics_Vulkan_VkRenderToScreen_hpp

#include "YTE/Core/EventHandler.hpp"

#include "YTE/Graphics/GraphicsView.hpp"
#include "YTE/Graphics/Vulkan/VkFunctionLoader.hpp"
#include "YTE/Graphics/Vulkan/ForwardDeclarations.hpp"
#include "YTE/Graphics/Vulkan/VkShaderDescriptions.hpp"
#include "YTE/Graphics/Vulkan/VkCommandBufferBuffer.hpp"
#include "YTE/Graphics/Vulkan/VkCommandBufferEventBuffer.hpp"
#include "YTE/Graphics/Vulkan/Drawers/VkRenderTarget.hpp"

namespace YTE
{
  class VkRenderToScreen : public EventHandler
  {
  public:
    YTEDeclareType(VkRenderToScreen);

    // forward declare quad
  private:
    class ScreenQuad;
    class ScreenShader;

  public:
    VkRenderToScreen(Window *aWindow,
                     VkRenderer *aRenderer,
                     VkRenderedSurface *aSurface,
                     vk::Format aColorFormat,
                     vk::Format aDepthFormat,
                     std::shared_ptr<vkhlf::Surface> &aVulkanSurface,
                     std::string aShaderSetName);

    ~VkRenderToScreen();

    void ReloadShaders(bool aFromSet = false);
    void Resize(vk::Extent2D &aExtent);
    void ResetRenderTargets(std::vector<VkRenderTarget*> &aRTs);
    void SetRenderTargets(std::vector<VkRenderTarget*> &aRTs);

    void FrameUpdate();
    const vk::Extent2D &GetExtent();

    bool PresentFrame(std::shared_ptr<vkhlf::Queue> &aGraphicsQueue,
                      std::shared_ptr<vkhlf::Semaphore> &aRenderCompleteSemaphore);

    void RenderFull(const vk::Extent2D &aExtent);
    void Render(std::shared_ptr<vkhlf::CommandBuffer> &aCBO);

    void LoadToVulkan(VkGraphicsDataUpdate *aEvent);

    void MoveToNextEvent();
    void ExecuteSecondaryEvent(std::shared_ptr<vkhlf::CommandBuffer> &aCBO);
    void ExecuteCommands(std::shared_ptr<vkhlf::CommandBuffer> &aCBO);

    // gettors / settors
    std::shared_ptr<vkhlf::RenderPass> &GetRenderPass()
    {
      return mRenderPass;
    }

    const std::shared_ptr<vkhlf::Semaphore> &GetPresentSemaphore()
    {
      return mFrameBufferSwapChain->getPresentSemaphore();
    }

    const std::shared_ptr<vkhlf::Framebuffer> &GetFrameBuffer()
    {
      return mFrameBufferSwapChain->getFramebuffer();
    }


  private:
    void CreateSwapChain(vk::Extent2D &aExtent);
    void CreateRenderPass();
    void ReloadQuad();
    void LoadQuad();
    void LoadShaders();

    VkRenderedSurface *mSurface;
    VkRenderer *mRenderer;
    Window *mWindow;
    std::unique_ptr<vkhlf::FramebufferSwapchain> mFrameBufferSwapChain;
    std::shared_ptr<vkhlf::Surface> mVulkanSurface;
    std::shared_ptr<vkhlf::RenderPass> mRenderPass;
    vk::Format mColorFormat;
    vk::Format mDepthFormat;
    bool mSignedUpForUpdate;
    std::unique_ptr<ScreenQuad> mScreenQuad;
    std::unique_ptr<ScreenShader> mScreenShader;
    std::string mShaderSetName;
    std::unique_ptr<VkCBOB<3, true>> mCBOB;
    std::unique_ptr<VkCBEB<3>> mCBEB;
    std::vector<VkRenderTarget::RenderTargetData*> mRenderTargetData;
    bool mIsResize = false;

    friend class ScreenQuad;
    friend class ScreenShader;




    // //////////////////////////////////////////////////
    // Screen Quad
    // //////////////////////////////////////////////////
    class ScreenQuad
    {
    public:
      struct ShaderData
      {
        VkShaderDescriptions mDescriptions;
        std::shared_ptr<vkhlf::PipelineLayout> mPipelineLayout;
        std::string mDefines;
      };


      ScreenQuad(VkRenderToScreen* aParent);
      ~ScreenQuad();

      void LoadToVulkan(VkGraphicsDataUpdate *aEvent);

      void Resize();

      void Bind(std::shared_ptr<vkhlf::CommandBuffer> aCBO);
      void Render(std::shared_ptr<vkhlf::CommandBuffer> aCBO);

      ShaderData& GetShaderData()
      {
        return mShaderData;
      }

      std::vector<std::pair<std::string, DrawerTypeCombination>>& GetSamplerData()
      {
        return mSamplers;
      }

    private:
      void Create();
      void Destroy();

      VkRenderToScreen *mParent;
      ShaderData mShaderData;

      std::shared_ptr<vkhlf::DescriptorSet> mDescriptorSet;
      std::vector<Vertex> mVertices;
      std::vector<u32> mIndices;
      std::shared_ptr<vkhlf::Buffer> mVertexBuffer;
      std::shared_ptr<vkhlf::Buffer> mIndexBuffer;
      std::shared_ptr<vkhlf::DescriptorSetLayout> mDescriptorSetLayout;
      u32 mIndexCount;
      std::vector<std::pair<std::string, DrawerTypeCombination>> mSamplers;
    };

    ////////////////////////////////////////////////////
    // Screen Quad Shader
    ////////////////////////////////////////////////////
    class ScreenShader
    {
    public:
      ScreenShader(VkRenderToScreen* aParent, ScreenQuad *aSibling, std::string& aShaderSetName, bool aReload);
      ~ScreenShader();

      void LoadToVulkan(VkGraphicsDataUpdate *aEvent);

      void Bind(std::shared_ptr<vkhlf::CommandBuffer> aCBO);

    private:
      void Create(std::string& aShaderSetName, bool aReload);
      void Destroy();
      std::string GenerateFragmentShader();

      VkRenderToScreen *mParent;
      VkRenderToScreen::ScreenQuad *mSibling;

      std::shared_ptr<vkhlf::Pipeline> mShader;
    };
  };
}


#endif