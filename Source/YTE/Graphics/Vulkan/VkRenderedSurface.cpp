///////////////////
// Author: Andrew Griffin
// YTE - Graphics - Vulkan
///////////////////


#include "YTE/Core/Engine.hpp"

#include "YTE/Graphics/GraphicsSystem.hpp"
#include "YTE/Graphics/UBOs.hpp"
#include "YTE/Graphics/Vulkan/VkInstantiatedModel.hpp"
#include "YTE/Graphics/Vulkan/VkInstantiatedSprite.hpp"
#include "YTE/Graphics/Vulkan/VkInternals.hpp"
#include "YTE/Graphics/Vulkan/VkMesh.hpp"
#include "YTE/Graphics/Vulkan/VkRenderer.hpp"
#include "YTE/Graphics/Vulkan/VkRenderedSurface.hpp"
#include "YTE/Graphics/Vulkan/VkShader.hpp"
#include "YTE/Graphics/Vulkan/VkTexture.hpp"

#include "YTE/Utilities/Utilities.h"

namespace YTE
{
  YTEDefineEvent(GraphicsDataUpdateVk);
  YTEDefineType(GraphicsDataUpdateVk)
  {
    YTERegisterType(GraphicsDataUpdateVk);
  }

  YTEDefineType(VkRenderedSurface)
  {
    YTERegisterType(VkRenderedSurface);
  }



  VkRenderedSurface::VkRenderedSurface(Window *aWindow,
                                       VkRenderer *aRenderer,
                                       std::shared_ptr<vkhlf::Surface> &aSurface)
    : mWindow(aWindow)
    , mRenderer(aRenderer)
    , mSurface(aSurface)
    , mDataUpdateRequired(true)
  {
    auto baseDevice = static_cast<vk::PhysicalDevice>
                                 (*(mRenderer->GetVkInternals()->GetPhysicalDevice().get()));
    vk::SurfaceKHR baseSurfaceKhr = static_cast<vk::SurfaceKHR>(*mSurface);

    auto supportDetails = SwapChainSupportDetails::QuerySwapChainSupport(baseDevice,
                                                                         baseSurfaceKhr);
    auto formats = supportDetails.Formats();

    // If the format list includes just one entry of VK_FORMAT_UNDEFINED,
    // the surface has no preferred format.  Otherwise, at least one
    // supported format will be returned.
    mColorFormat = ((formats.size() == 1) && (formats[0].format == vk::Format::eUndefined)) ?
                                                                 vk::Format::eB8G8R8A8Unorm :
                                                                 formats[0].format;

    PrintSurfaceFormats(formats);
    mDepthFormat = vk::Format::eD24UnormS8Uint;

    // Create a new device with the VK_KHR_SWAPCHAIN_EXTENSION enabled.
    vk::PhysicalDeviceFeatures enabledFeatures;
    enabledFeatures.setTextureCompressionBC(true);

    auto family = mRenderer->GetVkInternals()->GetQueueFamilies().GetGraphicsFamily();
    vkhlf::DeviceQueueCreateInfo deviceCreate{ family,
                                               0.0f };

    mDevice = mRenderer->GetVkInternals()->GetPhysicalDevice()->createDevice(deviceCreate,
                                                               nullptr,
                                                               { VK_KHR_SWAPCHAIN_EXTENSION_NAME },
                                                               enabledFeatures);

    mGraphicsQueue = mDevice->getQueue(family , 0);

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
                                              mRenderer->GetVkInternals()->GetQueueFamilies()
                                                .GetGraphicsFamily());


    mAllocators[AllocatorTypes::Mesh] =
      std::make_unique<vkhlf::DeviceMemoryAllocator>(mDevice, 1024 * 1024, nullptr);

    // 4x 1024 texture size for rgba in this one
    mAllocators[AllocatorTypes::Texture] =
      std::make_unique<vkhlf::DeviceMemoryAllocator>(mDevice, 4096 * 4096, nullptr);  

    mAllocators[AllocatorTypes::UniformBufferObject] =
      std::make_unique<vkhlf::DeviceMemoryAllocator>(mDevice, 1024 * 1024, nullptr);

    auto uboAllocator = mAllocators[AllocatorTypes::UniformBufferObject];
    mViewUBO = mDevice->createBuffer(sizeof(UBOView),
                                     vk::BufferUsageFlagBits::eTransferDst |
                                     vk::BufferUsageFlagBits::eUniformBuffer,
                                     vk::SharingMode::eExclusive,
                                     nullptr,
                                     vk::MemoryPropertyFlagBits::eDeviceLocal,
                                     uboAllocator);

    // TODO: (CBO) Command Buffer is allocated here, this shouldn't be here
    mRenderingCommandBuffer = mCommandPool->allocateCommandBuffer();

    // create Framebuffer & Swapchain
    WindowResize event;
    event.height = mWindow->GetHeight();
    event.width = mWindow->GetWidth();

    ResizeEvent(&event);

    mWindow->YTERegister(Events::WindowResize,
                         this,
                         &VkRenderedSurface::ResizeEvent);
  }



  VkRenderedSurface::~VkRenderedSurface()
  {
    mInstantiatedModels.clear();
    mTextures.clear();
    mMeshes.clear();
    mShaders.clear();
    mFrameBufferSwapChain.reset();
  }



  void VkRenderedSurface::UpdateSurfaceViewBuffer(UBOView &aView)
  {
    mViewUBOData = aView;
    this->YTERegister(Events::GraphicsDataUpdateVk, this,
                      &VkRenderedSurface::GraphicsDataUpdateVkEvent);

  }

  void VkRenderedSurface::PrintSurfaceFormats(std::vector<vk::SurfaceFormatKHR> &aFormats)
  {
    printf("Formats Available: \n");

    for (auto format : aFormats)
    {
      auto colorSpace = vk::to_string(format.colorSpace);
      auto formatString = vk::to_string(format.format);
      printf("  Format/Color Space: %s/%s\n", formatString.c_str(), colorSpace.c_str());
    }
  }

  void VkRenderedSurface::CreateSpritePipeline()
  {
    VkShaderDescriptions descriptions;
    descriptions.AddBinding<SpriteVertex>(vk::VertexInputRate::eVertex);

    //glm::vec3 mPosition;
    descriptions.AddAttribute<glm::vec3>(vk::Format::eR32G32B32Sfloat);

    //glm::vec2 mTextureCoordinates;
    descriptions.AddAttribute<glm::vec2>(vk::Format::eR32G32Sfloat);

    //glm::vec3 mNormal;
    descriptions.AddAttribute<glm::vec3>(vk::Format::eR32G32B32Sfloat);

    descriptions.AddBinding<SpriteInstance>(vk::VertexInputRate::eInstance);

    //u32 mTextureId;
    descriptions.AddAttribute<u32>(vk::Format::eR32Uint);

    //glm::vec4 mMatrix1;
    descriptions.AddAttribute<glm::vec4>(vk::Format::eR32G32B32A32Sfloat);

    //glm::vec4 mMatrix2;
    descriptions.AddAttribute<glm::vec4>(vk::Format::eR32G32B32A32Sfloat);

    //glm::vec4 mMatrix3;
    descriptions.AddAttribute<glm::vec4>(vk::Format::eR32G32B32A32Sfloat);

    //glm::vec4 mMatrix4;
    descriptions.AddAttribute<glm::vec4>(vk::Format::eR32G32B32A32Sfloat);


  }

  // Sprites
  std::unique_ptr<InstantiatedSprite> VkRenderedSurface::CreateSprite(std::string &aTextureFile)
  {
    return nullptr;
  }

  void VkRenderedSurface::DestroySprite(std::unique_ptr<VkInstantiatedSprite> aSprite)
  {

  }
  
  // Models
  std::unique_ptr<VkInstantiatedModel> VkRenderedSurface::CreateModel(std::string &aModelFile)
  {
    mDataUpdateRequired = true;
    auto model = std::make_unique<VkInstantiatedModel>(aModelFile, mRenderer->GetSurface(mWindow));
    mInstantiatedModels[model->mLoadedMesh].push_back(model.get());
    return std::move(model);
  }


  std::unique_ptr<VkInstantiatedModel> VkRenderedSurface::CreateModel(Mesh *aMesh)
  {
    mDataUpdateRequired = true;
    auto model = std::make_unique<VkInstantiatedModel>(aMesh, mRenderer->GetSurface(mWindow));
    mInstantiatedModels[model->mLoadedMesh].push_back(model.get());
    return std::move(model);
  }

  void VkRenderedSurface::DestroyModel(VkInstantiatedModel *aModel)
  {
    if (aModel == nullptr)
    {
      return;
    }

    auto mesh = mInstantiatedModels.find(aModel->mLoadedMesh);

    if (mesh != mInstantiatedModels.end())
    {
      // Remove this instance from the map.
      mesh->second.erase(std::remove(mesh->second.begin(), 
                                     mesh->second.end(), 
                                     aModel),
                         mesh->second.end());
    }
  }

  // Meshes
  VkMesh* VkRenderedSurface::CreateMesh(std::string &aFilename)
  {
    auto meshIt = mMeshes.find(aFilename);

    VkMesh *meshPtr{ nullptr };

    if (meshIt == mMeshes.end())
    {
      // create mesh
      auto mesh = std::make_unique<VkMesh>(mWindow,
                                           mRenderer->GetSurface(mWindow),
                                           aFilename);

      meshPtr = mesh.get();

      mMeshes[aFilename] = std::move(mesh);
      mDataUpdateRequired = true;
    }
    else
    {
      meshPtr = meshIt->second.get();
    }

    return meshPtr;
  }

  
  Mesh* VkRenderedSurface::CreateSimpleMesh(std::string &aName,
                                            std::vector<Submesh> &aSubmeshes)
  {
    auto meshIt = mMeshes.find(aName);

    VkMesh *meshPtr{ nullptr };

    if (meshIt == mMeshes.end())
    {
      // create mesh
      auto mesh = std::make_unique<VkMesh>(mWindow,
                                           mRenderer->GetSurface(mWindow),
                                           aName,
                                           aSubmeshes);

      meshPtr = mesh.get();

      mMeshes[aName] = std::move(mesh);
      mDataUpdateRequired = true;
    }
    else
    {
      meshPtr = meshIt->second.get();
    }

    return meshPtr;
  }

  // Textures
  VkTexture* VkRenderedSurface::CreateTexture(std::string &aFilename, vk::ImageViewType aType)
  {
    auto textureIt = mTextures.find(aFilename);
    VkTexture *texturePtr{ nullptr };

    if (textureIt == mTextures.end())
    {
      auto texture = std::make_unique<VkTexture>(aFilename,
                                                 mRenderer->GetSurface(mWindow),
                                                 aType);

      texturePtr = texture.get();
      mTextures[aFilename] = std::move(texture);
      mDataUpdateRequired = true;
    }
    else
    {
      texturePtr = textureIt->second.get();
    }

    return texturePtr;
  }

  // Shader
  VkShader* VkRenderedSurface::CreateShader(std::string &aShaderSetName,
                                            std::shared_ptr<vkhlf::PipelineLayout> &aPipelineLayout,
                                            VkShaderDescriptions &aDescription)
  {
    auto shaderIt = mShaders.find(aShaderSetName);
    VkShader *shaderPtr{ nullptr };

    if (shaderIt == mShaders.end())
    {
      auto shader = std::make_unique<VkShader>(aShaderSetName,
                                               mRenderer->GetSurface(mWindow),
                                               aPipelineLayout,
                                               aDescription);

      shaderPtr = shader.get();

      mShaders[aShaderSetName] = std::move(shader);
      mDataUpdateRequired = true;
    }
    else
    {
      shaderPtr = shaderIt->second.get();
    }

    return shaderPtr;
  }


  void VkRenderedSurface::ResizeEvent(WindowResize *aEvent)
  {
    YTEUnusedArgument(aEvent);

    auto baseDevice = static_cast<vk::PhysicalDevice>
                                  (*(mRenderer->GetVkInternals()->GetPhysicalDevice().get()));
    vk::SurfaceKHR baseSurfaceKhr = static_cast<vk::SurfaceKHR>(*mSurface);

    auto supportDetails = SwapChainSupportDetails::QuerySwapChainSupport(baseDevice,
                                                                         baseSurfaceKhr);

    auto extent = supportDetails.Capabilities().currentExtent;

    mWindow->SetExtent(extent.height, extent.width);

    DebugObjection((0 > extent.width) || (0 > extent.height),
                   "Resizing to a negative x or y direction is not possible");

    // TODO (Josh): According to vkhlf, you have to do this little dance,
    //              unsure why, should find out.
    mFrameBufferSwapChain.reset();
    mFrameBufferSwapChain = std::make_unique<vkhlf::FramebufferSwapchain>(mDevice,
                                                                          mSurface,
                                                                          mColorFormat,
                                                                          mDepthFormat,
                                                                          mRenderPass);

    DebugObjection(mFrameBufferSwapChain->getExtent() != extent,
                   "Swap chain extent did not update with resize");

    WindowResize event;
    event.height = extent.height;
    event.width = extent.width;
    mWindow->SendEvent(Events::RendererResize, &event);
  }

  void VkRenderedSurface::GraphicsDataUpdateVkEvent(GraphicsDataUpdateVk *aEvent)
  {
    mViewUBO->update<UBOView>(0, mViewUBOData, aEvent->mCBO);
    this->YTEDeregister(Events::GraphicsDataUpdateVk, this,
                        &VkRenderedSurface::GraphicsDataUpdateVkEvent);
  }

  void VkRenderedSurface::FrameUpdate(LogicUpdate *aEvent)
  {
    YTEUnusedArgument(aEvent);
    // Get the index of the next available swapchain image:
    mFrameBufferSwapChain->acquireNextFrame();
    RenderFrameForSurface();
  }

  void VkRenderedSurface::PresentFrame()
  {
    try
    {
      mFrameBufferSwapChain->present(mGraphicsQueue, mRenderCompleteSemaphore);
    }
    catch (...)
    {
      // create Framebuffer & Swapchain
      WindowResize event;
      event.height = mWindow->GetHeight();
      event.width = mWindow->GetWidth();

      ResizeEvent(&event);
    }
  }

  void VkRenderedSurface::GraphicsDataUpdate()
  {
    GraphicsDataUpdateVk update;
    update.mCBO = mCommandPool->allocateCommandBuffer();

    update.mCBO->begin();

    SendEvent(Events::GraphicsDataUpdateVk, &update);

    update.mCBO->end();

    vkhlf::submitAndWait(mGraphicsQueue, update.mCBO);
  }


  void VkRenderedSurface::ReloadAllShaders()
  {
    for (auto &shader : mShaders)
    {
      shader.second->Reload();
    }
  }


  void VkRenderedSurface::RenderFrameForSurface()
  {
    if (mWindow->mKeyboard.IsKeyDown(Keys::F2))
    {
      ReloadAllShaders();
    }

    if (mDataUpdateRequired)
    {
      GraphicsDataUpdate();
    }

    // TODO: (CBO) A cbo is created here, stop this
    mRenderingCommandBuffer = mCommandPool->allocateCommandBuffer();

    std::array<float, 4> colorValues;
    colorValues[0] = mClearColor.r;
    colorValues[1] = mClearColor.g;
    colorValues[2] = mClearColor.b;
    colorValues[3] = mClearColor.a;

    vk::ClearValue color{ colorValues };
    mRenderingCommandBuffer->begin();

    mRenderingCommandBuffer->beginRenderPass(mRenderPass,
                                             mFrameBufferSwapChain->getFramebuffer(),
                                             vk::Rect2D({ 0, 0 },
                                                        mFrameBufferSwapChain->getExtent()),
                                             { color,
                                             vk::ClearValue(vk::ClearDepthStencilValue(1.0f, 0)) },
                                             vk::SubpassContents::eInline);

    auto &extent = mFrameBufferSwapChain->getExtent();

    auto width = static_cast<float>(extent.width);
    auto height = static_cast<float>(extent.height);

    vk::Viewport viewport{ 0.0f, 0.0f, width, height, 0.0f,1.0f };

    mRenderingCommandBuffer->setViewport(0, viewport);
    vk::Rect2D scissor{ { 0, 0 }, extent };
    mRenderingCommandBuffer->setScissor(0, scissor);

    for (auto &shader : mShaders)
    {
      auto &pipeline = shader.second->mShader;

      mRenderingCommandBuffer->bindPipeline(vk::PipelineBindPoint::eGraphics, pipeline);
      for (auto &mesh : mMeshes)
      {
        auto range = mesh.second->mSubmeshes.equal_range(shader.second.get());

        for (auto it = range.first; it != range.second; ++it)
        {
          auto &submesh = it->second;

          mRenderingCommandBuffer->bindVertexBuffer(0,
                                                    submesh->mVertexBuffer,
                                                    0);

          mRenderingCommandBuffer->bindIndexBuffer(submesh->mIndexBuffer,
                                                   0,
                                                   vk::IndexType::eUint32);

          for (auto &model : mInstantiatedModels[mesh.second.get()])
          {
            auto data = model->mPipelineData[submesh.get()];
            mRenderingCommandBuffer->bindDescriptorSets(vk::PipelineBindPoint::eGraphics,
                                                        data.mPipelineLayout,
                                                        0,
                                                        data.mDescriptorSet,
                                                        nullptr);

            mRenderingCommandBuffer->drawIndexed(static_cast<u32>(submesh->mIndexCount),
                                                 1, 
                                                 0, 
                                                 0, 
                                                 0);
          }
        }
      }
    }

    mRenderingCommandBuffer->endRenderPass();
    mRenderingCommandBuffer->end();

    vkhlf::SubmitInfo submit{ { mFrameBufferSwapChain->getPresentSemaphore() },
                              { vk::PipelineStageFlagBits::eColorAttachmentOutput },
                              mRenderingCommandBuffer,
                              mRenderCompleteSemaphore };

    mGraphicsQueue->submit(submit);

    mGraphicsQueue->waitIdle();
  }
}
