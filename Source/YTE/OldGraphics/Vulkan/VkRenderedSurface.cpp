#include <array>
#include <filesystem>

#include "YTE/Core/AssetLoader.hpp"
#include "YTE/Core/Engine.hpp"
#include "YTE/Core/PrivateImplementation.hpp"

#include "YTE/Graphics/Vulkan/VkFunctionLoader.hpp"

#include "YTE/Graphics/GraphicsSystem.hpp"
#include "YTE/Graphics/Model.hpp"
#include "YTE/Graphics/Mesh.hpp"
#include "YTE/Graphics/ShaderDescriptions.hpp"
#include "YTE/Graphics/Texture.hpp"
#include "YTE/Graphics/View.hpp"
#include "YTE/Graphics/Vulkan/VkModel.hpp"
#include "YTE/Graphics/Vulkan/VkRenderer.hpp"
#include "YTE/Graphics/Vulkan/VkRenderedSurface.hpp"

#include "YTE/Utilities/Utilities.h"

namespace YTE
{
  VkRenderedSurface::~VkRenderedSurface()
  {
    //Must destroy the swapchain before destroying the Surface it's associated with.
    mFramebufferSwapchain.reset();
  }

  VkRenderedSurface::VkRenderedSurface(Window *aWindow,
    VkRenderer *aRenderer,
    std::shared_ptr<vkhlf::Surface> &aSurface,
    std::shared_ptr<vkhlf::Instance> aInstance)
    : mWindow(aWindow),
    mRenderer(aRenderer),
    mSurface(aSurface),
    mInstance(aInstance),
    mCameraPosition(-5.0f, 0.0f, 0.0f),
    mCameraRotation(),
    mClearColor(0.42f, 0.63f, 0.98f, 1.0f)
  {
    SelectDevice();

    auto baseDevice = static_cast<vk::PhysicalDevice>(*mMainDevice);
    vk::SurfaceKHR baseSurfaceKhr = static_cast<vk::SurfaceKHR>(*mSurface);

    mQueueFamilyIndices = QueueFamilyIndices::FindQueueFamilies(baseDevice);

    auto supportDetails = SwapChainSupportDetails::QuerySwapChainSupport(baseDevice,
      baseSurfaceKhr);
    auto formats = supportDetails.Formats();

    // If the format list includes just one entry of VK_FORMAT_UNDEFINED,
    // the surface has no preferred format.  Otherwise, at least one
    // supported format will be returned.
    mColorFormat = ((formats.size() == 1) && (formats[0].format == vk::Format::eUndefined)) ?
      vk::Format::eB8G8R8A8Unorm :
      formats[0].format;

    PrintFormats(formats);
    mDepthFormat = vk::Format::eD24UnormS8Uint;

    //// Create a new device with the VK_KHR_SWAPCHAIN_EXTENSION enabled.
    vk::PhysicalDeviceFeatures enabledFeatures;
    enabledFeatures.setTextureCompressionBC(true);

    auto family = mQueueFamilyIndices.GetGraphicsFamily();
    vkhlf::DeviceQueueCreateInfo deviceCreate{ mQueueFamilyIndices.GetGraphicsFamily(),
      0.0f };

    mDevice = mMainDevice->createDevice(deviceCreate,
      nullptr,
      { VK_KHR_SWAPCHAIN_EXTENSION_NAME },
      enabledFeatures);

    mGraphicsQueue = mDevice->getQueue(mQueueFamilyIndices.GetGraphicsFamily(), 0);

    // Attachment Descriptions
    vk::AttachmentDescription colorAttachment{ {},
      mColorFormat,
      vk::SampleCountFlagBits::e1,
      vk::AttachmentLoadOp::eClear,
      vk::AttachmentStoreOp::eStore, // color
      vk::AttachmentLoadOp::eDontCare,
      vk::AttachmentStoreOp::eDontCare, // stencil
      vk::ImageLayout::eUndefined,
      vk::ImageLayout::ePresentSrcKHR };

    vk::AttachmentDescription depthAttachment{ {},
      mDepthFormat,
      vk::SampleCountFlagBits::e1,
      vk::AttachmentLoadOp::eClear,
      vk::AttachmentStoreOp::eStore, // depth
      vk::AttachmentLoadOp::eDontCare,
      vk::AttachmentStoreOp::eDontCare, // stencil
      vk::ImageLayout::eUndefined,
      vk::ImageLayout::eDepthStencilAttachmentOptimal };

    std::array<vk::AttachmentDescription, 2> attachmentDescriptions{ colorAttachment ,
      depthAttachment };

    // Subpass Description
    vk::AttachmentReference colorReference(0, vk::ImageLayout::eColorAttachmentOptimal);
    vk::AttachmentReference depthReference(1, vk::ImageLayout::eDepthStencilAttachmentOptimal);

    vk::SubpassDescription subpass{ {},
      vk::PipelineBindPoint::eGraphics,
      0,
      nullptr,
      1,
      &colorReference,
      nullptr,
      &depthReference,
      0,
      nullptr };

    mRenderPass = mDevice->createRenderPass(attachmentDescriptions, subpass, nullptr);

    mRenderCompleteSemaphore = mDevice->createSemaphore();

    // create a command pool for command buffer allocation
    mCommandPool = mDevice->createCommandPool(vk::CommandPoolCreateFlagBits::eResetCommandBuffer,
      mQueueFamilyIndices.GetGraphicsFamily());

    auto deviceAllocator = aRenderer->GetAllocators()[mDevice.get()];

    deviceAllocator[AllocatorTypes::Mesh] =
      std::make_unique<vkhlf::DeviceMemoryAllocator>(mDevice, 1024 * 1024, nullptr);

    deviceAllocator[AllocatorTypes::Texture] =
      std::make_unique<vkhlf::DeviceMemoryAllocator>(mDevice, 1024 * 1024, nullptr);

    deviceAllocator[AllocatorTypes::UniformBufferObject] =
      std::make_unique<vkhlf::DeviceMemoryAllocator>(mDevice, 1024 * 1024, nullptr);

    auto uboAllocator = deviceAllocator[AllocatorTypes::UniformBufferObject];
    mUBOView = mDevice->createBuffer(sizeof(UBOView),
      vk::BufferUsageFlagBits::eTransferDst |
      vk::BufferUsageFlagBits::eUniformBuffer,
      vk::SharingMode::eExclusive,
      nullptr,
      vk::MemoryPropertyFlagBits::eDeviceLocal,
      uboAllocator);

    UpdateUniformBuffer();

    mRenderingCommandBuffer = mCommandPool->allocateCommandBuffer();

    // create Framebuffer & Swapchain
    WindowResize event;
    event.height = mWindow->GetHeight();
    event.width = mWindow->GetWidth();

    Resize(&event);

    mWindow->YTERegister(Events::WindowResize,
      this,
      &VkRenderedSurface::Resize);
    mWindow->mEngine->YTERegister(Events::FrameUpdate,
      this,
      &VkRenderedSurface::Render);
  }

  void VkRenderedSurface::UpdateUniformBuffer()
  {
    UBOView view;

    auto height = mWindow->GetHeight();
    auto width = mWindow->GetWidth();

    view.mProjectionMatrix = glm::perspective(glm::radians(45.0f),
      static_cast<float>(width) /
      static_cast<float>(height),
      0.1f,
      256.0f);

    view.mViewMatrix = glm::lookAt(mCameraPosition, glm::vec3(), glm::vec3(0.0f, 1.0f, 0.0f));

    auto update = mCommandPool->allocateCommandBuffer();

    update->begin();
    mUBOView->update<UBOView>(0, view, update);
    update->end();

    vkhlf::submitAndWait(mGraphicsQueue, update);
  }

  void VkRenderedSurface::UpdateViewBuffer(UBOView &aView)
  {
    auto update = mCommandPool->allocateCommandBuffer();

    update->begin();
    mUBOView->update<UBOView>(0, aView, update);
    update->end();

    vkhlf::submitAndWait(mGraphicsQueue, update);
  }

  void VkRenderedSurface::RenderFrame()
  {
    // TODO (Josh): Reuse command buffers;
    mRenderingCommandBuffer = mCommandPool->allocateCommandBuffer();

    std::array<float, 4> colorValues;
    colorValues[0] = mClearColor.r;
    colorValues[1] = mClearColor.g;
    colorValues[2] = mClearColor.b;
    colorValues[3] = mClearColor.a;

    vk::ClearValue color{ colorValues };
    mRenderingCommandBuffer->begin();

    mRenderingCommandBuffer->beginRenderPass(mRenderPass,
      mFramebufferSwapchain->getFramebuffer(),
      vk::Rect2D({ 0, 0 },
        mFramebufferSwapchain->getExtent()),
        { color,
      vk::ClearValue(vk::ClearDepthStencilValue(1.0f, 0)) },
      vk::SubpassContents::eInline);

    auto &extent = mFramebufferSwapchain->getExtent();

    auto width = static_cast<float>(extent.width);
    auto height = static_cast<float>(extent.height);

    vk::Viewport viewport{ 0.0f, 0.0f, width, height, 0.0f,1.0f };

    mRenderingCommandBuffer->setViewport(0, viewport);
    vk::Rect2D scissor{ { 0, 0 }, extent };
    mRenderingCommandBuffer->setScissor(0, scissor);

    for (auto &pipelineIt : mPipelines)
    {
      auto &pipeline = pipelineIt.second.mPipeline;
      auto &models = pipelineIt.second.mModels;
      mRenderingCommandBuffer->bindPipeline(vk::PipelineBindPoint::eGraphics,
        pipelineIt.second.mPipeline);
      for (auto &model : models)
      {
        auto &mesh = model.second;

        mRenderingCommandBuffer->bindDescriptorSets(vk::PipelineBindPoint::eGraphics,
          *mesh.mPipelineLayout,
          0,
          *mesh.mDescriptorSet,
          nullptr);
        mRenderingCommandBuffer->bindVertexBuffer(0,
          *mesh.mVertexBuffer,
          0);

        mRenderingCommandBuffer->bindIndexBuffer(*mesh.mIndexBuffer,
          0,
          vk::IndexType::eUint32);

        mRenderingCommandBuffer->drawIndexed(static_cast<u32>(mesh.mIndexCount), 1, 0, 0, 0);
      }
    }

    mRenderingCommandBuffer->endRenderPass();
    mRenderingCommandBuffer->end();

    vkhlf::SubmitInfo submit{ { mFramebufferSwapchain->getPresentSemaphore() },
    { vk::PipelineStageFlagBits::eColorAttachmentOutput },
      mRenderingCommandBuffer,
      mRenderCompleteSemaphore };

    mGraphicsQueue->submit(submit);

    mGraphicsQueue->waitIdle();
  }

  void VkRenderedSurface::Render(LogicUpdate *mEvent)
  {
    // Get the index of the next available swapchain image:
    mFramebufferSwapchain->acquireNextFrame();
    RenderFrame();
    mFramebufferSwapchain->present(mGraphicsQueue, mRenderCompleteSemaphore);\
  }

  void VkRenderedSurface::Resize(WindowResize *aEvent)
  {
    auto baseDevice = static_cast<vk::PhysicalDevice>(*mMainDevice);
    vk::SurfaceKHR baseSurfaceKhr = static_cast<vk::SurfaceKHR>(*mSurface);

    mQueueFamilyIndices = QueueFamilyIndices::FindQueueFamilies(baseDevice);

    auto supportDetails = SwapChainSupportDetails::QuerySwapChainSupport(baseDevice,
      baseSurfaceKhr);

    auto extent = supportDetails.Capabilities().currentExtent;

    mWindow->SetExtent(extent.height, extent.width);

    //TODO (Josh): Change asserts to DebugObjections
    assert((0 <= extent.width) && (0 <= extent.height));

    // TODO (Josh): According to vkhlf, you have to do this little dance,
    //              unsure why, should find out.
    mFramebufferSwapchain.reset();
    mFramebufferSwapchain.reset(new vkhlf::FramebufferSwapchain(mDevice,
      mSurface,
      mColorFormat,
      mDepthFormat,
      mRenderPass));

    assert(mFramebufferSwapchain->getExtent() == extent);

    UpdateUniformBuffer();
    WindowResize event;
    event.height = extent.height;
    event.width = extent.width;
    mWindow->SendEvent(Events::RendererResize, &event);
  }

  void VkRenderedSurface::PrintFormats(std::vector<vk::SurfaceFormatKHR> &aFormats)
  {
    printf("Formats Availible: \n");

    for (auto format : aFormats)
    {
      auto colorSpace = vk::to_string(format.colorSpace);
      auto formatString = vk::to_string(format.format);
      printf("  Format/Color Space: %s/%s\n", formatString.c_str(), colorSpace.c_str());
    }
  }

  void VkRenderedSurface::SelectDevice()
  {
    // Find a physical device with presentation support
    DebugObjection(mInstance->getPhysicalDeviceCount() == 0,
      "We can't find any graphics devices!");

    QueueFamilyIndices::AddRequiredExtension("VK_KHR_swapchain");

    auto count = mInstance->getPhysicalDeviceCount();

    for (size_t i = 0; i < count; ++i)
    {
      auto device = mInstance->getPhysicalDevice(i);
      auto baseDevice = static_cast<vk::PhysicalDevice>(*device);
      vk::SurfaceKHR baseSurfaceKhr = static_cast<vk::SurfaceKHR>(*mSurface);

      auto indices = QueueFamilyIndices::FindQueueFamilies(baseDevice);

      if (indices.IsDeviceSuitable(baseDevice, baseSurfaceKhr))
      {
        mPhysicalDevices.emplace_back(device);
      }

      PrintDeviceProperties(device);
    }

    FindDeviceOfType(vk::PhysicalDeviceType::eDiscreteGpu);

    // If we can't find a Discrete GPU, select one of the others by default.
    if (false == mMainDevice)
    {
      FindDeviceOfType(vk::PhysicalDeviceType::eIntegratedGpu);
    }

    if (false == mMainDevice)
    {
      FindDeviceOfType(vk::PhysicalDeviceType::eVirtualGpu);
    }

    if (false == mMainDevice)
    {
      FindDeviceOfType(vk::PhysicalDeviceType::eCpu);
    }

    if (false == mMainDevice)
    {
      FindDeviceOfType(vk::PhysicalDeviceType::eOther);
    }

    DebugObjection(mMainDevice == false, "We can't find a suitible graphics device!");

    printf("Chosen Device: %s\n", mMainDevice->getProperties().deviceName);
  }

  void VkRenderedSurface::FindDeviceOfType(vk::PhysicalDeviceType aType)
  {
    for (auto &device : mPhysicalDevices)
    {
      if (device->getProperties().deviceType == aType)
      {
        mMainDevice = device;
      }
    }
  }

  void VkRenderedSurface::PrintDeviceProperties(std::shared_ptr<vkhlf::PhysicalDevice> &aDevice)
  {
    auto props = aDevice->getProperties();

    printf("\nDevice:");

    printf("  Device Name: %s\n", props.deviceName);
    printf("  Device ID: %u\n", props.deviceID);
    printf("  Driver Version: %u\n", props.driverVersion);
    printf("  Vendor ID: %u\n", props.vendorID);

    auto type = props.deviceType;

    switch (type)
    {
      case vk::PhysicalDeviceType::eOther:
      {
        printf("  Device Type: Other\n");
        break;
      }

      case vk::PhysicalDeviceType::eIntegratedGpu:
      {
        printf("  Device Type: Integrated GPU\n");
        break;
      }

      case vk::PhysicalDeviceType::eDiscreteGpu:
      {
        printf("  Device Type: Discrete GPU\n");
        break;
      }

      case vk::PhysicalDeviceType::eVirtualGpu:
      {
        printf("  Device Type: Virtual GPU\n");
        break;
      }

      case vk::PhysicalDeviceType::eCpu:
      {
        printf("  Device Type: CPU\n");
        break;
      }
    }

    // TODO (Josh): Print/Store device limits.
    //auto limits = props.limits;

    printf("  API Version: %u\n", props.apiVersion);
  }
}