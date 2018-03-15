///////////////////
// Author: Andrew Griffin
// YTE - Graphics - Vulkan
///////////////////

#pragma once

#ifndef YTE_Graphics_Vulkan_VkRendererdSurface_hpp
#define YTE_Graphics_Vulkan_VkRendererdSurface_hpp

#include "YTE/Core/Utilities.hpp"

#include "YTE/Graphics/GraphicsView.hpp"
#include "YTE/Graphics/UBOs.hpp"
#include "YTE/Graphics/Vulkan/ForwardDeclarations.hpp"
#include "YTE/Graphics/Vulkan/VkFunctionLoader.hpp"
#include "YTE/Graphics/Vulkan/VkShaderDescriptions.hpp"
#include "YTE/Graphics/Vulkan/VkLightManager.hpp"
#include "YTE/Graphics/Vulkan/VkRenderToScreen.hpp"
#include "YTE/Graphics/Vulkan/Drawers/VkRenderTarget.hpp"
#include "YTE/Graphics/Vulkan/VkCommandBufferBuffer.hpp"
#include "YTE/Graphics/Vulkan/VkShader.hpp"

namespace YTE
{
  YTEDeclareEvent(GraphicsDataUpdateVk);
  YTEDeclareEvent(AnimationUpdateVk);

  // forward declare
  struct VkCreatePipelineDataSet;

  // vulkan specific graphics data update event so that it can contain the CBO object
  class GraphicsDataUpdateVk : public Event
  {
  public:
    YTEDeclareType(GraphicsDataUpdateVk);
    std::shared_ptr<vkhlf::CommandBuffer> mCBO;
  };


  struct ViewData
  {
  public:
    ViewData() = default;

    // Buffers
    std::shared_ptr<vkhlf::Buffer> mViewUBO;
    std::shared_ptr<vkhlf::Buffer> mIlluminationUBO;
    std::shared_ptr<vkhlf::Buffer> mClipPlanesUBO;
    std::string mName = "EMPTY";

    // Engine Side Data
    glm::vec4 mClearColor;
    UBOView mViewUBOData;
    UBOIllumination mIlluminationUBOData;
    UBOClipPlanes mClipPlanesUBOData;
    VkLightManager mLightManager;
    std::unordered_map<VkMesh*, std::vector<VkInstantiatedModel*>> mInstantiatedModels;
    std::unordered_map<std::string, std::unique_ptr<VkShader>> mShaders;
    std::unique_ptr<VkRenderTarget> mRenderTarget;
    std::unique_ptr<VkRenderTarget> mWaterDrawer;
    float mViewOrder;
  };

  class VkRenderedSurface : public EventHandler
  {
  public:
    YTEDeclareType(VkRenderedSurface);

    VkRenderedSurface(Window *aWindow,
                      VkRenderer *aRenderer,
                      std::shared_ptr<vkhlf::Surface> &aSurface);

    ~VkRenderedSurface();

    void UpdateSurfaceIlluminationBuffer(GraphicsView* aView, UBOIllumination &aIllumination);
    void UpdateSurfaceClipPlanesBuffer(GraphicsView* aView, UBOClipPlanes &aClipPlanes);
    void UpdateSurfaceViewBuffer(GraphicsView *aView, UBOView &aUBOView);
    void PrintSurfaceFormats(std::vector<vk::SurfaceFormatKHR> &aFormats);


    /////////////////////////////////
    // Creation / Destruction
    /////////////////////////////////
    std::unique_ptr<VkInstantiatedModel> CreateModel(GraphicsView *aView, std::string &aModelFile);
    std::unique_ptr<VkInstantiatedModel> CreateModel(GraphicsView *aView, Mesh *aMesh);
    void AddModel(VkInstantiatedModel *aModel);
    void DestroyModel(GraphicsView *aView, VkInstantiatedModel *aModel);
    
    void DestroyMeshAndModel(GraphicsView *aView, VkInstantiatedModel *aModel);
    void DestroyMesh(GraphicsView *aView, VkMesh *aMesh);
    

    VkShader* CreateShader(std::string &aShaderSetName,
                           std::shared_ptr<vkhlf::PipelineLayout> &aPipelineLayout,
                           VkShaderDescriptions &aDescription, 
                           GraphicsView* aView);

    std::unique_ptr<VkInstantiatedLight> CreateLight(GraphicsView *aView);


    void ReloadAllShaders();

    /////////////////////////////////
    // Events
    /////////////////////////////////
    void ResizeEvent(WindowResize *aEvent);
    void GraphicsDataUpdateVkEvent(GraphicsDataUpdateVk *aEvent);
    void FrameUpdate(LogicUpdate *aEvent);
    void PresentFrame();
    void GraphicsDataUpdate();
    void AnimationUpdate();

    void SetLights(bool aOnOrOff);
    void RegisterView(GraphicsView *aView);
    void RegisterView(GraphicsView *aView, YTEDrawerTypes aDrawerType, YTEDrawerTypeCombination aCombination);
    void SetViewDrawingType(GraphicsView *aView, YTEDrawerTypes aDrawerType, YTEDrawerTypeCombination aCombination);
    void SetViewCombinationType(GraphicsView *aView, YTEDrawerTypeCombination aCombination);
    void DeregisterView(GraphicsView *aView);
    void ViewOrderChanged(GraphicsView *aView, float aNewOrder);

    /////////////////////////////////
    // Getters / Setters
    /////////////////////////////////
    Window* GetWindow() const
    {
      return mWindow;
    }

    VkRenderer* GetRenderer() const
    {
      return mRenderer;
    }

    std::shared_ptr<vkhlf::Surface>& GetSurface()
    {
      return mSurface;
    }

    std::shared_ptr<vkhlf::Device>& GetDevice();

    std::shared_ptr<vkhlf::DeviceMemoryAllocator>& GetAllocator(const std::string aName);

    std::shared_ptr<vkhlf::RenderPass>& GetRenderPass(GraphicsView *aView)
    {
      return GetViewData(aView).mRenderTarget->GetRenderPass();
    }

    std::shared_ptr<vkhlf::Buffer>& GetUBOViewBuffer(GraphicsView *aView)
    {
      return GetViewData(aView).mViewUBO;
    }

    std::shared_ptr<vkhlf::Buffer>& GetUBOIlluminationBuffer(GraphicsView *aView)
    {
      return GetViewData(aView).mIlluminationUBO;
    }

    std::shared_ptr<vkhlf::CommandPool>& GetCommandPool();
    std::shared_ptr<vkhlf::Queue>& GetGraphicsQueue();

    void SetWindow(Window *aWindow)
    {
      mWindow = aWindow;
    }

    void SetRenderer(VkRenderer *aRenderer)
    {
      mRenderer = aRenderer;
    }

    glm::vec4 GetClearColor(GraphicsView *aView)
    {
      return GetViewData(aView).mClearColor;
    }

    void SetClearColor(GraphicsView *aView, glm::vec4 aColor)
    {
      GetViewData(aView).mClearColor = aColor;
    }

    VkLightManager* GetLightManager(GraphicsView *aView)
    {
      return &GetViewData(aView).mLightManager;
    }

    std::vector<VkInstantiatedModel*>& GetInstantiatedModels(GraphicsView *aView, VkMesh *aMesh)
    {
      return GetViewData(aView).mInstantiatedModels[aMesh];
    }

    std::map<GraphicsView*, ViewData>& GetViews()
    {
      return mViewData;
    }

    ViewData& GetViewData(GraphicsView *aView)
    {
      return mViewData[aView];
    }

    vk::Extent2D GetExtent()
    {
      return mRenderToScreen->GetExtent();
    }

    VkRenderTarget* GetRenderTarget(GraphicsView* aView)
    {
      return GetViewData(aView).mRenderTarget.get();
    }

    std::shared_ptr<vkhlf::CommandBuffer> const& GetRenderingCBOB()
    {
      return mRenderingCBOB->GetCurrentCBO();
    }

  private:
    void RenderFrameForSurface();
    std::unique_ptr<VkRenderTarget> CreateRenderTarget(YTEDrawerTypes aDrawerType,
                                                       ViewData *view,
                                                       YTEDrawerTypeCombination aCombination);
    
    Window *mWindow;
    VkRenderer *mRenderer;

    // Vkhlf stuff
    std::shared_ptr<vkhlf::Surface> mSurface;
    //std::shared_ptr<vkhlf::CommandBuffer> mRenderingCommandBuffer;
    vk::Format mColorFormat;
    vk::Format mDepthFormat;

    // Rendering stuff
    std::unique_ptr<VkRenderToScreen> mRenderToScreen;

    // loaded data
    std::map<GraphicsView*, ViewData> mViewData;
    std::unordered_map<std::string, VkCreatePipelineDataSet> mShaderCreateInfos;

    // loaders
    std::unique_ptr<VkCBOB<3, false>> mAnimationUpdateCBOB;
    std::unique_ptr<VkCBOB<3, false>> mGraphicsDataUpdateCBOB;
    std::unique_ptr<VkCBOB<3, false>> mRenderingCBOB;

    // rendering blocks
    //std::shared_ptr<vkhlf::Semaphore> mRenderPass1;
    //std::shared_ptr<vkhlf::Semaphore> mRenderPass2;
    //std::shared_ptr<vkhlf::Semaphore> mRenderPass3;
    std::shared_ptr<vkhlf::Semaphore> mRenderCompleteSemaphore;
    std::shared_ptr<vkhlf::Semaphore> mCubemapComplete;
    // final semaphore is in the swapchain


    // Engine Data
    bool mDataUpdateRequired;
    bool mConstructing;
    bool mCanPresent = false;
  };
}


#endif
