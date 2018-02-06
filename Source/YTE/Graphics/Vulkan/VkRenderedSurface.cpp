///////////////////
// Author: Andrew Griffin
// YTE - Graphics - Vulkan
///////////////////


#include "YTE/Core/Engine.hpp"

#include "YTE/Graphics/GraphicsSystem.hpp"
#include "YTE/Graphics/UBOs.hpp"
#include "YTE/Graphics/Vulkan/VkInstantiatedLight.hpp"
#include "YTE/Graphics/Vulkan/VkInstantiatedModel.hpp"
#include "YTE/Graphics/Vulkan/VkInternals.hpp"
#include "YTE/Graphics/Vulkan/VkLightManager.hpp"
#include "YTE/Graphics/Vulkan/VkMesh.hpp"
#include "YTE/Graphics/Vulkan/VkRenderer.hpp"
#include "YTE/Graphics/Vulkan/VkRenderedSurface.hpp"
#include "YTE/Graphics/Vulkan/VkShader.hpp"
#include "YTE/Graphics/Vulkan/VkTexture.hpp"

#include "YTE/StandardLibrary/Range.hpp"

#include "YTE/Utilities/Utilities.hpp"

namespace YTE
{
  YTEDefineEvent(AnimationUpdateVk);
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
    auto internals = mRenderer->GetVkInternals();

    auto baseDevice = static_cast<vk::PhysicalDevice>(*(internals->GetPhysicalDevice().get()));
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

    auto family = internals->GetQueueFamilies().GetGraphicsFamily();
    vkhlf::DeviceQueueCreateInfo deviceCreate{family,
                                              0.0f};

    mDevice = internals->GetPhysicalDevice()->createDevice(deviceCreate,
                                                           nullptr,
                                                           { VK_KHR_SWAPCHAIN_EXTENSION_NAME },
                                                           enabledFeatures);

    mGraphicsQueue = mDevice->getQueue(family , 0);

    // Attachment Descriptions
    vk::AttachmentDescription colorAttachment{{},
                                              mColorFormat,
                                              vk::SampleCountFlagBits::e1,
                                              //vk::AttachmentLoadOp::eLoad,
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

    std::array<vk::AttachmentDescription, 2> attachmentDescriptions{colorAttachment,
                                                                    depthAttachment};

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

    std::array<vk::SubpassDependency, 2> subpassDependencies;

    // Transition from final to initial (VK_SUBPASS_EXTERNAL refers to all commmands executed outside of the actual renderpass)
    subpassDependencies[0].srcSubpass = VK_SUBPASS_EXTERNAL;
    subpassDependencies[0].dstSubpass = 0;
    subpassDependencies[0].srcStageMask = vk::PipelineStageFlagBits::eBottomOfPipe;
    subpassDependencies[0].dstStageMask = vk::PipelineStageFlagBits::eColorAttachmentOutput;
    subpassDependencies[0].srcAccessMask = vk::AccessFlagBits::eMemoryRead;
    subpassDependencies[0].dstAccessMask = vk::AccessFlagBits::eColorAttachmentRead |
                                           vk::AccessFlagBits::eColorAttachmentWrite;
    subpassDependencies[0].dependencyFlags = vk::DependencyFlagBits::eByRegion;

    // Transition from initial to final
    subpassDependencies[1].srcSubpass = 0;
    subpassDependencies[1].dstSubpass = VK_SUBPASS_EXTERNAL;
    subpassDependencies[1].srcStageMask = vk::PipelineStageFlagBits::eColorAttachmentOutput;
    subpassDependencies[1].dstStageMask = vk::PipelineStageFlagBits::eBottomOfPipe;
    subpassDependencies[1].srcAccessMask = vk::AccessFlagBits::eColorAttachmentRead |
                                           vk::AccessFlagBits::eColorAttachmentWrite;
    subpassDependencies[1].dstAccessMask = vk::AccessFlagBits::eMemoryRead;
    subpassDependencies[1].dependencyFlags = vk::DependencyFlagBits::eByRegion;

    mRenderPass = mDevice->createRenderPass(attachmentDescriptions, subpass, subpassDependencies);

    mRenderCompleteSemaphore = mDevice->createSemaphore();

    // create a command pool for command buffer allocation
    mCommandPool = mDevice->createCommandPool(vk::CommandPoolCreateFlagBits::eResetCommandBuffer,
                                              family);


    mAllocators[AllocatorTypes::Mesh] =
      std::make_unique<vkhlf::DeviceMemoryAllocator>(mDevice, 1024 * 1024, nullptr);

    // 4x 1024 texture size for rgba in this one
    mAllocators[AllocatorTypes::Texture] =
      std::make_unique<vkhlf::DeviceMemoryAllocator>(mDevice, 4096 * 4096, nullptr);  

    mAllocators[AllocatorTypes::UniformBufferObject] =
      std::make_unique<vkhlf::DeviceMemoryAllocator>(mDevice, 1024 * 1024, nullptr);

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
    mViewData.clear();
    mTextures.clear();
    mMeshes.clear();
    mShaders.clear();
    mFrameBufferSwapChain.reset();
  }
  
  void VkRenderedSurface::UpdateSurfaceViewBuffer(GraphicsView *aView, UBOView &aUBOView)
  {
    GetViewData(aView).mViewUBOData = aUBOView;
    ////GetViewData(aView).mViewUBOData.mProjectionMatrix[0][0] *= -1;   // flips vulkan x axis right, since it defaults down
    //GetViewData(aView).mViewUBOData.mProjectionMatrix[1][1] *= -1;   // flips vulkan y axis up, since it defaults down
    this->YTERegister(Events::GraphicsDataUpdateVk,
                      this,
                      &VkRenderedSurface::GraphicsDataUpdateVkEvent);

  }



  void VkRenderedSurface::UpdateSurfaceIlluminationBuffer(GraphicsView *aView, UBOIllumination& aIllumination)
  {
    GetViewData(aView).mIlluminationUBOData = aIllumination;
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
  
  // Models
  std::unique_ptr<VkInstantiatedModel> VkRenderedSurface::CreateModel(GraphicsView *aView, std::string &aModelFile)
  {
    mDataUpdateRequired = true;
    auto model = std::make_unique<VkInstantiatedModel>(aModelFile, this, aView);
    auto &instantiatedModels = GetViewData(aView).mInstantiatedModels;
    instantiatedModels[static_cast<VkMesh*>(model->GetMesh())].push_back(model.get());
    return std::move(model);
  }


  std::unique_ptr<VkInstantiatedModel> VkRenderedSurface::CreateModel(GraphicsView *aView, Mesh *aMesh)
  {
    mDataUpdateRequired = true;
    auto model = std::make_unique<VkInstantiatedModel>(aMesh, this, aView);
    auto &instantiatedModels = GetViewData(aView).mInstantiatedModels;
    instantiatedModels[static_cast<VkMesh*>(model->GetMesh())].push_back(model.get());
    return std::move(model);
  }

  void VkRenderedSurface::DestroyModel(GraphicsView *aView, VkInstantiatedModel *aModel)
  {
    if (aModel == nullptr)
    {
      return;
    }

    auto &instantiatedModels = GetViewData(aView).mInstantiatedModels;

    auto mesh = instantiatedModels.find(static_cast<VkMesh*>(aModel->GetMesh()));

    if (mesh != instantiatedModels.end())
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
                                            std::vector<Submesh> &aSubmeshes,
		                                        bool aForceUpdate)
  {
    auto meshIt = mMeshes.find(aName);

    VkMesh *meshPtr{ nullptr };

    if (aForceUpdate || meshIt == mMeshes.end())
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
                                            VkShaderDescriptions &aDescription,
                                            bool aCullBackFaces)
  {
    auto shaderIt = mShaders.find(aShaderSetName);
    VkShader *shaderPtr{ nullptr };

    if (shaderIt == mShaders.end())
    {
      auto shader = std::make_unique<VkShader>(aShaderSetName,
                                               mRenderer->GetSurface(mWindow),
                                               aPipelineLayout,
                                               aDescription,
                                               aCullBackFaces);

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



  std::unique_ptr<VkInstantiatedLight> VkRenderedSurface::CreateLight(GraphicsView* aView)
  {
    mDataUpdateRequired = true;
    auto light = GetViewData(aView).mLightManager.CreateLight();
    return std::move(light);
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

  void VkRenderedSurface::RegisterView(GraphicsView *aView)
  {
    auto key = std::make_pair(aView->GetOrder(), aView);
    auto it = mViewData.find(key);

    if (it == mViewData.end())
    {
      auto emplaced = mViewData.emplace(key, ViewData());

      auto uboAllocator = mAllocators[AllocatorTypes::UniformBufferObject];
      auto buffer = mDevice->createBuffer(sizeof(UBOView),
                                          vk::BufferUsageFlagBits::eTransferDst |
                                          vk::BufferUsageFlagBits::eUniformBuffer,
                                          vk::SharingMode::eExclusive,
                                          nullptr,
                                          vk::MemoryPropertyFlagBits::eDeviceLocal,
                                          uboAllocator);
      auto buffer2 = mDevice->createBuffer(sizeof(UBOIllumination),
                                           vk::BufferUsageFlagBits::eTransferDst |
                                           vk::BufferUsageFlagBits::eUniformBuffer,
                                           vk::SharingMode::eExclusive,
                                           nullptr,
                                           vk::MemoryPropertyFlagBits::eDeviceLocal,
                                           uboAllocator);

      auto &view = emplaced.first->second;
      view.mViewUBO = buffer;
      view.mIlluminationUBO = buffer2;
      view.mLightManager.SetSurfaceAndView(this, aView);
    }
  }

  void VkRenderedSurface::DeregisterView(GraphicsView *aView)
  {
    auto key = std::make_pair(aView->GetOrder(), aView);
    auto it = mViewData.find(key);

    if (it != mViewData.end())
    {
      mViewData.erase(it);
    }
  }

  void VkRenderedSurface::ViewOrderChanged(GraphicsView *aView, float aOldOrder, float aNewOrder)
  {
    auto it = mViewData.find(std::make_pair(aOldOrder, aView));

    if (it != mViewData.end())
    {
      mViewData.emplace(std::make_pair(aNewOrder, aView), std::move(it->second));
      mViewData.erase(it);
    }
    else
    {
      mViewData.emplace(std::make_pair(aNewOrder, aView), ViewData());
    }
  }

  void VkRenderedSurface::GraphicsDataUpdateVkEvent(GraphicsDataUpdateVk *aEvent)
  {
    for (auto &viewDataIt : mViewData)
    {
      auto &viewData = viewDataIt.second;
      viewData.mViewUBO->update<UBOView>(0, viewData.mViewUBOData, aEvent->mCBO);
      viewData.mIlluminationUBO->update<UBOIllumination>(0, viewData.mIlluminationUBOData, aEvent->mCBO);
      this->YTEDeregister(Events::GraphicsDataUpdateVk, 
                          this,
                          &VkRenderedSurface::GraphicsDataUpdateVkEvent);
    }
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
    //try
    //{
    //  mFrameBufferSwapChain->present(mGraphicsQueue, mRenderCompleteSemaphore);
    //}
    //catch (...)
    //{
    //  // create Framebuffer & Swapchain
    //  WindowResize event;
    //  event.height = mWindow->GetHeight();
    //  event.width = mWindow->GetWidth();
    //  ResizeEvent(&event);
    //}
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


  void VkRenderedSurface::AnimationUpdate()
  {
    GraphicsDataUpdateVk update;
    update.mCBO = mCommandPool->allocateCommandBuffer();
    update.mCBO->begin();
    SendEvent(Events::AnimationUpdateVk, &update);
    update.mCBO->end();
    vkhlf::submitAndWait(mGraphicsQueue, update.mCBO);
  }



  void VkRenderedSurface::SetLights(bool aOnOrOff)
  {
    for (auto& view : mViewData)
    {
      view.second.mLightManager.SetLights(aOnOrOff);
    }
  }



  void VkRenderedSurface::RenderFrameForSurface()
  {
    if (mWindow->mKeyboard.IsKeyDown(Keys::Control) && mWindow->mKeyboard.IsKeyDown(Keys::R))
    {
      ReloadAllShaders();
    }

    if (mDataUpdateRequired)
    {
      GraphicsDataUpdate();
    }

    
    // TODO: (CBO) A cbo is created here, stop this
    auto buffer = mCommandPool->allocateCommandBuffer();

    bool first = true;
    buffer->begin();

    std::array<float, 4> colorValues;

    if (0 < mViewData.size())
    {
      auto &view = mViewData.begin()->second;
      colorValues[0] = view.mClearColor.r;
      colorValues[1] = view.mClearColor.g;
      colorValues[2] = view.mClearColor.b;
      colorValues[3] = view.mClearColor.a;
    }
    else
    {
      colorValues[0] = 0.44f;
      colorValues[1] = 0.44f;
      colorValues[2] = 0.44f;
      colorValues[3] = 0.44f;
    }

    vk::ClearValue color{colorValues};
    vk::ClearDepthStencilValue depthStensil{1.0f, 0};

    auto &extent = mFrameBufferSwapChain->getExtent();

    buffer->beginRenderPass(mRenderPass,
                            mFrameBufferSwapChain->getFramebuffer(),
                            vk::Rect2D({ 0, 0 }, extent),
                            {color, depthStensil},
                            vk::SubpassContents::eInline);

    auto width = static_cast<float>(extent.width);
    auto height = static_cast<float>(extent.height);

    for (auto [viewData, i] : enumerate(mViewData))
    {
      vk::Viewport viewport{ 0.0f, 0.0f, width, height, 0.0f,1.0f };
      buffer->setViewport(0, viewport);

      vk::Rect2D scissor{ { 0, 0 }, extent };
      buffer->setScissor(0, scissor);

      auto &instantiatedModels = viewData->second.mInstantiatedModels;

      for (auto &shader : mShaders)
      {
        auto &pipeline = shader.second->mShader;

        buffer->bindPipeline(vk::PipelineBindPoint::eGraphics, pipeline);
        for (auto &mesh : mMeshes)
        {
          auto &models = instantiatedModels[mesh.second.get()];

          // We can early out on this mesh if there are no models that use it.
          if (models.empty())
          {
            continue;
          }

          // We get the sub meshes that use the current shader, then draw them.
          auto range = mesh.second->mSubmeshes.equal_range(shader.second.get());

          if (mesh.second->GetInstanced())
          {
            buffer->bindVertexBuffer(1,
                                     mesh.second->mInstanceManager.InstanceBuffer(),
                                     0);
          }

          for (auto it = range.first; it != range.second; ++it)
          {
            auto &submesh = it->second;

            buffer->bindVertexBuffer(0,
                                     submesh->mVertexBuffer,
                                     0);

            buffer->bindIndexBuffer(submesh->mIndexBuffer,
                                    0,
                                    vk::IndexType::eUint32);



            if (mesh.second->GetInstanced())
            {
              auto data = submesh->mPipelineData;
              buffer->bindDescriptorSets(vk::PipelineBindPoint::eGraphics,
                                         data.mPipelineLayout,
                                         0,
                                         data.mDescriptorSet,
                                         nullptr);

              buffer->drawIndexed(static_cast<u32>(submesh->mIndexCount),
                                  1, 
                                  0, 
                                  0, 
                                  0);
            }
            else
            {
              for (auto &model : models)
              {
                if (model->mUseAlphaBlending || model->mUseAdditiveBlending)
                {
                  continue;
                }
                auto &data = model->mPipelineData[submesh.get()];
                buffer->bindDescriptorSets(vk::PipelineBindPoint::eGraphics,
                                           data.mPipelineLayout,
                                           0,
                                           data.mDescriptorSet,
                                           nullptr);

                buffer->drawIndexed(static_cast<u32>(submesh->mIndexCount),
                                    1, 
                                    0, 
                                    0, 
                                    0);
              }
            }
          }
        }
      }

      // pass for additive and alpha
      for (auto &shader : mShaders)
      {
        for (auto &mesh : mMeshes)
        {
          auto &models = instantiatedModels[mesh.second.get()];

          // We can early out on this mesh if there are no models that use it.
          if (models.empty())
          {
            continue;
          }

          // We get the sub meshes that use the current shader, then draw them.
          auto range = mesh.second->mSubmeshes.equal_range(shader.second.get());

          if (mesh.second->GetInstanced())
          {
            buffer->bindVertexBuffer(1,
              mesh.second->mInstanceManager.InstanceBuffer(),
              0);
          }

          for (auto it = range.first; it != range.second; ++it)
          {
            auto &submesh = it->second;

            buffer->bindVertexBuffer(0,
              submesh->mVertexBuffer,
              0);

            buffer->bindIndexBuffer(submesh->mIndexBuffer,
              0,
              vk::IndexType::eUint32);



            if (mesh.second->GetInstanced())
            {
              auto data = submesh->mPipelineData;
              buffer->bindDescriptorSets(vk::PipelineBindPoint::eGraphics,
                data.mPipelineLayout,
                0,
                data.mDescriptorSet,
                nullptr);

              buffer->drawIndexed(static_cast<u32>(submesh->mIndexCount),
                1,
                0,
                0,
                0);
            }
            else
            {
              for (auto &model : models)
              {
                if (model->mUseAlphaBlending == false && model->mUseAdditiveBlending == false)
                {
                  continue;
                }

                if (model->mUseAlphaBlending)
                {
                  auto &pipeline = shader.second->mAlphaBlendShader;

                  buffer->bindPipeline(vk::PipelineBindPoint::eGraphics, pipeline);
                }
                else if (model->mUseAdditiveBlending)
                {
                  auto &pipeline = shader.second->mAdditiveBlendShader;

                  buffer->bindPipeline(vk::PipelineBindPoint::eGraphics, pipeline);
                }
                
                auto &data = model->mPipelineData[submesh.get()];
                buffer->bindDescriptorSets(vk::PipelineBindPoint::eGraphics,
                  data.mPipelineLayout,
                  0,
                  data.mDescriptorSet,
                  nullptr);

                buffer->drawIndexed(static_cast<u32>(submesh->mIndexCount),
                  1,
                  0,
                  0,
                  0);
              }
            }
          }
        }
      }

      if (i + 1 < mViewData.size())
      {
        buffer->nextSubpass(vk::SubpassContents::eInline);
      }
    }

    buffer->endRenderPass();
    buffer->end();

    vkhlf::SubmitInfo submit{{mFrameBufferSwapChain->getPresentSemaphore()},
                             {vk::PipelineStageFlagBits::eColorAttachmentOutput},
                             buffer,
                             mRenderCompleteSemaphore};

    mGraphicsQueue->submit(submit);
    mGraphicsQueue->waitIdle();

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

    mGraphicsQueue->waitIdle();
  }
}
