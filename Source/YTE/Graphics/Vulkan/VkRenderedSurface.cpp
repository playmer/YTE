///////////////////
// Author: Andrew Griffin
// YTE - Graphics - Vulkan
///////////////////


#include "YTE/Core/Engine.hpp"

#include "YTE/Graphics/GraphicsSystem.hpp"
#include "YTE/Graphics/UBOs.hpp"
#include "YTE/Graphics/Vulkan/Drawers/VkImgui.hpp"
#include "YTE/Graphics/Vulkan/Drawers/VkRTGameForwardDrawer.hpp"
#include "YTE/Graphics/Vulkan/VkInstantiatedLight.hpp"
#include "YTE/Graphics/Vulkan/VkInstantiatedInfluenceMap.hpp"
#include "YTE/Graphics/Vulkan/VkInstantiatedModel.hpp"
#include "YTE/Graphics/Vulkan/VkInternals.hpp"
#include "YTE/Graphics/Vulkan/VkLightManager.hpp"
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
  YTEDefineEvent(VkAnimationUpdate);
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


  VkRenderedSurface::VkRenderedSurface(Window *aWindow,
                                       VkRenderer *aRenderer,
                                       std::shared_ptr<vkhlf::Surface> &aSurface)
    : mWindow(aWindow)
    , mRenderer(aRenderer)
    , mSurface(aSurface)
    , mDataUpdateRequired(true)
  {
    YTEProfileFunction();

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

    mRenderToScreen = std::make_unique<VkRenderToScreen>(mWindow, mRenderer, this, mColorFormat, mDepthFormat, mSurface, "RenderToScreen");

    mRenderCompleteSemaphore = mRenderer->mDevice->createSemaphore();
    mCubemapComplete = mRenderer->mDevice->createSemaphore();
    //mRenderPass1 = mRenderer->mDevice->createSemaphore();
    //mRenderPass2 = mDevice->createSemaphore();
    //mRenderPass3 = mDevice->createSemaphore();

    mAnimationUpdateCBOB = std::make_unique<VkCBOB<3, false>>(mRenderer->mCommandPool);
    mGraphicsDataUpdateCBOB = std::make_unique<VkCBOB<3, false>>(mRenderer->mCommandPool);
    mRenderingCBOB = std::make_unique<VkCBOB<3, false>>(mRenderer->mCommandPool);

    // create Framebuffer & Swapchain
    WindowResize event;
    event.height = mWindow->GetHeight();
    event.width = mWindow->GetWidth();

    ResizeInternal(true);

    mWindow->RegisterEvent<&VkRenderedSurface::ResizeEvent>(Events::WindowResize, this);
  }

  VkRenderedSurface::~VkRenderedSurface()
  {
    YTEProfileFunction();

    if (mCanPresent)
    {
      PresentFrame();
    }

    mViewData.clear();
    mShaderCreateInfos.clear();
    mRenderToScreen.reset();
  }
  
  void VkRenderedSurface::UpdateSurfaceViewBuffer(GraphicsView *aView, UBOs::View &aUBOView)
  {
    YTEProfileFunction();

    GetViewData(aView)->mViewUBOData = aUBOView;
    ////GetViewData(aView).mViewUBOData.mProjectionMatrix[0][0] *= -1;   // flips vulkan x axis right, since it defaults down
    //GetViewData(aView).mViewUBOData.mProjectionMatrix[1][1] *= -1;   // flips vulkan y axis up, since it defaults down
    this->RegisterEvent<&VkRenderedSurface::GraphicsDataUpdateVkEvent>(Events::VkGraphicsDataUpdate, this);
  }

  void VkRenderedSurface::UpdateSurfaceIlluminationBuffer(GraphicsView *aView, UBOs::Illumination& aIllumination)
  {
    YTEProfileFunction();

    GetViewData(aView)->mIlluminationUBOData = aIllumination;
    this->RegisterEvent<&VkRenderedSurface::GraphicsDataUpdateVkEvent>(Events::VkGraphicsDataUpdate, this);
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
    YTEProfileFunction();

    mDataUpdateRequired = true;
    auto model = std::make_unique<VkInstantiatedModel>(aModelFile, this, aView);
    auto &instantiatedModels = GetViewData(aView)->mInstantiatedModels;
    instantiatedModels[static_cast<VkMesh*>(model->GetVkMesh())].push_back(model.get());
    return std::move(model);
  }

  std::unique_ptr<VkInstantiatedModel> VkRenderedSurface::CreateModel(GraphicsView *aView, Mesh *aMesh)
  {
    YTEProfileFunction();

    mDataUpdateRequired = true;

    auto model = std::make_unique<VkInstantiatedModel>(mRenderer->mMeshes[aMesh->mName].get(), this, aView);
    auto &instantiatedModels = GetViewData(aView)->mInstantiatedModels;
    instantiatedModels[static_cast<VkMesh*>(model->GetVkMesh())].push_back(model.get());
    return std::move(model);
  }

  void VkRenderedSurface::AddModel(VkInstantiatedModel *aModel)
  {
    YTEProfileFunction();

    auto &instantiatedModels = GetViewData(aModel->mView)->mInstantiatedModels;
    instantiatedModels[static_cast<VkMesh*>(aModel->GetVkMesh())].push_back(aModel);
  }

  std::shared_ptr<vkhlf::Device>& VkRenderedSurface::GetDevice()
  {
    return mRenderer->mDevice;
  }

  std::shared_ptr<vkhlf::DeviceMemoryAllocator>& VkRenderedSurface::GetAllocator(const std::string aName)
  {
    return mRenderer->mAllocators[aName];
  }


  void VkRenderedSurface::DestroyModel(GraphicsView *aView, VkInstantiatedModel *aModel)
  {
    if (aModel == nullptr)
    {
      return;
    }

    YTEProfileFunction();

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

    YTEProfileFunction();

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

  // Shader
  VkShader* VkRenderedSurface::CreateShader(std::string &aShaderSetName,
                                            std::shared_ptr<vkhlf::PipelineLayout> &aPipelineLayout,
                                            VkShaderDescriptions &aDescription,
                                            GraphicsView* aView)
  {
    auto shaderIt = mShaderCreateInfos.find(aShaderSetName);
    VkShader *shaderPtr{ nullptr };
    ViewData *view = GetViewData(aView);
    auto viewShaderIt = view->mShaders.find(aShaderSetName);

    // if view doesn't have shader, but surface does
    if (viewShaderIt == view->mShaders.end() && shaderIt != mShaderCreateInfos.end())
    {
      auto shader = std::make_unique<VkShader>(mShaderCreateInfos[aShaderSetName], view);

      shaderPtr = shader.get();

      view->mShaders[aShaderSetName] = std::move(shader);
    }
    // If surface doesn't have shader
    else if (shaderIt == mShaderCreateInfos.end())
    {
      VkCreatePipelineDataSet cpds = VkShader::CreateInfo(aShaderSetName,
                                                          this,
                                                          aPipelineLayout,
                                                          aDescription,
                                                          false);
                                    
      DebugObjection(cpds.mValid == false,
                  fmt::format("Shader {} failed to compile and had no previously compiled shader to use.                                      Compilation Message:                                      {}",
                              aShaderSetName, cpds.mErrorMessage)
                  .c_str());

      mShaderCreateInfos.emplace(aShaderSetName, cpds);

      auto shader = std::make_unique<VkShader>(mShaderCreateInfos[aShaderSetName], view);

      shaderPtr = shader.get();

      view->mShaders[aShaderSetName] = std::move(shader);
      //mDataUpdateRequired = true; // shades do not load data
    }
    // otherwise both have the shader
    else
    {
      shaderPtr = view->mShaders[aShaderSetName].get();
    }

    return shaderPtr;
  }



  std::unique_ptr<VkInstantiatedLight> VkRenderedSurface::CreateLight(GraphicsView* aView)
  {
    mDataUpdateRequired = true;
    auto light = GetViewData(aView)->mLightManager.CreateLight();
    return std::move(light);
  }

  std::unique_ptr<VkInstantiatedInfluenceMap> VkRenderedSurface::CreateWaterInfluenceMap(GraphicsView* aView)
  {
    mDataUpdateRequired = true;
    auto map = GetViewData(aView)->mWaterInfluenceMapManager.CreateMap();
    return std::move(map);
  }


  std::shared_ptr<vkhlf::CommandPool>& VkRenderedSurface::GetCommandPool()
  {
    return mRenderer->mCommandPool;
  }

  std::shared_ptr<vkhlf::Queue>& VkRenderedSurface::GetGraphicsQueue()
  {
    return mRenderer->mGraphicsQueue;
  }

  void VkRenderedSurface::ResizeInternal(bool aConstructing)
  {
    YTEProfileFunction();

    auto baseDevice = static_cast<vk::PhysicalDevice>
                                  (*(mRenderer->GetVkInternals()->GetPhysicalDevice().get()));
    vk::SurfaceKHR baseSurfaceKhr = static_cast<vk::SurfaceKHR>(*mSurface);

    auto supportDetails = SwapChainSupportDetails::QuerySwapChainSupport(baseDevice,
                                                                         baseSurfaceKhr);

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
    YTEProfileFunction();

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
    YTEProfileFunction();

    auto it = mViewData.find(aView);

    if (it == mViewData.end())
    {
      auto emplaced = mViewData.try_emplace(aView);

      auto uboAllocator = mRenderer->mAllocators[AllocatorTypes::UniformBufferObject];
      auto buffer = mRenderer->mDevice->createBuffer(sizeof(UBOs::View),
                                          vk::BufferUsageFlagBits::eTransferDst |
                                          vk::BufferUsageFlagBits::eUniformBuffer,
                                          vk::SharingMode::eExclusive,
                                          nullptr,
                                          vk::MemoryPropertyFlagBits::eDeviceLocal,
                                          uboAllocator);
      auto buffer2 = mRenderer->mDevice->createBuffer(sizeof(UBOs::Illumination),
                                           vk::BufferUsageFlagBits::eTransferDst |
                                           vk::BufferUsageFlagBits::eUniformBuffer,
                                           vk::SharingMode::eExclusive,
                                           nullptr,
                                           vk::MemoryPropertyFlagBits::eDeviceLocal,
                                           uboAllocator);

      auto &view = emplaced.first->second;

      view.mName = aView->GetOwner()->GetGUID().ToIdentifierString();
      view.mView = aView;
      view.mViewUBO = buffer;
      view.mIlluminationUBO = buffer2;
      view.mLightManager.SetSurfaceAndView(this, aView);
      view.mWaterInfluenceMapManager.SetSurfaceAndView(this, aView);
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
    YTEProfileFunction();

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
    YTEProfileFunction();

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
    YTEProfileFunction();

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
    YTEProfileFunction();

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

  void VkRenderedSurface::GraphicsDataUpdateVkEvent(VkGraphicsDataUpdate *aEvent)
  {
    YTEProfileFunction();

    for (auto const&[view, data] : mViewData)
    {
      YTEMetaProfileBlock(data.mName.c_str());

      data.mViewUBO->update<UBOs::View>(0, data.mViewUBOData, aEvent->mCBO);
      data.mIlluminationUBO->update<UBOs::Illumination>(0, data.mIlluminationUBOData, aEvent->mCBO);
      this->DeregisterEvent<&VkRenderedSurface::GraphicsDataUpdateVkEvent>(Events::VkGraphicsDataUpdate, this);
    }
  }

  void VkRenderedSurface::FrameUpdate(LogicUpdate *aEvent)
  {
    YTEProfileFunction();
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
    if (mCanPresent == false)
    {
      return;
    }

    // wait till rendering is complete
    mRenderer->mGraphicsQueue->waitIdle();
    
    if (mRenderToScreen->PresentFrame(mRenderer->mGraphicsQueue, mRenderCompleteSemaphore) == false)
    {
      // create Framebuffer & Swapchain
      WindowResize event;
      event.height = mWindow->GetHeight();
      event.width = mWindow->GetWidth();
      ResizeEvent(&event);
    }

    mCanPresent = false;
  }

  void VkRenderedSurface::GraphicsDataUpdate()
  {
    VkGraphicsDataUpdate update;
    mGraphicsDataUpdateCBOB->NextCommandBuffer();
    update.mCBO = mGraphicsDataUpdateCBOB->GetCurrentCBO();

    update.mCBO->begin();

    SendEvent(Events::VkGraphicsDataUpdate, &update);

    update.mCBO->end();

    vkhlf::submitAndWait(mRenderer->mGraphicsQueue, update.mCBO);
  }


  void VkRenderedSurface::ReloadAllShaders()
  {
    GetRenderer()->GetEngine()->Log(LogType::Information, fmt::format("\n\nReloading All Shaders:"));

    // reconstruct
    for (auto &shader : mShaderCreateInfos)
    {
      VkCreatePipelineDataSet cpds = VkShader::CreateInfo(shader.second.mName,
                                                          this,
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


  void VkRenderedSurface::AnimationUpdate()
  {
    VkGraphicsDataUpdate update;
    mAnimationUpdateCBOB->NextCommandBuffer();
    update.mCBO = mAnimationUpdateCBOB->GetCurrentCBO();
    update.mCBO->begin();
    SendEvent(Events::VkAnimationUpdate, &update);
    update.mCBO->end();
    vkhlf::submitAndWait(mRenderer->mGraphicsQueue, update.mCBO);
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
    YTEProfileFunction();

    {
      YTEMetaProfileBlock("mRenderer->mGraphicsQueue->waitIdle()");
      mRenderer->mGraphicsQueue->waitIdle();
    }

    if (mWindow->mKeyboard.IsKeyDown(Keys::Control) && mWindow->mKeyboard.IsKeyDown(Keys::R))
    {
      ReloadAllShaders();
    }

    if (mDataUpdateRequired)
    {
      GraphicsDataUpdate();
    }

    
    std::array<float, 4> colorValues;

    vk::ClearDepthStencilValue depthStencil{1.0f, 0};

    auto &extent = mRenderToScreen->GetExtent();
    mRenderingCBOB->NextCommandBuffer();


    // build secondaries
    {
      YTEMetaProfileBlock("Building Secondary Command Buffers");

      for (auto const& [view, data] : mViewData)
      {
        YTEMetaProfileBlock(data.mName.c_str());
        
        data.mRenderTarget->RenderFull(mRenderer->mMeshes);
        data.mRenderTarget->MoveToNextEvent();
      }
    }

    // render to screen;
    {
      YTEMetaProfileBlock("Building Secondary Command Buffers");

      mRenderToScreen->RenderFull(extent);
      mRenderToScreen->MoveToNextEvent();
    }

    // cube map render
    std::vector<std::shared_ptr<vkhlf::Semaphore>> waitSemaphores = { mRenderToScreen->GetPresentSemaphore() };


    // build primary
    auto cbo = mRenderingCBOB->GetCurrentCBO();
    cbo->begin();

    // render all first pass render targets
    // wait on present semaphore for first render
    {
      YTEMetaProfileBlock("Building Primary Command Buffer");

      for (auto const&[view, data] : mViewData)
      {
        YTEMetaProfileBlock(data.mName.c_str());

        data.mRenderTarget->ExecuteSecondaryEvent(cbo);

        glm::vec4 col = data.mClearColor;

        colorValues[0] = col.x;
        colorValues[1] = col.y;
        colorValues[2] = col.z;
        colorValues[3] = col.w;
        vk::ClearValue color{ colorValues };

        cbo->beginRenderPass(data.mRenderTarget->GetRenderPass(),
                             data.mRenderTarget->GetFrameBuffer(),
                             vk::Rect2D({ 0, 0 }, data.mRenderTarget->GetRenderTargetData()->mExtent),
                             { color, depthStencil },
                             vk::SubpassContents::eSecondaryCommandBuffers);

        data.mRenderTarget->ExecuteCommands(cbo);

        cbo->endRenderPass();
      }
    }

    colorValues[0] = 1.0f;
    colorValues[1] = 0.0f;
    colorValues[2] = 0.0f;
    colorValues[3] = 1.0f;
    vk::ClearValue color{ colorValues };

    mRenderToScreen->ExecuteSecondaryEvent(cbo);

    cbo->beginRenderPass(mRenderToScreen->GetRenderPass(),
                         mRenderToScreen->GetFrameBuffer(),
                         vk::Rect2D({ 0, 0 }, extent),
                         { color, depthStencil },
                         vk::SubpassContents::eSecondaryCommandBuffers);

    mRenderToScreen->ExecuteCommands(cbo);

    cbo->endRenderPass();
    
    cbo->end();

    vk::ArrayProxy<const std::shared_ptr<vkhlf::Semaphore>> vkWaitSemaphores(waitSemaphores);

    // submit
    vkhlf::SubmitInfo submit{ vkWaitSemaphores,
                              { vk::PipelineStageFlagBits::eColorAttachmentOutput },
                              cbo,
                              mRenderCompleteSemaphore };

    {
      YTEMetaProfileBlock("Submitting to the Queue");

      mRenderer->mGraphicsQueue->submit(submit);
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
