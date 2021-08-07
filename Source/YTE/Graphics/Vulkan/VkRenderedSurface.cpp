#include "YTE/Core/Engine.hpp"

#include "YTE/Graphics/GraphicsSystem.hpp"
#include "YTE/Graphics/UBOs.hpp"
#include "YTE/Graphics/Vulkan/Drawers/VkImgui.hpp"
#include "YTE/Graphics/Vulkan/Drawers/VkRTGameForwardDrawer.hpp"
#include "YTE/Graphics/Vulkan/VkInstantiatedModel.hpp"
#include "YTE/Graphics/Vulkan/VkInternals.hpp"
#include "YTE/Graphics/Vulkan/VkWaterInfluenceMapManager.hpp"
#include "YTE/Graphics/Vulkan/VkMesh.hpp"
#include "YTE/Graphics/Vulkan/VkRenderer.hpp"
#include "YTE/Graphics/Vulkan/VkRenderedSurface.hpp"
#include "YTE/Graphics/Vulkan/VkRenderToScreen.hpp"
#include "YTE/Graphics/Vulkan/VkShader.hpp"
#include "YTE/Graphics/Vulkan/VkTexture.hpp"

#include "YTE/StandardLibrary/Range.hpp"

#include "YTE/Utilities/Utilities.hpp"

namespace YTE
{
  YTEDefineEvent(VkGraphicsDataUpdate);
  YTEDefineType(VkGraphicsDataUpdate)
  {
    RegisterType<VkGraphicsDataUpdate>();
    TypeBuilder<VkGraphicsDataUpdate> builder;
  }

  YTEDefineType(VkRenderedSurface)
  {
    RegisterType<VkRenderedSurface>();
    TypeBuilder<VkRenderedSurface> builder;
    GetStaticType()->AddAttribute<RunInEditor>();
  }

  
  DescriptorPoolManager::DescriptorPoolManager(VkRenderedSurface* aSurface)
    : mSurface{aSurface}
  {

  }
  
  auto DescriptorPoolManager::AllocateDescriptorSet(
    std::vector<vk::DescriptorPoolSize> const& aDescriptor,
    std::shared_ptr<vkhlf::DescriptorSetLayout>& aLayout)
    -> std::shared_ptr<vkhlf::DescriptorSet>
  {
    OPTICK_EVENT();
    PoolInfo* info = nullptr;

    std::lock_guard lock{ mPoolMutex };
    std::string descriptorIdentifier;
    StringFromDescriptors(aDescriptor, descriptorIdentifier);
    auto& device = mSurface->GetDevice();
    
    auto it = mPools.find(descriptorIdentifier);
    if (it != mPools.end())
    {
      auto subPoolIt = std::find_if(it->second.begin(), it->second.end(), [](PoolInfo& subPool)
      {
        return subPool.mAvailible != 0;
      });

      if (subPoolIt != it->second.end())
      {
        info = &(*subPoolIt);
      }
    }

    //std::cout << descriptorIdentifier << "\n";
    //
    //if (info != nullptr)
    //  std::cout << "\tFound existing pool\n";

    if (info == nullptr)
    {
      auto descriptorCopy = aDescriptor;

      for (auto& item : descriptorCopy)
      {
        item.descriptorCount *= 40;
      }

      info = &mPools[descriptorIdentifier].emplace_back(PoolInfo{});
      info->mPool = device->createDescriptorPool({}, 40, descriptorCopy);
      info->mAvailible = 40;
    }

    DebugObjection(info == nullptr, "info must be initialized by this point.");

    --(info->mAvailible);

    //std::cout << "\tDescriptors\n";
    //for (auto& item : aDescriptor)
    //{
    //  std::cout << "\t\t" << vk::to_string(item.type) << ": " << std::to_string(item.descriptorCount) << "\n";
    //}
    //
    //std::cout << "\tLayout\n";
    //for (auto& item : aLayout->getBindings())
    //{
    //  std::cout << "\t\t" << fmt::format("Descriptor({}); Flags({}); Samplers({}); Binding({})", vk::to_string(item.descriptorType), vk::to_string(item.stageFlags), item.immutableSamplers.size(), item.binding) << "\n";
    //}

    return mSurface->GetDevice()->allocateDescriptorSet(info->mPool, aLayout);
  }

  
  void DescriptorPoolManager::StringFromDescriptors(std::vector<vk::DescriptorPoolSize> const& aDescriptor, std::string& aOut)
  {
    for (auto& poolSize : aDescriptor)
    {
      char const* name = nullptr;
      switch (poolSize.type)
      {  
        case vk::DescriptorType::eSampler: 
        {
          name = "S";
          break;
        }
        case vk::DescriptorType::eCombinedImageSampler: 
        {
          name = "CIS";
          break;
        }
        case vk::DescriptorType::eSampledImage: 
        {
          name = "SaI";
          break;
        }
        case vk::DescriptorType::eStorageImage: 
        {
          name = "SI";
          break;
        }
        case vk::DescriptorType::eUniformTexelBuffer: 
        {
          name = "UTB";
          break;
        }
        case vk::DescriptorType::eStorageTexelBuffer: 
        {
          name = "STB";
          break;
        }
        case vk::DescriptorType::eUniformBuffer: 
        {
          name = "UB";
          break;
        }
        case vk::DescriptorType::eStorageBuffer: 
        {
          name = "SB";
          break;
        }
        case vk::DescriptorType::eUniformBufferDynamic: 
        {
          name = "UBD";
          break;
        }
        case vk::DescriptorType::eStorageBufferDynamic: 
        {
          name = "SBD";
          break;
        }
        case vk::DescriptorType::eInputAttachment: 
        {
          name = "IA";
          break;
        }
      }
      
      aOut += fmt::format("{}{}", name, poolSize.descriptorCount);
    }
  }


  vk::Format findSupportedFormat(std::shared_ptr<vkhlf::PhysicalDevice> aDevice, const std::vector<vk::Format>& candidates, vk::ImageTiling tiling, vk::FormatFeatureFlags features) {
    for (vk::Format format : candidates) {
        vk::FormatProperties props = aDevice->getFormatProperties(format);

        if (tiling == vk::ImageTiling::eLinear && (props.linearTilingFeatures & features) == features) {
            return format;
        } else if (tiling == vk::ImageTiling::eOptimal && (props.optimalTilingFeatures & features) == features) {
            return format;
        }
    }

    throw std::runtime_error("failed to find supported format!");
}

  VkRenderedSurface::VkRenderedSurface(Window *aWindow,
                                       VkRenderer *aRenderer,
                                       std::shared_ptr<vkhlf::Surface> &aSurface)
    : mPoolManager{this}
    , mWindow(aWindow)
    , mRenderer(aRenderer)
    , mSurface(aSurface)
    , mDataUpdateRequired(true)
  {
    OPTICK_EVENT();

    auto internals = mRenderer->GetVkInternals();

    auto supportDetails = SwapChainSupportDetails::QuerySwapChainSupport(internals->GetPhysicalDevice(),
                                                                         mSurface);
    auto formats = supportDetails.Formats();

    // If the format list includes just one entry of VK_FORMAT_UNDEFINED,
    // the surface has no preferred format.  Otherwise, at least one
    // supported format will be returned.
    mColorFormat = ((formats.size() == 1) && (formats[0].format == vk::Format::eUndefined)) ?
                                                                 vk::Format::eB8G8R8A8Unorm :
                                                                 formats[0].format;

    PrintSurfaceFormats(formats);
    mDepthFormat = findSupportedFormat(aRenderer->mDevice->get<vkhlf::PhysicalDevice>(), { vk::Format::eD32Sfloat, vk::Format::eD32SfloatS8Uint, vk::Format::eD24UnormS8Uint }, vk::ImageTiling::eOptimal, vk::FormatFeatureFlagBits::eDepthStencilAttachment);

    //std::array<vk::Format, 3> depthFormats = 
    //{
    //  
    //aRenderer->mDevice->get<vkhlf::PhysicalDevice>()->getFormatProperties()
    //  mDepthFormat = vk::Format::eD24UnormS8Uint;
    //}


    mRenderToScreen = std::make_unique<VkRenderToScreen>(mWindow, mRenderer, this, mColorFormat, mDepthFormat, mSurface, "RenderToScreen");

    mRenderCompleteSemaphore = mRenderer->mDevice->createSemaphore();

    mTransferBufferedCommandBuffer = std::make_unique<VkCBOB<3, false>>(mRenderer->mTransferQueueData->mCommandPool);
    mRenderingCBOB = std::make_unique<VkCBOB<3, false>>(mRenderer->mGraphicsQueueData->mCommandPool);

    // create Framebuffer & Swapchain
    WindowResize event;
    event.height = mWindow->GetHeight();
    event.width = mWindow->GetWidth();

    ResizeInternal(true);

    mWindow->RegisterEvent<&VkRenderedSurface::ResizeEvent>(Events::WindowResize, this);
  }

  VkRenderedSurface::~VkRenderedSurface()
  {
    OPTICK_EVENT();

    if (mCanPresent)
    {
      PresentFrame();
    }

    mViewData.clear();
    mShaderCreateInfos.clear();
    mRenderToScreen.reset();
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
    OPTICK_EVENT();

    mDataUpdateRequired = true;
    auto model = std::make_unique<VkInstantiatedModel>(aModelFile, this, aView);
    auto &instantiatedModels = GetViewData(aView)->mInstantiatedModels;
    instantiatedModels[static_cast<VkMesh*>(model->GetVkMesh())].push_back(model.get());
    return std::move(model);
  }

  std::unique_ptr<VkInstantiatedModel> VkRenderedSurface::CreateModel(GraphicsView *aView, Mesh *aMesh)
  {
    OPTICK_EVENT();

    mDataUpdateRequired = true;

    auto model = std::make_unique<VkInstantiatedModel>(mRenderer->mMeshes[aMesh->mName].get(), this, aView);
    auto &instantiatedModels = GetViewData(aView)->mInstantiatedModels;
    instantiatedModels[static_cast<VkMesh*>(model->GetVkMesh())].push_back(model.get());
    return std::move(model);
  }

  void VkRenderedSurface::AddModel(VkInstantiatedModel *aModel)
  {
    OPTICK_EVENT();

    auto &instantiatedModels = GetViewData(aModel->mView)->mInstantiatedModels;
    instantiatedModels[static_cast<VkMesh*>(aModel->GetVkMesh())].push_back(aModel);
  }

  std::shared_ptr<vkhlf::Device>& VkRenderedSurface::GetDevice()
  {
    return mRenderer->mDevice;
  }

  //std::shared_ptr<vkhlf::DeviceMemoryAllocator>& VkRenderedSurface::GetAllocator(const std::string aName)
  //{
  //  return mRenderer->GetAllocator(aName);
  //}


  void VkRenderedSurface::DestroyModel(GraphicsView *aView, VkInstantiatedModel *aModel)
  {
    if (aModel == nullptr)
    {
      return;
    }

    OPTICK_EVENT();

    auto &instantiatedModels = GetViewData(aView)->mInstantiatedModels;

    auto mesh = instantiatedModels.find(aModel->GetVkMesh());

    if (mesh != instantiatedModels.end())
    {
      // Remove this instance from the map.
      mesh->second.erase(std::remove(mesh->second.begin(), 
                                     mesh->second.end(), 
                                     aModel),
                         mesh->second.end());
    }
  }

  void VkRenderedSurface::DestroyMeshAndModel(GraphicsView *aView, VkInstantiatedModel *aModel)
  {
    if (aModel == nullptr)
    {
      return;
    }

    OPTICK_EVENT();

    auto &instantiatedModels = GetViewData(aView)->mInstantiatedModels;

    auto mesh = instantiatedModels.find(aModel->GetVkMesh());

    if (mesh != instantiatedModels.end())
    {
      // Remove this instance from the map.
      mesh->second.erase(std::remove(mesh->second.begin(),
                                     mesh->second.end(),
                                     aModel),
                         mesh->second.end());

      instantiatedModels.erase(mesh);
    }
  }


  VkCreatePipelineDataSet* VkRenderedSurface::IfShaderExistsCreateOnView(
    std::string& aShaderSetName,
    GraphicsView* aView)
  {
    auto shaderIt = mShaderCreateInfos.find(aShaderSetName);
    ViewData* view = GetViewData(aView);
    auto viewShaderIt = view->mShaders.find(aShaderSetName);

    if (shaderIt == mShaderCreateInfos.end())
    {
      return nullptr;
    }
    else if (viewShaderIt == view->mShaders.end() && shaderIt != mShaderCreateInfos.end())
    {
      auto shader = std::make_unique<VkShader>(mShaderCreateInfos[aShaderSetName], view);
      view->mShaders[aShaderSetName] = std::move(shader);

      return &shaderIt->second;
    }
    else
    {
      return &shaderIt->second;
    }
  }

  // Shader
  VkCreatePipelineDataSet* VkRenderedSurface::CreateShader(
    std::string &aShaderSetName,
    std::shared_ptr<vkhlf::DescriptorSetLayout> aDescriptorSetLayout,
    std::shared_ptr<vkhlf::PipelineLayout> &aPipelineLayout,
    VkShaderDescriptions &aDescription,
    GraphicsView* aView)
  {
    auto shaderIt = mShaderCreateInfos.find(aShaderSetName);
    VkCreatePipelineDataSet* piplineInfoPtr{ nullptr };
    ViewData *view = GetViewData(aView);
    auto viewShaderIt = view->mShaders.find(aShaderSetName);

    // if view doesn't have shader, but surface does
    if (viewShaderIt == view->mShaders.end() && shaderIt != mShaderCreateInfos.end())
    {
      auto shader = std::make_unique<VkShader>(mShaderCreateInfos[aShaderSetName], view);

      piplineInfoPtr = &shaderIt->second;

      view->mShaders[aShaderSetName] = std::move(shader);
    }
    // If surface doesn't have shader
    else if (shaderIt == mShaderCreateInfos.end())
    {
      VkCreatePipelineDataSet cpds = VkShader::CreateInfo(
        aShaderSetName,
        this,
        aDescriptorSetLayout,
        aPipelineLayout,
        aDescription,
        false);
                                    
      DebugObjection(cpds.mValid == false, cpds.mErrorMessage.c_str());

      piplineInfoPtr = &mShaderCreateInfos.emplace(aShaderSetName, cpds).first->second;

      auto shader = std::make_unique<VkShader>(mShaderCreateInfos[aShaderSetName], view);
      
      view->mShaders[aShaderSetName] = std::move(shader);
    }
    // otherwise both have the shader
    else
    {
      piplineInfoPtr = &shaderIt->second;
    }

    return piplineInfoPtr;
  }

  void VkRenderedSurface::ResizeInternal(bool aConstructing)
  {
    OPTICK_EVENT();

    auto supportDetails = SwapChainSupportDetails::QuerySwapChainSupport(mRenderer->GetVkInternals()->GetPhysicalDevice(),
                                                                         mSurface);

    auto extent = supportDetails.Capabilities().currentExtent;

    if (0 == extent.width || 0 == extent.height)
    {
      return;
    }

    // resize all render targets (and swapchain)
    mRenderToScreen->Resize(extent);

    if (false == aConstructing)
    {
      // reset swapchain's references to render target frame buffers
      std::vector<VkRenderTarget*> rts;

      for (auto &v : mViewData)
      {
        auto superSampling = v.first->GetSuperSampling();

        auto renderTargetExtent = extent;
        renderTargetExtent.height *= superSampling;
        renderTargetExtent.width *= superSampling;

        v.second.mRenderTarget->Resize(renderTargetExtent);
        rts.push_back(v.second.mRenderTarget.get());
      }

      if (mViewData.size() != 0)
      {
        mRenderToScreen->ResetRenderTargets(rts);
      }
    }

    WindowResize event;
    event.height = extent.height;
    event.width = extent.width;
    mWindow->SendEvent(Events::RendererResize, &event);

    for (auto &view : mViewData)
    {
      view.first->SendEvent(Events::RendererResize, &event);
    }
  }

  void VkRenderedSurface::ResizeEvent(WindowResize *aEvent)
  {
    OPTICK_EVENT();

    UnusedArguments(aEvent);

    ResizeInternal(false);
  }

  void VkRenderedSurface::RegisterView(GraphicsView *aView)
  {
    RegisterView(aView, DrawerTypes::DefaultDrawer, DrawerTypeCombination::DefaultCombination);
  }

  void VkRenderedSurface::RegisterView(GraphicsView *aView,
                                       DrawerTypes aDrawerType,
                                       DrawerTypeCombination aCombination)
  {
    OPTICK_EVENT();

    auto it = mViewData.find(aView);

    if (it == mViewData.end())
    {
      auto emplaced = mViewData.try_emplace(aView);

      auto &view = emplaced.first->second;

      auto viewName = fmt::format("{}_{}", aView->GetOwner()->GetName().c_str(), aView->GetOwner()->GetGUID().ToIdentifierString());

      view.mName = viewName;
      view.mView = aView;
      view.mView->GetLightManager()->SetView(aView);
      view.mView->GetWaterInfluenceMapManager()->SetView(aView);
      view.mRenderTarget = CreateRenderTarget(aDrawerType, &view, aCombination);
      view.mRenderTarget->SetView(&view);
      view.mViewOrder = aView->GetOrder(); // default
      view.mRenderTarget->SetOrder(view.mViewOrder);
    }

    // reset swapchain's references to render target frame buffers
    std::vector<VkRenderTarget*> rts;
    for (auto const& [view, data] : mViewData)
    {
      rts.push_back(data.mRenderTarget.get());
    }

    if (mViewData.size() != 0)
    {
      mRenderToScreen->SetRenderTargets(rts);
    }
  }

  void VkRenderedSurface::SetViewDrawingType(GraphicsView *aView,
                                             DrawerTypes aDrawerType,
                                             DrawerTypeCombination aCombination)
  {
    OPTICK_EVENT();

    auto viewData = GetViewData(aView);
    viewData->mRenderTarget.reset();
    viewData->mRenderTarget = CreateRenderTarget(aDrawerType, viewData, aCombination);
    viewData->mRenderTarget->SetView(viewData);

    // reset swapchain's references to render target frame buffers
    std::vector<VkRenderTarget*> rts;
    for (auto const&[view, data] : mViewData)
    {
      rts.push_back(data.mRenderTarget.get());
    }

    if (mViewData.size() != 0)
    {
      mRenderToScreen->SetRenderTargets(rts);
    }
  }

  void VkRenderedSurface::SetViewCombinationType(GraphicsView *aView,
                                                    DrawerTypeCombination aCombination)
  {
    OPTICK_EVENT();

    auto viewData = GetViewData(aView);
    viewData->mRenderTarget->SetCombinationType(aCombination);
    viewData->mRenderTarget->SetView(viewData);

    // reset swapchain's references to render target frame buffers
    std::vector<VkRenderTarget*> rts;
    for (auto const& [view, data] : mViewData)
    {
      rts.push_back(data.mRenderTarget.get());
    }

    if (mViewData.size() != 0)
    {
      mRenderToScreen->SetRenderTargets(rts);
    }
  }

  void VkRenderedSurface::DeregisterView(GraphicsView *aView)
  {
    OPTICK_EVENT();

    auto it = mViewData.find(aView);

    if (it != mViewData.end())
    {
      mViewData.erase(it);
    }

    // reset swapchain's references to render target frame buffers
    std::vector<VkRenderTarget*> rts;
    for (auto const& [view, data] : mViewData)
    {
      rts.push_back(data.mRenderTarget.get());
    }

    if (mViewData.size() != 0)
    {
      mRenderToScreen->SetRenderTargets(rts);
    }
  }

  void VkRenderedSurface::ViewOrderChanged(GraphicsView *aView, float aNewOrder)
  {
    OPTICK_EVENT();

    auto it = mViewData.find(aView);

    if (it != mViewData.end())
    {
      it->second.mViewOrder = aNewOrder;
      it->second.mRenderTarget->SetOrder(aNewOrder);
    }

    // reset swapchain's references to render target frame buffers
    std::vector<VkRenderTarget*> rts;
    for (auto const&[view, data] : mViewData)
    {
      rts.push_back(data.mRenderTarget.get());
    }
    
    if (mViewData.size() != 0)
    {
      mRenderToScreen->SetRenderTargets(rts);
    }
  }

  void VkRenderedSurface::FrameUpdate(LogicUpdate *aEvent)
  {
    OPTICK_EVENT();
    UnusedArguments(aEvent);

    if (mWindow->IsMinimized() || mViewData.empty())
    {
      return;
    }

    if (mViewData.size() == 0)
    {
      return;
    }

    // Get the index of the next available swapchain image:
    mRenderToScreen->FrameUpdate();
    RenderFrameForSurface();
  }

  void VkRenderedSurface::PresentFrame()
  {
    OPTICK_EVENT();

    if (mCanPresent == false)
    {
      return;
    }

    // wait till rendering is complete
    //mRenderer->mGraphicsQueueData->mQueue->waitIdle();

    auto [graphicsCommandBuffer, graphicsFence] = **mRenderingCBOB;

    {
      OPTICK_EVENT("Waiting on rendering fence");
      waitOnFence(mRenderer->mDevice, { graphicsFence });
    }
    
    {
      OPTICK_EVENT("Presenting");

      if (mRenderToScreen->PresentFrame(mRenderer->mGraphicsQueueData->mQueue, mRenderCompleteSemaphore) == false)
      {
        // create Framebuffer & Swapchain
        WindowResize event;
        event.height = mWindow->GetHeight();
        event.width = mWindow->GetWidth();
        ResizeEvent(&event);
      }
    }

    mCanPresent = false;
  }

  void VkRenderedSurface::GraphicsDataUpdate()
  {
    VkGraphicsDataUpdate update;

    ++(*mTransferBufferedCommandBuffer);

    auto [transferCommandBuffer, transferFence] = **mTransferBufferedCommandBuffer;

    update.mCBO = transferCommandBuffer;

    update.mCBO->begin();

    SendEvent(Events::VkGraphicsDataUpdate, &update);

    update.mCBO->end();

    mRenderer->mTransferQueueData->mQueue->submit(update.mCBO, transferFence);

    mDataUpdateRequired = false;
  }


  void VkRenderedSurface::ReloadAllShaders()
  {
    GetRenderer()->GetEngine()->Log(LogType::Information, fmt::format("\n\nReloading All Shaders:"));

    // reconstruct
    for (auto &shader : mShaderCreateInfos)
    {
      VkCreatePipelineDataSet cpds = VkShader::CreateInfo(
        shader.second.mName,
        this,
        shader.second.mDescriptorSetLayout,
        shader.second.mPipelineLayout,
        shader.second.mDescriptions,
        true);

      // failed to compile, error already posted, just reuse the shader
      if (cpds.mValid)
      {
        shader.second = cpds;
      }
    }

    // reload
    for (auto &view : mViewData)
    {
      for (auto &shader : view.second.mShaders)
      {
        shader.second->Reload(mShaderCreateInfos[shader.first]);
      }
    }

    mRenderToScreen->ReloadShaders();
  }

  void VkRenderedSurface::SetLights(bool aOnOrOff)
  {
    for (auto& view : mViewData)
    {
      view.first->GetLightManager()->SetLights(aOnOrOff);
    }
  }

  void VkRenderedSurface::RenderFrameForSurface()
  {
    OPTICK_EVENT();

    //{
    //  YTEMetaProfileBlock("mRenderer->mGraphicsQueue->waitIdle()");
    //  mRenderer->mGraphicsQueueData->mQueue->waitIdle();
    //}

    if (mWindow->mKeyboard.IsKeyDown(Keys::Control) && mWindow->mKeyboard.IsKeyDown(Keys::R))
    {
      ReloadAllShaders();
    }

    if (mDataUpdateRequired)
    {
      #if YTE_Windows
        __debugbreak();
      #else
        __builtin_trap();
      #endif
      //GraphicsDataUpdate();
    }

    
    std::array<float, 4> colorValues;

    vk::ClearDepthStencilValue depthStencil{1.0f, 0};

    auto &extent = mRenderToScreen->GetExtent();
    ++(*mRenderingCBOB);


    // build secondaries
    {
      OPTICK_EVENT("Building Secondary Command Buffers");

      for (auto const& [view, data] : mViewData)
      {
        OPTICK_EVENT_DYNAMIC(data.mName.c_str());
        
        data.mRenderTarget->RenderFull(mRenderer->mMeshes);
      }
    }

    // render to screen;
    {
      OPTICK_EVENT("Building Secondary Command Buffers");

      mRenderToScreen->RenderFull(extent);
    }

    std::vector<std::shared_ptr<vkhlf::Semaphore>> waitSemaphores = { mRenderToScreen->GetPresentSemaphore() };

    // build primary
    auto [renderingCommandBuffer, renderingFence] = **mRenderingCBOB;

    renderingCommandBuffer->begin();

    // render all first pass render targets
    // wait on present semaphore for first render
    {
      OPTICK_EVENT("Building Primary Command Buffer");

      for (auto const&[view, data] : mViewData)
      {
        OPTICK_EVENT_DYNAMIC(data.mName.c_str());

        glm::vec4 col = data.mView->GetClearColor();

        colorValues[0] = col.x;
        colorValues[1] = col.y;
        colorValues[2] = col.z;
        colorValues[3] = col.w;
        vk::ClearValue color{ colorValues };

        renderingCommandBuffer->beginRenderPass(data.mRenderTarget->GetRenderPass(),
                                                data.mRenderTarget->GetFrameBuffer(),
                                                vk::Rect2D({ 0, 0 }, data.mRenderTarget->GetRenderTargetData()->mExtent),
                                                { color, depthStencil },
                                                vk::SubpassContents::eSecondaryCommandBuffers);

        renderingCommandBuffer->executeCommands(data.mRenderTarget->GetCommands());

        renderingCommandBuffer->endRenderPass();
      }
    }

    colorValues[0] = 1.0f;
    colorValues[1] = 0.0f;
    colorValues[2] = 0.0f;
    colorValues[3] = 1.0f;
    vk::ClearValue color{ colorValues };

    renderingCommandBuffer->beginRenderPass(mRenderToScreen->GetRenderPass(),
                                            mRenderToScreen->GetFrameBuffer(),
                                            vk::Rect2D({ 0, 0 }, extent),
                                            { color, depthStencil },
                                            vk::SubpassContents::eSecondaryCommandBuffers);


    renderingCommandBuffer->executeCommands(mRenderToScreen->GetCommands());

    renderingCommandBuffer->endRenderPass();
    
    renderingCommandBuffer->end();

    vk::ArrayProxy<const std::shared_ptr<vkhlf::Semaphore>> vkWaitSemaphores(waitSemaphores);

    // submit
    vkhlf::SubmitInfo submit{ vkWaitSemaphores,
                             { vk::PipelineStageFlagBits::eColorAttachmentOutput },
                             renderingCommandBuffer,
                             mRenderCompleteSemaphore };

    {
      OPTICK_EVENT("Waiting on fences.");

      auto [transferCommandBuffer, transferFence] = **mTransferBufferedCommandBuffer;

      waitOnFence(mRenderer->mDevice, { transferFence });
    }

    {
      OPTICK_EVENT("Submitting to the Queue");

      mRenderer->mGraphicsQueueData->mQueue->submit(submit, renderingFence);
    }

    mCanPresent = true;
  }


  std::unique_ptr<VkRenderTarget> VkRenderedSurface::CreateRenderTarget(DrawerTypes aDrawerType, 
                                                                        ViewData *view,
                                                                        DrawerTypeCombination aCombination)
  {
    switch (aDrawerType)
    {
      case DrawerTypes::GameForwardDrawer:
      {
        return std::move(std::make_unique<VkRTGameForwardDrawer>(this,
                                                                 mColorFormat,
                                                                 mDepthFormat,
                                                                 mSurface,
                                                                 view,
                                                                 aCombination));
        break;
      }
      case DrawerTypes::ImguiDrawer:
      {
        return std::move(std::make_unique<VkImguiDrawer>(this,
                                                         mColorFormat,
                                                         mDepthFormat,
                                                         mSurface,
                                                         view,
                                                         aCombination));
        break;
      }
      case DrawerTypes::DefaultDrawer:
      default:
      {
        return std::move(std::make_unique<VkRTGameForwardDrawer>(this, 
                                                                 mColorFormat,
                                                                 mDepthFormat,
                                                                 mSurface,
                                                                 view,
                                                                 aCombination));
        break;
      }
    }
  }
}
