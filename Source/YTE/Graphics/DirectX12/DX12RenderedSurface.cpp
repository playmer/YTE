///////////////////
// Author: Andrew Griffin
// YTE - Graphics - Vulkan
///////////////////


#include "YTE/Core/Engine.hpp"

#include "YTE/Graphics/GraphicsSystem.hpp"
#include "YTE/Graphics/UBOs.hpp"
#include "YTE/Graphics/DirectX12/DX12Drawers/VkImgui.hpp"
#include "YTE/Graphics/DirectX12/DX12Drawers/VkRTGameForwardDrawer.hpp"
#include "YTE/Graphics/DirectX12/DX12VkInstantiatedLight.hpp"
#include "YTE/Graphics/DirectX12/DX12VkInstantiatedInfluenceMap.hpp"
#include "YTE/Graphics/DirectX12/DX12VkInstantiatedModel.hpp"
#include "YTE/Graphics/DirectX12/DX12Dx12Internals.hpp"
#include "YTE/Graphics/DirectX12/DX12VkLightManager.hpp"
#include "YTE/Graphics/DirectX12/DX12VkWaterInfluenceMapManager.hpp"
#include "YTE/Graphics/DirectX12/DX12VkMesh.hpp"
#include "YTE/Graphics/DirectX12/DX12Dx12Renderer.hpp"
#include "YTE/Graphics/DirectX12/DX12Dx12RenderedSurface.hpp"
#include "YTE/Graphics/DirectX12/DX12VkRenderToScreen.hpp"
#include "YTE/Graphics/DirectX12/DX12Dx12Shader.hpp"
#include "YTE/Graphics/DirectX12/DX12VkTexture.hpp"

#include "YTE/StandardLibrary/Range.hpp"

#include "YTE/Utilities/Utilities.hpp"

namespace YTE
{
  YTEDefineEvent(AnimationUpdateVk);
  YTEDefineEvent(GraphicsDataUpdateVk);
  YTEDefineType(GraphicsDataUpdateVk)
  {
    RegisterType<GraphicsDataUpdateVk>();
    TypeBuilder<GraphicsDataUpdateVk> builder;
  }

  YTEDefineType(Dx12RenderedSurface)
  {
    RegisterType<Dx12RenderedSurface>();
    TypeBuilder<Dx12RenderedSurface> builder;
    GetStaticType()->AddAttribute<RunInEditor>();
  }


  Dx12RenderedSurface::Dx12RenderedSurface(Window *aWindow,
                                       Dx12Renderer *aRenderer,
                                       std::shared_ptr<vkhlf::Surface> &aSurface)
    : mWindow(aWindow)
    , mRenderer(aRenderer)
    , mSurface(aSurface)
    , mDataUpdateRequired(true)
  {
    auto internals = mRenderer->GetDx12Internals();

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

    mAnimationUpdateCBOB = std::make_unique<Dx12CBOB<3, false>>(mRenderer->mCommandPool);
    mGraphicsDataUpdateCBOB = std::make_unique<Dx12CBOB<3, false>>(mRenderer->mCommandPool);
    mRenderingCBOB = std::make_unique<Dx12CBOB<3, false>>(mRenderer->mCommandPool);

    // create Framebuffer & Swapchain
    WindowResize event;
    event.height = mWindow->GetHeight();
    event.width = mWindow->GetWidth();

    ResizeInternal(true);

    mWindow->RegisterEvent<&Dx12RenderedSurface::ResizeEvent>(Events::WindowResize, this);
  }

  Dx12RenderedSurface::~Dx12RenderedSurface()
  {
    if (mCanPresent)
    {
      PresentFrame();
    }

    mViewData.clear();
    mShaderCreateInfos.clear();
    mRenderToScreen.reset();
  }
  
  void Dx12RenderedSurface::UpdateSurfaceViewBuffer(GraphicsView *aView, UBOView &aUBOView)
  {
    GetViewData(aView)->mViewUBOData = aUBOView;
    ////GetViewData(aView).mViewUBOData.mProjectionMatrix[0][0] *= -1;   // flips vulkan x axis right, since it defaults down
    //GetViewData(aView).mViewUBOData.mProjectionMatrix[1][1] *= -1;   // flips vulkan y axis up, since it defaults down
    this->RegisterEvent<&Dx12RenderedSurface::GraphicsDataUpdateVkEvent>(Events::GraphicsDataUpdateVk, this);
  }

  void Dx12RenderedSurface::UpdateSurfaceIlluminationBuffer(GraphicsView *aView, UBOIllumination& aIllumination)
  {
    GetViewData(aView)->mIlluminationUBOData = aIllumination;
    this->RegisterEvent<&Dx12RenderedSurface::GraphicsDataUpdateVkEvent>(Events::GraphicsDataUpdateVk, this);
  }

  void Dx12RenderedSurface::PrintSurfaceFormats(std::vector<vk::SurfaceFormatKHR> &aFormats)
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
  std::unique_ptr<VkInstantiatedModel> Dx12RenderedSurface::CreateModel(GraphicsView *aView, std::string &aModelFile)
  {
    mDataUpdateRequired = true;
    auto model = std::make_unique<VkInstantiatedModel>(aModelFile, this, aView);
    auto &instantiatedModels = GetViewData(aView)->mInstantiatedModels;
    instantiatedModels[static_cast<VkMesh*>(model->GetVkMesh())].push_back(model.get());
    return std::move(model);
  }

  std::unique_ptr<VkInstantiatedModel> Dx12RenderedSurface::CreateModel(GraphicsView *aView, Mesh *aMesh)
  {
    mDataUpdateRequired = true;

    auto model = std::make_unique<VkInstantiatedModel>(mRenderer->mMeshes[aMesh->mName].get(), this, aView);
    auto &instantiatedModels = GetViewData(aView)->mInstantiatedModels;
    instantiatedModels[static_cast<VkMesh*>(model->GetVkMesh())].push_back(model.get());
    return std::move(model);
  }

  void Dx12RenderedSurface::AddModel(VkInstantiatedModel *aModel)
  {
    auto &instantiatedModels = GetViewData(aModel->mView)->mInstantiatedModels;
    instantiatedModels[static_cast<VkMesh*>(aModel->GetVkMesh())].push_back(aModel);
  }

  std::shared_ptr<vkhlf::Device>& Dx12RenderedSurface::GetDevice()
  {
    return mRenderer->mDevice;
  }

  std::shared_ptr<vkhlf::DeviceMemoryAllocator>& Dx12RenderedSurface::GetAllocator(const std::string aName)
  {
    return mRenderer->mAllocators[aName];
  }


  void Dx12RenderedSurface::DestroyModel(GraphicsView *aView, VkInstantiatedModel *aModel)
  {
    if (aModel == nullptr)
    {
      return;
    }

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

  void Dx12RenderedSurface::DestroyMeshAndModel(GraphicsView *aView, VkInstantiatedModel *aModel)
  {
    if (aModel == nullptr)
    {
      return;
    }

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
  Dx12Shader* Dx12RenderedSurface::CreateShader(std::string &aShaderSetName,
                                            std::shared_ptr<vkhlf::PipelineLayout> &aPipelineLayout,
                                            Dx12ShaderDescriptions &aDescription,
                                            GraphicsView* aView)
  {
    auto shaderIt = mShaderCreateInfos.find(aShaderSetName);
    Dx12Shader *shaderPtr{ nullptr };
    ViewData *view = GetViewData(aView);
    auto viewShaderIt = view->mShaders.find(aShaderSetName);

    // if view doesn't have shader, but surface does
    if (viewShaderIt == view->mShaders.end() && shaderIt != mShaderCreateInfos.end())
    {
      auto shader = std::make_unique<Dx12Shader>(mShaderCreateInfos[aShaderSetName], view);

      shaderPtr = shader.get();

      view->mShaders[aShaderSetName] = std::move(shader);
    }
    // If surface doesn't have shader
    else if (shaderIt == mShaderCreateInfos.end())
    {
      VkCreatePipelineDataSet cpds = Dx12Shader::CreateInfo(aShaderSetName,
                                                          this,
                                                          aPipelineLayout,
                                                          aDescription,
                                                          false);
                                    
      DebugObjection(cpds.mValid == false,
                  fmt::format("Shader {} failed to compile and had no previously compiled shader to use.                                      Compilation Message:                                      {}",
                              aShaderSetName, cpds.mErrorMessage)
                  .c_str());

      mShaderCreateInfos.emplace(aShaderSetName, cpds);

      auto shader = std::make_unique<Dx12Shader>(mShaderCreateInfos[aShaderSetName], view);

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



  std::unique_ptr<VkInstantiatedLight> Dx12RenderedSurface::CreateLight(GraphicsView* aView)
  {
    mDataUpdateRequired = true;
    auto light = GetViewData(aView)->mLightManager.CreateLight();
    return std::move(light);
  }

  std::unique_ptr<VkInstantiatedInfluenceMap> Dx12RenderedSurface::CreateWaterInfluenceMap(GraphicsView* aView)
  {
    mDataUpdateRequired = true;
    auto map = GetViewData(aView)->mWaterInfluenceMapManager.CreateMap();
    return std::move(map);
  }


  std::shared_ptr<vkhlf::CommandPool>& Dx12RenderedSurface::GetCommandPool()
  {
    return mRenderer->mCommandPool;
  }

  std::shared_ptr<vkhlf::Queue>& Dx12RenderedSurface::GetGraphicsQueue()
  {
    return mRenderer->mGraphicsQueue;
  }

  void Dx12RenderedSurface::ResizeInternal(bool aConstructing)
  {
    auto baseDevice = static_cast<vk::PhysicalDevice>
                                  (*(mRenderer->GetDx12Internals()->GetPhysicalDevice().get()));
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

  void Dx12RenderedSurface::ResizeEvent(WindowResize *aEvent)
  {
    UnusedArguments(aEvent);

    ResizeInternal(false);
  }

  void Dx12RenderedSurface::RegisterView(GraphicsView *aView)
  {
    RegisterView(aView, DrawerTypes::DefaultDrawer, DrawerTypeCombination::DefaultCombination);
  }

  void Dx12RenderedSurface::RegisterView(GraphicsView *aView,
                                       DrawerTypes aDrawerType,
                                       DrawerTypeCombination aCombination)
  {
    auto it = mViewData.find(aView);

    if (it == mViewData.end())
    {
      auto emplaced = mViewData.try_emplace(aView);

      auto uboAllocator = mRenderer->mAllocators[AllocatorTypes::UniformBufferObject];
      auto buffer = mRenderer->mDevice->createBuffer(sizeof(UBOView),
                                          vk::BufferUsageFlagBits::eTransferDst |
                                          vk::BufferUsageFlagBits::eUniformBuffer,
                                          vk::SharingMode::eExclusive,
                                          nullptr,
                                          vk::MemoryPropertyFlagBits::eDeviceLocal,
                                          uboAllocator);
      auto buffer2 = mRenderer->mDevice->createBuffer(sizeof(UBOIllumination),
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

  void Dx12RenderedSurface::SetViewDrawingType(GraphicsView *aView,
                                             DrawerTypes aDrawerType,
                                             DrawerTypeCombination aCombination)
  {
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

  void Dx12RenderedSurface::SetViewCombinationType(GraphicsView *aView,
                                                    DrawerTypeCombination aCombination)
  {
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

  void Dx12RenderedSurface::DeregisterView(GraphicsView *aView)
  {
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

  void Dx12RenderedSurface::ViewOrderChanged(GraphicsView *aView, float aNewOrder)
  {
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

  void Dx12RenderedSurface::GraphicsDataUpdateVkEvent(GraphicsDataUpdateVk *aEvent)
  {
    for (auto const&[view, data] : mViewData)
    {
      data.mViewUBO->update<UBOView>(0, data.mViewUBOData, aEvent->mCBO);
      data.mIlluminationUBO->update<UBOIllumination>(0, data.mIlluminationUBOData, aEvent->mCBO);
      this->DeregisterEvent<&Dx12RenderedSurface::GraphicsDataUpdateVkEvent>(Events::GraphicsDataUpdateVk, this);
    }
  }

  void Dx12RenderedSurface::FrameUpdate(LogicUpdate *aEvent)
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

  void Dx12RenderedSurface::PresentFrame()
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

  void Dx12RenderedSurface::GraphicsDataUpdate()
  {
    GraphicsDataUpdateVk update;
    mGraphicsDataUpdateCBOB->NextCommandBuffer();
    update.mCBO = mGraphicsDataUpdateCBOB->GetCurrentCBO();

    update.mCBO->begin();

    SendEvent(Events::GraphicsDataUpdateVk, &update);

    update.mCBO->end();

    vkhlf::submitAndWait(mRenderer->mGraphicsQueue, update.mCBO);
  }


  void Dx12RenderedSurface::ReloadAllShaders()
  {
    GetRenderer()->GetEngine()->Log(LogType::Information, fmt::format("\n\nReloading All Shaders:"));

    // reconstruct
    for (auto &shader : mShaderCreateInfos)
    {
      VkCreatePipelineDataSet cpds = Dx12Shader::CreateInfo(shader.second.mName,
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


  void Dx12RenderedSurface::AnimationUpdate()
  {
    GraphicsDataUpdateVk update;
    mAnimationUpdateCBOB->NextCommandBuffer();
    update.mCBO = mAnimationUpdateCBOB->GetCurrentCBO();
    update.mCBO->begin();
    SendEvent(Events::AnimationUpdateVk, &update);
    update.mCBO->end();
    vkhlf::submitAndWait(mRenderer->mGraphicsQueue, update.mCBO);
  }



  void Dx12RenderedSurface::SetLights(bool aOnOrOff)
  {
    for (auto& view : mViewData)
    {
      view.second.mLightManager.SetLights(aOnOrOff);
    }
  }



  void Dx12RenderedSurface::RenderFrameForSurface()
  {
    mRenderer->mGraphicsQueue->waitIdle();

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
    for (auto const& [view, data] : mViewData)
    {
      data.mRenderTarget->RenderFull(mRenderer->mMeshes);
      data.mRenderTarget->MoveToNextEvent();
    }


    // render to screen;
    mRenderToScreen->RenderFull(extent);
    mRenderToScreen->MoveToNextEvent();


    // cube map render
    std::vector<std::shared_ptr<vkhlf::Semaphore>> waitSemaphores = { mRenderToScreen->GetPresentSemaphore() };


    // build primary
    auto cbo = mRenderingCBOB->GetCurrentCBO();
    cbo->begin();

    // render all first pass render targets
    // wait on present semaphore for first render
    for (auto &v : mViewData)
    {
      v.second.mRenderTarget->ExecuteSecondaryEvent(cbo);

      glm::vec4 col = v.second.mClearColor;

      colorValues[0] = col.x;
      colorValues[1] = col.y;
      colorValues[2] = col.z;
      colorValues[3] = col.w;
      vk::ClearValue color{ colorValues };

      cbo->beginRenderPass(v.second.mRenderTarget->GetRenderPass(),
                           v.second.mRenderTarget->GetFrameBuffer(),
                           vk::Rect2D({ 0, 0 }, v.second.mRenderTarget->GetRenderTargetData()->mExtent),
                           { color, depthStencil },
                           vk::SubpassContents::eSecondaryCommandBuffers);

      v.second.mRenderTarget->ExecuteCommands(cbo);

      cbo->endRenderPass();
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
    
    mRenderer->mGraphicsQueue->submit(submit);

    mCanPresent = true;
  }


  std::unique_ptr<VkRenderTarget> Dx12RenderedSurface::CreateRenderTarget(DrawerTypes aDrawerType, 
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
        return std::move(std::make_unique<Dx12ImguiDrawer>(this,
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
