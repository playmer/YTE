#pragma once

#include "YTE/Core/Utilities.hpp"

#include "YTE/Graphics/ForwardDeclarations.hpp"
#include "YTE/Graphics/Renderer.hpp"
#include "YTE/Graphics/Vulkan/VkFunctionLoader.hpp"
#include "YTE/Graphics/Vulkan/VkPrimitives.hpp"
#include "YTE/Graphics/Vulkan/VkUtils.hpp"

#include "YTE/StandardLibrary/Delegate.hpp"

#include <set>
#include "vulkan/vulkan.hpp"

namespace YTE
{
  class VkRenderedSurface : public EventHandler
  {
  public:
    ~VkRenderedSurface();

    VkRenderedSurface(Window *aWindow,
      VkRenderer *aRenderer,
      std::shared_ptr<vkhlf::Surface> &aSurface,
      std::shared_ptr<vkhlf::Instance> aInstance);

    void UpdateUniformBuffer();
    void UpdateViewBuffer(UBOView &aView);
    void RenderFrame();
    void Render(LogicUpdate *mEvent);
    void Resize(WindowResize *aEvent);
    void PrintFormats(std::vector<vk::SurfaceFormatKHR> &aFormats);
    void SelectDevice();
    void FindDeviceOfType(vk::PhysicalDeviceType aType);
    void PrintDeviceProperties(std::shared_ptr<vkhlf::PhysicalDevice> &aDevice);
    
    Window *GetWindow()
    {
      return mWindow;
    }

    VkRenderer *GetRenderer()
    {
      return mRenderer;
    }

    void SetWindow(Window *aWindow)
    {
      mWindow = aWindow;
    }

    void SetRenderer(VkRenderer *aRenderer)
    {
      mRenderer = aRenderer;
    }

  private:
    Window *mWindow;
    VkRenderer *mRenderer;

    std::unique_ptr<vkhlf::FramebufferSwapchain> mFramebufferSwapchain;
    std::shared_ptr<vkhlf::Surface> mSurface;
    std::shared_ptr<vkhlf::Queue> mGraphicsQueue;
    std::shared_ptr<vkhlf::RenderPass> mRenderPass;
    std::shared_ptr<vkhlf::Semaphore> mRenderCompleteSemaphore;
    std::shared_ptr<vkhlf::CommandPool> mCommandPool;
    std::shared_ptr<vkhlf::CommandBuffer> mRenderingCommandBuffer;

    std::shared_ptr<vkhlf::PhysicalDevice> mMainDevice;
    std::vector<std::shared_ptr<vkhlf::PhysicalDevice>> mPhysicalDevices;
    std::shared_ptr<vkhlf::Instance> mInstance;
    std::shared_ptr<vkhlf::Device> mDevice;
    vk::Format mColorFormat;
    vk::Format mDepthFormat;

    std::shared_ptr<vkhlf::Buffer> mUBOView;

    QueueFamilyIndices mQueueFamilyIndices;

    glm::vec3 mCameraPosition;
    glm::quat mCameraRotation;

    glm::vec4 mClearColor;

    std::unordered_map<std::string, PipelineData> mPipelines;
  };
}
