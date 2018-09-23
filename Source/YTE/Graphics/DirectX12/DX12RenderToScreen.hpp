#pragma once

#ifndef YTE_Graphics_Vulkan_VkRenderToScreen_hpp
#define YTE_Graphics_Vulkan_VkRenderToScreen_hpp

#include "YTE/Core/EventHandler.hpp"

#include "YTE/Graphics/GraphicsView.hpp"
#include "YTE/Graphics/DirectX12/DX12FunctionLoader.hpp"
#include "YTE/Graphics/DirectX12/ForwardDeclarations.hpp"
#include "YTE/Graphics/DirectX12/DX12ShaderDescriptions.hpp"
#include "YTE/Graphics/DirectX12/DX12CommandBufferBuffer.hpp"
#include "YTE/Graphics/DirectX12/DX12CommandBufferEventBuffer.hpp"
#include "YTE/Graphics/DirectX12/Drawers/DX12RenderTarget.hpp"

namespace YTE
{
  class DX12RenderToScreen : public EventHandler
  {
  public:
    YTEDeclareType(DX12RenderToScreen);

    // forward declare quad
  private:
    class ScreenQuad;
    class ScreenShader;

  public:
    DX12RenderToScreen(Window *aWindow,
                       Dx12Renderer *aRenderer,
                       Dx12RenderedSurface *aSurface,
                       //vk::Format aColorFormat,
                       //vk::Format aDepthFormat,
                       //std::shared_ptr<vkhlf::Surface> &aVulkanSurface,
                       std::string aShaderSetName);

    ~DX12RenderToScreen();

    void ReloadShaders(bool aFromSet = false);
    void Resize(/*vk::Extent2D &aExtent*/);
    void ResetRenderTargets(std::vector<DX12RenderTarget*> &aRTs);
    void SetRenderTargets(std::vector<DX12RenderTarget*> &aRTs);

    void FrameUpdate();
    //const vk::Extent2D &GetExtent();

    bool PresentFrame(/*std::shared_ptr<vkhlf::Queue> &aGraphicsQueue,
                      std::shared_ptr<vkhlf::Semaphore> &aRenderCompleteSemaphore*/);

    void RenderFull(/*const vk::Extent2D &aExtent*/);
    void Render(/*std::shared_ptr<vkhlf::CommandBuffer> &aCBO*/);

    void LoadToVulkan(DX12GraphicsDataUpdate *aEvent);

    void MoveToNextEvent();
    void ExecuteSecondaryEvent(/*std::shared_ptr<vkhlf::CommandBuffer> &aCBO*/);
    void ExecuteCommands(/*std::shared_ptr<vkhlf::CommandBuffer> &aCBO*/);

    //std::shared_ptr<vkhlf::RenderPass> &GetRenderPass()
    //{
    //  return mRenderPass;
    //}
    //
    //const std::shared_ptr<vkhlf::Semaphore> &GetPresentSemaphore()
    //{
    //  return mFrameBufferSwapChain->getPresentSemaphore();
    //}
    //
    //const std::shared_ptr<vkhlf::Framebuffer> &GetFrameBuffer()
    //{
    //  return mFrameBufferSwapChain->getFramebuffer();
    //}


  private:
    void CreateSwapChain(/*vk::Extent2D &aExtent*/);
    void CreateRenderPass();
    void ReloadQuad();
    void LoadQuad();
    void LoadShaders();

    Dx12RenderedSurface *mSurface;
    Dx12Renderer *mRenderer;
    Window *mWindow;
    //std::unique_ptr<vkhlf::FramebufferSwapchain> mFrameBufferSwapChain;
    //std::shared_ptr<vkhlf::Surface> mVulkanSurface;
    //std::shared_ptr<vkhlf::RenderPass> mRenderPass;
    //vk::Format mColorFormat;
    //vk::Format mDepthFormat;
    bool mSignedUpForUpdate;
    std::unique_ptr<ScreenQuad> mScreenQuad;
    std::unique_ptr<ScreenShader> mScreenShader;
    std::string mShaderSetName;
    std::unique_ptr<Dx12CBOB<3, true>> mCBOB;
    std::unique_ptr<DX12CBEB<3>> mCBEB;
    std::vector<DX12RenderTarget::DX12RenderTargetData*> mRenderTargetData;
    bool mIsResize = false;

    friend class ScreenQuad;
    friend class ScreenShader;

    ////////////////////////////////////////////////////
    // Screen Quad
    ////////////////////////////////////////////////////
    class ScreenQuad
    {
    public:
      struct ShaderData
      {
        Dx12ShaderDescriptions mDescriptions;
        //std::shared_ptr<vkhlf::PipelineLayout> mPipelineLayout;
        std::string mDefines;
      };


      ScreenQuad(DX12RenderToScreen* aParent);
      ~ScreenQuad();

      void LoadToVulkan(DX12GraphicsDataUpdate *aEvent);

      void Resize();

      void Bind(/*std::shared_ptr<vkhlf::CommandBuffer> aCBO*/);
      void Render(/*std::shared_ptr<vkhlf::CommandBuffer> aCBO*/);

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

      DX12RenderToScreen *mParent;
      ShaderData mShaderData;

      std::vector<Vertex> mVertices;
      std::vector<u32> mIndices;
      //std::shared_ptr<vkhlf::DescriptorSet> mDescriptorSet;
      //std::shared_ptr<vkhlf::Buffer> mVertexBuffer;
      //std::shared_ptr<vkhlf::Buffer> mIndexBuffer;
      //std::shared_ptr<vkhlf::DescriptorSetLayout> mDescriptorSetLayout;
      u32 mIndexCount;
      std::vector<std::pair<std::string, DrawerTypeCombination>> mSamplers;
    };





    ////////////////////////////////////////////////////
    // Screen Quad Shader
    ////////////////////////////////////////////////////
    class ScreenShader
    {
    public:
      ScreenShader(DX12RenderToScreen* aParent, ScreenQuad *aSibling, std::string& aShaderSetName, bool aReload);
      ~ScreenShader();

      void LoadToVulkan(DX12GraphicsDataUpdate *aEvent);

      void Bind(/*std::shared_ptr<vkhlf::CommandBuffer> aCBO*/);

    private:
      void Create(std::string& aShaderSetName, bool aReload);
      void Destroy();
      std::string GenerateFragmentShader();

      DX12RenderToScreen *mParent;
      DX12RenderToScreen::ScreenQuad *mSibling;

      //std::shared_ptr<vkhlf::Pipeline> mShader;
    };
  };
}


#endif