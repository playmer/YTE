
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
#include "YTE/Graphics/Vulkan/VkRenderer.hpp"

#include "YTE/Utilities/Utilities.h"


namespace fs = std::experimental::filesystem;

namespace YTE
{
  struct MeshToRender
  {
    MeshToRender(std::shared_ptr<vkhlf::PipelineLayout> const *aPipelineLayout,
                 std::shared_ptr<vkhlf::DescriptorSet> const *aDescriptorSet,
                 std::shared_ptr<vkhlf::Buffer> const *aVertexBuffer,
                 std::shared_ptr<vkhlf::Buffer> const *aIndexBuffer,
                 u64 aIndexCount)
      : mPipelineLayout(aPipelineLayout),
        mDescriptorSet(aDescriptorSet),
        mVertexBuffer(aVertexBuffer),
        mIndexBuffer(aIndexBuffer),
        mIndexCount(aIndexCount)
    {

    }

    MeshToRender(const MeshToRender &aRight)
      : mPipelineLayout(aRight.mPipelineLayout),
        mDescriptorSet(aRight.mDescriptorSet),
        mVertexBuffer(aRight.mVertexBuffer),
        mIndexBuffer(aRight.mIndexBuffer),
        mIndexCount(aRight.mIndexCount)
    {

    }

    MeshToRender& operator=(const MeshToRender &aRight)
    {
      mPipelineLayout = aRight.mPipelineLayout;
      mDescriptorSet = aRight.mDescriptorSet;
      mVertexBuffer = aRight.mVertexBuffer;
      mIndexBuffer = aRight.mIndexBuffer;
      mIndexCount = aRight.mIndexCount;

      return *this;
    }

    std::shared_ptr<vkhlf::PipelineLayout> const *mPipelineLayout;
    std::shared_ptr<vkhlf::DescriptorSet> const *mDescriptorSet;
    std::shared_ptr<vkhlf::Buffer> const *mVertexBuffer;
    std::shared_ptr<vkhlf::Buffer> const *mIndexBuffer;
    u64 mIndexCount;
  };

  struct PipelineData
  {
    PipelineData(std::shared_ptr<vkhlf::Pipeline> aPipeline)
      : mPipeline(aPipeline)
    {

    }

    std::shared_ptr<vkhlf::Pipeline> mPipeline;

    OrderedMap<u64, MeshToRender> mModels;
  };

  namespace AllocatorTypes
  {
    extern std::string Mesh{ "Mesh" };
    extern std::string Texture{ "Texture" };
    extern std::string UniformBufferObject{ "UniformBufferObject" };
  }

  static VKAPI_ATTR
  VkBool32 VKAPI_CALL debugReportCallback(VkDebugReportFlagsEXT flags,
                                          VkDebugReportObjectTypeEXT objectType,
                                          uint64_t object,
                                          size_t location,
                                          int32_t messageCode,
                                          const char* pLayerPrefix,
                                          const char* pMessage,
                                          void* pUserData)
  {
    switch (flags)
    {
      case VK_DEBUG_REPORT_INFORMATION_BIT_EXT:
      {
        printf("INFORMATION: %s\n", pMessage);
        return VK_FALSE;
      }
      case VK_DEBUG_REPORT_WARNING_BIT_EXT:
      {
        printf("WARNING: ");
        break;
      }
      case VK_DEBUG_REPORT_PERFORMANCE_WARNING_BIT_EXT:
      {
        printf("PERFORMANCE WARNING: ");
        break;
      }
      case VK_DEBUG_REPORT_ERROR_BIT_EXT:
      {
        printf("ERROR: ");
        break;
      }
      case VK_DEBUG_REPORT_DEBUG_BIT_EXT:
      {
        printf("DEBUG: %s\n", pMessage);
        return VK_FALSE;
      }
      default:
      {
        printf("Unknown Flag (%u): ", flags);
        break;
      }
    }
    printf("%s\n", pMessage);
    assert(!pMessage);
    return VK_TRUE;
  }

  struct RenderedSurface : public EventHandler
  {
    ~RenderedSurface()
    {
      //Must destroy the swapchain before destroying the Surface it's associated with.
      mFramebufferSwapchain.reset();
    }

    RenderedSurface(Window *aWindow,
                    VkRenderer *aRenderer,
                    std::shared_ptr<vkhlf::Surface> &aSurface,
                    std::shared_ptr<vkhlf::Instance> aInstance)
      : mWindow(aWindow),
        mRenderer(aRenderer),
        mSurface(aSurface),
        mInstance(aInstance),
        mCameraPosition(-5.0f, 0.0f, 0.0f),
        mCameraRotation()
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
      vk::AttachmentDescription colorAttachment{{},
                                                mColorFormat,
                                                vk::SampleCountFlagBits::e1,
                                                vk::AttachmentLoadOp::eClear,
                                                vk::AttachmentStoreOp::eStore, // color
                                                vk::AttachmentLoadOp::eDontCare,
                                                vk::AttachmentStoreOp::eDontCare, // stencil
                                                vk::ImageLayout::eUndefined,
                                                vk::ImageLayout::ePresentSrcKHR };

      vk::AttachmentDescription depthAttachment{{}, 
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

      vk::SubpassDescription subpass{{},
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
                       &RenderedSurface::Resize);
      mWindow->mEngine->YTERegister(Events::FrameUpdate,
                                this,
                                &RenderedSurface::Render);
    }

    void UpdateUniformBuffer()
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


    void UpdateViewBuffer(UBOView &aView)
    {
      auto update = mCommandPool->allocateCommandBuffer();

      update->begin();
      mUBOView->update<UBOView>(0, aView, update);
      update->end();

      vkhlf::submitAndWait(mGraphicsQueue, update);
    }

    void RenderFrame()
    {
      // TODO (Josh): Reuse command buffers;
      mRenderingCommandBuffer = mCommandPool->allocateCommandBuffer();

      std::array<float, 4> ccv = { 0.42f, 0.63f, 0.98f };
      mRenderingCommandBuffer->begin();

      mRenderingCommandBuffer->beginRenderPass(mRenderPass,
                                               mFramebufferSwapchain->getFramebuffer(),
                                               vk::Rect2D({ 0, 0 },
                                                 mFramebufferSwapchain->getExtent()),
                                                 { vk::ClearValue(ccv),
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

    void Render(LogicUpdate *mEvent)
    {
      // Get the index of the next available swapchain image:
      mFramebufferSwapchain->acquireNextFrame();
      RenderFrame();
      mFramebufferSwapchain->present(mGraphicsQueue, mRenderCompleteSemaphore);
    }

    void Resize(WindowResize *aEvent)
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

    void PrintFormats(std::vector<vk::SurfaceFormatKHR> &aFormats)
    {
      printf("Formats Availible: \n");

      for (auto format : aFormats)
      {
        auto colorSpace = vk::to_string(format.colorSpace);
        auto formatString = vk::to_string(format.format);
        printf("  Format/Color Space: %s/%s\n", formatString.c_str(), colorSpace.c_str());
      }
    }

    void SelectDevice()
    {
      // Find a physical device with presentation support
      DebugObjection(mInstance->getPhysicalDeviceCount() == 0,
        "We can't find any graphics devices!");

      QueueFamilyIndices::AddRequiredExtension("VK_KHR_swapchain");

      for (size_t i = 0; i < mInstance->getPhysicalDeviceCount(); ++i)
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

    void FindDeviceOfType(vk::PhysicalDeviceType aType)
    {
      for (auto &device : mPhysicalDevices)
      {
        if (device->getProperties().deviceType == aType)
        {
          mMainDevice = device;
        }
      }
    }

    void PrintDeviceProperties(std::shared_ptr<vkhlf::PhysicalDevice> &aDevice)
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



    std::unordered_map<std::string, PipelineData> mPipelines;
  };


  void InstantiatedMeshRendererData::UpdateUniformBuffer(InstantiatedMesh & aModel)
  {
    UBOModel model;

    model.mModelMatrix = glm::translate(model.mModelMatrix, aModel.mPosition);
    model.mModelMatrix = model.mModelMatrix * glm::toMat4(aModel.mRotation);
    model.mModelMatrix = glm::scale(model.mModelMatrix, aModel.mScale);

    auto update = mSurface->mCommandPool->allocateCommandBuffer();

    update->begin();
    mUBOModel->update<UBOModel>(0, model, update);
    update->end();

    vkhlf::submitAndWait(mSurface->mGraphicsQueue, update);
  }

  VkRenderer::~VkRenderer()
  {
    vkelUninit();
  }

  VkRenderer::VkRenderer(Engine *aEngine)
    : mEngine(aEngine)
  {
    std::vector<std::string>  enabledExtensions, enabledLayers;

    enabledExtensions.emplace_back("VK_KHR_surface");
      
    #if defined(Windows)
      enabledExtensions.emplace_back("VK_KHR_win32_surface");
    #endif

    #if !defined(NDEBUG)
      enabledExtensions.emplace_back("VK_EXT_debug_report");

      // Enable standard validation layer to find as much errors as possible!
      enabledLayers.push_back("VK_LAYER_LUNARG_standard_validation");
    #endif

    // Create a new vulkan instance using the required extensions
    mInstance = vkhlf::Instance::create("Yours Truly Engine", 
                                        1, 
                                        enabledLayers, 
                                        enabledExtensions);

    #if !defined(NDEBUG)
      vk::DebugReportFlagsEXT flags(//vk::DebugReportFlagBitsEXT::eInformation        |
                                    vk::DebugReportFlagBitsEXT::eWarning            |
                                    vk::DebugReportFlagBitsEXT::ePerformanceWarning |
                                    vk::DebugReportFlagBitsEXT::eError              |
                                    vk::DebugReportFlagBitsEXT::eDebug);
      mDebugReportCallback 
        = mInstance->createDebugReportCallback(flags,
                                                &debugReportCallback);
    #endif

    vkelInstanceInit(static_cast<vk::Instance>(*mInstance));

    auto &windows = aEngine->GetWindows();

    for (auto &window : windows)
    {
      if (window.second->mShouldBeRenderedTo)
      {
        auto surface = window.second->SetUpVulkanWindow(static_cast<void*>(mInstance.get()));
        mSurfaces.emplace(window.second.get(), 
                          std::make_unique<RenderedSurface>(window.second.get(),
                                                            this,
                                                            surface,
                                                            mInstance));
      }
    }
  }

  Texture* VkRenderer::AddTexture(Window *aWindow,
                                  const char *aTextureFile)
  {
    auto surfaceIt = mSurfaces.find(aWindow);

    if (surfaceIt == mSurfaces.end())
    {
      DebugObjection(true, "We can't find a surface corresponding to the provided window.");
      return nullptr;
    }

    auto surface = surfaceIt->second.get();

    return AddTexture(surface, aTextureFile);
  };

  void VkRenderer::UpdateModelTransformation(Model *aModel)
  {
    auto data = aModel->GetInstantiatedMesh()->mData.Get<InstantiatedMeshRendererData>();
    data->UpdateUniformBuffer(*aModel->GetInstantiatedMesh());
  }
    
  Texture* VkRenderer::AddTexture(RenderedSurface *aSurface,
                                  const char *aTexture)
  {
    auto device = aSurface->mDevice;

    fs::path file{ aTexture };

    std::string textureName{ file.stem().string() };

    //TODO (Josh): Make Crunch work.
    //file = L"Crunch" / file.filename().concat(L".crn");
    file = L"Originals" / file.filename();
    std::string fileStr{ file.string() };

    auto &textureMap = mTextures[device.get()];

    auto it = textureMap.find(textureName);

    if (it != textureMap.end())
    {
      return it->second.get();
    }

    auto textureIt = textureMap.emplace(textureName, std::make_unique<Texture>(fileStr));

    auto texture = textureIt.first->second.get();

    auto rendererData = texture->mRendererData.ConstructAndGet<TextureRendererData>();

    auto allocator = mAllocators[device.get()][AllocatorTypes::Texture];

    auto update = aSurface->mCommandPool->allocateCommandBuffer();

    // 1. init image
    vk::Format format;

    switch (texture->mType)
    {
      case TextureType::DXT1_sRGB:
      {
        format = vk::Format::eBc1RgbaSrgbBlock;
        break;
      }
      case TextureType::DXT5_sRGB:
      {
        format = vk::Format::eBc3SrgbBlock;
        break;
      }
      case TextureType::RGBA:
      {
        format = vk::Format::eR8G8B8A8Unorm;
        break;
      }
    }

    vk::FormatProperties imageFormatProperties = aSurface->mMainDevice->getFormatProperties(format);

    assert((imageFormatProperties.linearTilingFeatures & 
            vk::FormatFeatureFlagBits::eSampledImage) || 
            (imageFormatProperties.optimalTilingFeatures & 
            vk::FormatFeatureFlagBits::eSampledImage));

    vk::Extent3D imageExtent{ texture->mWidth, texture->mHeight, 1 };

    rendererData->mImage = device->createImage({},
                                               vk::ImageType::e2D, 
                                               format, 
                                               imageExtent,
                                               1,
                                               1, 
                                               vk::SampleCountFlagBits::e1, 
                                               vk::ImageTiling::eOptimal,
                                               vk::ImageUsageFlagBits::eTransferDst | 
                                               vk::ImageUsageFlagBits::eSampled, 
                                               vk::SharingMode::eExclusive, 
                                               {}, 
                                               vk::ImageLayout::eUndefined,
                                               vk::MemoryPropertyFlagBits::eDeviceLocal, 
                                               allocator);

    update->begin();
    {
      // create a temporary upload image and fill it with pixel data. 
      // The destructor of MappedImage will put the transfer into the command buffer.
      vkhlf::MappedImage mi(rendererData->mImage, update, 0, texture->mData.size());
      vk::SubresourceLayout layout = mi.getSubresourceLayout(vk::ImageAspectFlagBits::eColor, 0, 0);
      uint8_t * data = reinterpret_cast<uint8_t*>(mi.getPointer());

      auto height = texture->mHeight;
      auto width = texture->mWidth;

      auto pixels = texture->mData.data();

      for (size_t y = 0; y < height; y++)
      {
        uint8_t * rowPtr = data;
        for (size_t x = 0; x < width; x++, rowPtr += 4, pixels += 4)
        {
          rowPtr[0] = pixels[0];
          rowPtr[1] = pixels[1];
          rowPtr[2] = pixels[2];
          rowPtr[3] = pixels[3];
        }
        data += layout.rowPitch;
      }
    }
    update->end();
    vkhlf::submitAndWait(aSurface->mGraphicsQueue, update);

    rendererData->mView = rendererData->mImage->createImageView(vk::ImageViewType::e2D, format);

    // 2. init sampler
    rendererData->mSampler = device->createSampler(vk::Filter::eNearest,
                                                   vk::Filter::eNearest, 
                                                   vk::SamplerMipmapMode::eNearest, 
                                                   vk::SamplerAddressMode::eClampToEdge, 
                                                   vk::SamplerAddressMode::eClampToEdge,
                                                   vk::SamplerAddressMode::eClampToEdge, 
                                                   0.0f, 
                                                   false, 
                                                   1.0f, 
                                                   false, 
                                                   vk::CompareOp::eNever, 
                                                   0.0f, 
                                                   0.0f, 
                                                   vk::BorderColor::eFloatOpaqueWhite, 
                                                   false);


    return texture;
  }



  void VkRenderer::AddDescriptorSet(RenderedSurface *aSurface,
                                    InstantiatedMesh *aModel,
                                    Mesh::SubMesh *aSubMesh,
                                    InstantiatedMeshRendererData::SubmeshPipelineData *aSubmeshPipelineData)
  {
    auto device = aSurface->mDevice;

    // init descriptor and pipeline layouts
    std::vector<vkhlf::DescriptorSetLayoutBinding> dslbs;
    dslbs.emplace_back(0,
                       vk::DescriptorType::eUniformBuffer,
                       vk::ShaderStageFlagBits::eVertex,
                       nullptr);
    dslbs.emplace_back(1,
                       vk::DescriptorType::eUniformBuffer,
                       vk::ShaderStageFlagBits::eVertex,
                       nullptr);
    dslbs.emplace_back(2,
                       vk::DescriptorType::eUniformBuffer,
                       vk::ShaderStageFlagBits::eFragment,
                       nullptr);

    u32 samplers = 0;

    if (nullptr != aSubMesh->mDiffuseMap) ++samplers;
    if (nullptr != aSubMesh->mSpecularMap) ++samplers;
    if (nullptr != aSubMesh->mNormalMap) ++samplers;
    
    std::shared_ptr<vkhlf::DescriptorPool> descriptorPool;

    if (0 != samplers)
    {
      std::vector<vk::DescriptorPoolSize> descriptorTypes;
      descriptorTypes.emplace_back(vk::DescriptorType::eUniformBuffer, 1);
      descriptorTypes.emplace_back(vk::DescriptorType::eUniformBuffer, 1);
      descriptorTypes.emplace_back(vk::DescriptorType::eUniformBuffer, 1);

      for (u32 i = 0; i < samplers; ++i)
      {
        dslbs.emplace_back(i + 3,
                           vk::DescriptorType::eCombinedImageSampler,
                           vk::ShaderStageFlagBits::eFragment,
                           nullptr);

        descriptorTypes.emplace_back(vk::DescriptorType::eCombinedImageSampler, 1);
      }

      descriptorPool = device->createDescriptorPool({}, 1, descriptorTypes);
    }
    else
    {
      descriptorPool = device->createDescriptorPool({},
                                                    1,
                                                    { { vk::DescriptorType::eUniformBuffer, 3 },
                                                    });
    }

    auto modelData = aModel->mData.Get<InstantiatedMeshRendererData>();
    auto subMeshData = aSubMesh->mRendererData.Get<MeshRendererData>();

    auto descriptorSetLayout = device->createDescriptorSetLayout(dslbs);

    aSubmeshPipelineData->mPipelineLayout = device->createPipelineLayout(descriptorSetLayout, nullptr);

    // init descriptor set
    aSubmeshPipelineData->mDescriptorSet = device->allocateDescriptorSet(descriptorPool, descriptorSetLayout);
    std::vector<vkhlf::WriteDescriptorSet> wdss;
    wdss.reserve(6);

    // Helper constants and variables.
    constexpr auto unibuf = vk::DescriptorType::eUniformBuffer;
    auto &ds = aSubmeshPipelineData->mDescriptorSet;
    u32 binding = 0;

    // Add Uniform Buffers
    vkhlf::DescriptorBufferInfo uboView{aSurface->mUBOView, 0, sizeof(UBOView)};
    vkhlf::DescriptorBufferInfo uboModel{ modelData->mUBOModel, 0, sizeof(UBOModel) };
    vkhlf::DescriptorBufferInfo uboMaterial{ subMeshData->mUBOMaterial, 0, sizeof(UBOMaterial) };
    
    wdss.emplace_back(ds, binding++, 0, 1, unibuf, nullptr, uboView);
    wdss.emplace_back(ds, binding++, 0, 1, unibuf, nullptr, uboModel);
    wdss.emplace_back(ds, binding++, 0, 1, unibuf, nullptr, uboMaterial);

    // Add Texture Samplers
    auto addTS = [&wdss, &binding, &ds](Texture *aData, vkhlf::DescriptorImageInfo &imageInfo)
    {
      constexpr auto imgsam = vk::DescriptorType::eCombinedImageSampler;

      if (nullptr == aData)
      {
        return;
      }

      auto t = aData->mRendererData.Get<TextureRendererData>();

      imageInfo.sampler = t->mSampler;
      imageInfo.imageView = t->mView;
      wdss.emplace_back(ds, binding++, 0, 1, imgsam, imageInfo, nullptr);
    };

    vkhlf::DescriptorImageInfo dTexInfo{ nullptr, nullptr, vk::ImageLayout::eGeneral };
    vkhlf::DescriptorImageInfo sTexInfo{ nullptr, nullptr, vk::ImageLayout::eGeneral };
    vkhlf::DescriptorImageInfo nTexInfo{ nullptr, nullptr, vk::ImageLayout::eGeneral };

    addTS(aSubMesh->mDiffuseMap, dTexInfo);
    addTS(aSubMesh->mSpecularMap, sTexInfo);
    addTS(aSubMesh->mNormalMap, nTexInfo);
      
    device->updateDescriptorSets(wdss, nullptr);
  }

  std::shared_ptr<vkhlf::Pipeline> VkRenderer::AddPipeline(RenderedSurface *aSurface,
                                                           InstantiatedMesh *aModel,
                                                           Mesh::SubMesh *aSubMesh, 
                                                           InstantiatedMeshRendererData::SubmeshPipelineData *aSubmeshPipelineData)
  {
    auto device = aSurface->mDevice;

    auto vertex = aSubMesh->mShaderSetName + ".vert";
    auto fragment = aSubMesh->mShaderSetName + ".frag";

    auto vertexFile = Path::GetShaderPath(Path::GetEnginePath(), vertex.c_str());
    auto fragmentFile = Path::GetShaderPath(Path::GetEnginePath(), fragment.c_str());

    std::string vertexText;
    ReadFileToString(vertexFile, vertexText);

    std::string fragmetText;
    ReadFileToString(fragmentFile, fragmetText);

    auto vertexData = vkhlf::compileGLSLToSPIRV(vk::ShaderStageFlagBits::eVertex, 
                                                vertexText);
    auto fragmentData = vkhlf::compileGLSLToSPIRV(vk::ShaderStageFlagBits::eFragment, 
                                                  fragmetText);

    auto vertexModule = device->createShaderModule(vertexData);
    auto fragmentModule = device->createShaderModule(fragmentData);


    // TODO (Josh): We should be reflecting these.
    ShaderDescriptions descriptions;
    descriptions.AddBinding<Vertex>(vk::VertexInputRate::eVertex);

    //glm::vec3 mPosition;
    descriptions.AddAttribute<glm::vec3>(vk::Format::eR32G32B32Sfloat);

    //glm::vec3 mTextureCoordinates;
    descriptions.AddAttribute<glm::vec3>(vk::Format::eR32G32B32Sfloat);

    //glm::vec3 mNormal;
    descriptions.AddAttribute<glm::vec3>(vk::Format::eR32G32B32Sfloat);

    //glm::vec3 mColor;
    descriptions.AddAttribute<glm::vec3>(vk::Format::eR32G32B32Sfloat);

    //glm::vec3 mTangent;
    descriptions.AddAttribute<glm::vec3>(vk::Format::eR32G32B32Sfloat);

    //glm::vec3 mBinormal;
    descriptions.AddAttribute<glm::vec3>(vk::Format::eR32G32B32Sfloat);

    //glm::vec3 mBitangent;
    descriptions.AddAttribute<glm::vec3>(vk::Format::eR32G32B32Sfloat);

      // Initialize Pipeline
    std::shared_ptr<vkhlf::PipelineCache> pipelineCache = device->createPipelineCache(0, nullptr);

    vkhlf::PipelineShaderStageCreateInfo vertexStage(vk::ShaderStageFlagBits::eVertex, 
                                                     vertexModule, 
                                                     "main");
    vkhlf::PipelineShaderStageCreateInfo fragmentStage(vk::ShaderStageFlagBits::eFragment, 
                                                       fragmentModule, 
                                                       "main");

    vkhlf::PipelineVertexInputStateCreateInfo vertexInput(descriptions.Bindings(), 
                                                          descriptions.Attributes());
    vk::PipelineInputAssemblyStateCreateInfo assembly({}, 
                                                      vk::PrimitiveTopology::eTriangleList, 
                                                      VK_FALSE);

      // One dummy viewport and scissor, as dynamic state sets them.
    vkhlf::PipelineViewportStateCreateInfo viewport({ {} }, { {} });
    vk::PipelineRasterizationStateCreateInfo rasterization({}, 
                                                            false, 
                                                            false, 
                                                            vk::PolygonMode::eFill, 
                                                            //vk::CullModeFlagBits::eNone, 
                                                            vk::CullModeFlagBits::eBack, 
                                                            vk::FrontFace::eCounterClockwise, 
                                                            false, 
                                                            0.0f, 
                                                            0.0f, 
                                                            0.0f, 
                                                            1.0f);

    vkhlf::PipelineMultisampleStateCreateInfo multisample(vk::SampleCountFlagBits::e1, 
                                                          false, 
                                                          0.0f, 
                                                          nullptr, 
                                                          false, 
                                                          false);

    vk::StencilOpState stencilOpState(vk::StencilOp::eKeep, 
                                      vk::StencilOp::eKeep, 
                                      vk::StencilOp::eKeep, 
                                      vk::CompareOp::eAlways, 
                                      0, 
                                      0, 
                                      0);
    vk::PipelineDepthStencilStateCreateInfo depthStencil({}, 
                                                         true, 
                                                         true, 
                                                         vk::CompareOp::eLessOrEqual, 
                                                         false, 
                                                         false, 
                                                         stencilOpState, 
                                                         stencilOpState, 
                                                         0.0f, 
                                                         0.0f);

    vk::PipelineColorBlendAttachmentState colorBlendAttachment(false,
                                                               vk::BlendFactor::eDstAlpha, 
                                                               vk::BlendFactor::eOne, 
                                                               vk::BlendOp::eAdd, 
                                                               vk::BlendFactor::eZero, 
                                                               vk::BlendFactor::eOneMinusSrcAlpha, 
                                                               vk::BlendOp::eAdd,
                                                               vk::ColorComponentFlagBits::eR | 
                                                               vk::ColorComponentFlagBits::eG | 
                                                               vk::ColorComponentFlagBits::eB | 
                                                               vk::ColorComponentFlagBits::eA);

    vkhlf::PipelineColorBlendStateCreateInfo colorBlend(false, 
                                                        vk::LogicOp::eNoOp, 
                                                        colorBlendAttachment, 
                                                        { 1.0f, 1.0f, 1.0f, 1.0f });

    vkhlf::PipelineDynamicStateCreateInfo dynamic({ vk::DynamicState::eViewport, 
                                                    vk::DynamicState::eScissor });

    auto pipeline = device->createGraphicsPipeline(pipelineCache, 
                                                    {}, 
                                                    { vertexStage, fragmentStage }, 
                                                    vertexInput, 
                                                    assembly, 
                                                    nullptr, 
                                                    viewport, 
                                                    rasterization, 
                                                    multisample, 
                                                    depthStencil, 
                                                    colorBlend, 
                                                    dynamic, 
                                                    aSubmeshPipelineData->mPipelineLayout,
                                                    aSurface->mRenderPass);

    return pipeline;
  }

  Mesh* VkRenderer::AddMesh(RenderedSurface *aSurface, 
                            std::string &aFilename)
  {
    auto update = aSurface->mCommandPool->allocateCommandBuffer();
    auto device = aSurface->mDevice;

    auto &meshMap = mMeshes[device.get()];

    auto it = meshMap.find(aFilename);

    if (it != meshMap.end())
    {
      return it->second.get();
    }

    auto meshIt = meshMap.emplace(aFilename,
                                  std::make_unique<Mesh>(aSurface->mRenderer, 
                                                         aSurface->mWindow, 
                                                         aFilename));

    auto mesh = meshIt.first->second.get();

    auto allocator = mAllocators[device.get()][AllocatorTypes::Mesh];

    for (auto &subMesh : mesh->mParts)
    {
      auto subMeshData = subMesh.mRendererData.ConstructAndGet<MeshRendererData>();
      
      subMeshData->mVertexBuffer =
        aSurface->mDevice->createBuffer(subMesh.mVertexBufferSize,
                                        vk::BufferUsageFlagBits::eTransferDst |
                                        vk::BufferUsageFlagBits::eVertexBuffer,
                                        vk::SharingMode::eExclusive,
                                        nullptr,
                                        vk::MemoryPropertyFlagBits::eDeviceLocal,
                                        allocator);

      subMeshData->mIndexBuffer =
        aSurface->mDevice->createBuffer(subMesh.mIndexBufferSize,
                                        vk::BufferUsageFlagBits::eTransferDst |
                                        vk::BufferUsageFlagBits::eIndexBuffer,
                                        vk::SharingMode::eExclusive,
                                        nullptr,
                                        vk::MemoryPropertyFlagBits::eDeviceLocal,
                                        allocator);
      
      subMeshData->mUBOMaterial = 
        aSurface->mDevice->createBuffer(sizeof(UBOMaterial),
                                        vk::BufferUsageFlagBits::eTransferDst |
                                        vk::BufferUsageFlagBits::eUniformBuffer,
                                        vk::SharingMode::eExclusive,
                                        nullptr,
                                        vk::MemoryPropertyFlagBits::eDeviceLocal,
                                        allocator);

      auto update = aSurface->mCommandPool->allocateCommandBuffer();

      update->begin();
      subMeshData->mVertexBuffer->update<Vertex>(0, subMesh.mVertexBuffer, update);
      subMeshData->mIndexBuffer->update<u32>(0, subMesh.mIndexBuffer, update);
      subMeshData->mUBOMaterial->update<UBOMaterial>(0, subMesh.mUBOMaterial, update);
      update->end();

      vkhlf::submitAndWait(aSurface->mGraphicsQueue, update);
    }

    return mesh;
  }

  std::unique_ptr<InstantiatedMesh> VkRenderer::AddModel(Window *aWindow,
                                                         std::string &aMeshFile)
  {
    auto surfaceIt = mSurfaces.find(aWindow);

    if (surfaceIt == mSurfaces.end())
    {
      DebugObjection(true, "We can't find a surface corresponding to the provided window.");
      return nullptr;
    }

    auto surface = surfaceIt->second.get();

    auto allocator = mAllocators[surface->mDevice.get()][AllocatorTypes::UniformBufferObject];
    auto iMesh = std::make_unique<InstantiatedMesh>();

    iMesh->mMesh = AddMesh(surface, aMeshFile);

    auto iMeshData = iMesh->mData.ConstructAndGet<InstantiatedMeshRendererData>(this, iMesh->mMesh, aWindow);

    iMeshData->mSurface = surface;
    iMeshData->mUBOModel = surface->mDevice->createBuffer(sizeof(UBOModel),
                                                          vk::BufferUsageFlagBits::eTransferDst |
                                                          vk::BufferUsageFlagBits::eUniformBuffer,
                                                          vk::SharingMode::eExclusive,
                                                          nullptr,
                                                          vk::MemoryPropertyFlagBits::eDeviceLocal,
                                                          allocator);

    iMeshData->UpdateUniformBuffer(*iMesh);

    auto &pipelines = surface->mPipelines;

    auto numberOfSubmeshes = iMesh->mMesh->mParts.size();

    iMeshData->mSubmeshPipelineData.reserve(numberOfSubmeshes);

    for (size_t i = 0; i < numberOfSubmeshes; ++i)
    {
      auto &subMesh = iMesh->mMesh->mParts[i];

      auto subMeshData = subMesh.mRendererData.Get<MeshRendererData>();

      iMeshData->mSubmeshPipelineData.push_back(InstantiatedMeshRendererData::SubmeshPipelineData{});

      auto &submeshPipelineData = iMeshData->mSubmeshPipelineData[i];

      submeshPipelineData.mId = mMeshIdCounter++;

      AddDescriptorSet(surface,
                       iMesh.get(),
                       &subMesh,
                       &submeshPipelineData);

      auto it = pipelines.find(subMesh.mShaderSetName);

      if (it == pipelines.end())
      {
        auto pipeline = AddPipeline(surface,
                                    iMesh.get(),
                                    &subMesh,
                                    &submeshPipelineData);

        PipelineData pipelineData{ pipeline };

        it = pipelines.emplace(subMesh.mShaderSetName, pipelineData).first;
      }

      MeshToRender toRender{ &submeshPipelineData.mPipelineLayout,
                             &submeshPipelineData.mDescriptorSet,
                             &subMeshData->mVertexBuffer,
                             &subMeshData->mIndexBuffer,
                             subMesh.mIndexBuffer.size()};


      auto &models = it->second.mModels;

      models.Emplace(submeshPipelineData.mId, toRender);
    }

    return std::move(iMesh);
  }

  void VkRenderer::RemoveMeshId(Window *aWindow, u64 aId)
  {
    auto surfaceIt = mSurfaces.find(aWindow);

    if (surfaceIt == mSurfaces.end())
    {
      DebugObjection(true, "We can't find a surface corresponding to the provided window.");
      return;
    }

    for (auto &pipelineIt : surfaceIt->second->mPipelines)
    {
      auto &models = pipelineIt.second.mModels;

      auto it = models.Find(aId);

      if (it != models.end())
      {
        models.Erase(it);
      }
    }
  }

  void VkRenderer::UpdateViewBuffer(Window *aWindow, UBOView &aView)
  {
    auto surfaceIt = mSurfaces.find(aWindow);

    if (surfaceIt == mSurfaces.end())
    {
      DebugObjection(true, "We can't find a surface corresponding to the provided window.");
      return;
    }

    surfaceIt->second->UpdateViewBuffer(aView);
  }
}
