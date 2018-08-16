///////////////////
// Author: Andrew Griffin
// YTE - Graphics - Vulkan
///////////////////

#pragma once

#ifndef YTE_Graphics_Vulkan_Dx12RendererdSurface_hpp
#define YTE_Graphics_Vulkan_Dx12RendererdSurface_hpp

#include "YTE/Core/Utilities.hpp"

#include "YTE/Graphics/GraphicsView.hpp"
#include "YTE/Graphics/UBOs.hpp"
#include "YTE/Graphics/DirectX12/DX12ForwardDeclarations.hpp"
#include "YTE/Graphics/DirectX12/DX12VkFunctionLoader.hpp"
#include "YTE/Graphics/DirectX12/DX12Dx12ShaderDescriptions.hpp"
#include "YTE/Graphics/DirectX12/DX12VkLightManager.hpp"
#include "YTE/Graphics/DirectX12/DX12VkWaterInfluenceMapManager.hpp"
#include "YTE/Graphics/DirectX12/DX12VkRenderToScreen.hpp"
#include "YTE/Graphics/DirectX12/DX12Drawers/VkRenderTarget.hpp"
#include "YTE/Graphics/DirectX12/DX12VkCommandBufferBuffer.hpp"
#include "YTE/Graphics/DirectX12/DX12Dx12Shader.hpp"

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
    ViewData()
    {

    }

    // Buffers
    std::shared_ptr<vkhlf::Buffer> mViewUBO;
    std::shared_ptr<vkhlf::Buffer> mIlluminationUBO;
    std::string mName = "EMPTY";

    // Engine Side Data
    glm::vec4 mClearColor;
    UBOView mViewUBOData;
    UBOIllumination mIlluminationUBOData;
    VkLightManager mLightManager;
    VkWaterInfluenceMapManager mWaterInfluenceMapManager;
    std::unordered_map<VkMesh*, std::vector<VkInstantiatedModel*>> mInstantiatedModels;
    std::unordered_map<std::string, std::unique_ptr<Dx12Shader>> mShaders;
    std::unique_ptr<VkRenderTarget> mRenderTarget;
    GraphicsView *mView;
    float mViewOrder;
  };

  class Dx12RenderedSurface : public EventHandler
  {
  public:
    YTEDeclareType(Dx12RenderedSurface);

    Dx12RenderedSurface(Window *aWindow,
                      Dx12Renderer *aRenderer,
                      std::shared_ptr<vkhlf::Surface> &aSurface);

    ~Dx12RenderedSurface();

    void UpdateSurfaceIlluminationBuffer(GraphicsView* aView, UBOIllumination &aIllumination);
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
    

    Dx12Shader* CreateShader(std::string &aShaderSetName,
                           std::shared_ptr<vkhlf::PipelineLayout> &aPipelineLayout,
                           Dx12ShaderDescriptions &aDescription, 
                           GraphicsView* aView);

    std::unique_ptr<VkInstantiatedLight> CreateLight(GraphicsView *aView);
    std::unique_ptr<VkInstantiatedInfluenceMap> CreateWaterInfluenceMap(GraphicsView *aView);


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
    void RegisterView(GraphicsView *aView, DrawerTypes aDrawerType, DrawerTypeCombination aCombination);
    void SetViewDrawingType(GraphicsView *aView, DrawerTypes aDrawerType, DrawerTypeCombination aCombination);
    void SetViewCombinationType(GraphicsView *aView, DrawerTypeCombination aCombination);
    void DeregisterView(GraphicsView *aView);
    void ViewOrderChanged(GraphicsView *aView, float aNewOrder);

    /////////////////////////////////
    // Getters / Setters
    /////////////////////////////////
    Window* GetWindow() const
    {
      return mWindow;
    }

    Dx12Renderer* GetRenderer() const
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
      return GetViewData(aView)->mRenderTarget->GetRenderPass();
    }

    std::shared_ptr<vkhlf::Buffer>& GetUBOViewBuffer(GraphicsView *aView)
    {
      return GetViewData(aView)->mViewUBO;
    }

    std::shared_ptr<vkhlf::Buffer>& GetUBOIlluminationBuffer(GraphicsView *aView)
    {
      return GetViewData(aView)->mIlluminationUBO;
    }

    std::shared_ptr<vkhlf::CommandPool>& GetCommandPool();
    std::shared_ptr<vkhlf::Queue>& GetGraphicsQueue();

    void SetWindow(Window *aWindow)
    {
      mWindow = aWindow;
    }

    void SetRenderer(Dx12Renderer *aRenderer)
    {
      mRenderer = aRenderer;
    }

    glm::vec4 GetClearColor(GraphicsView *aView)
    {
      return GetViewData(aView)->mClearColor;
    }

    void SetClearColor(GraphicsView *aView, glm::vec4 aColor)
    {
      GetViewData(aView)->mClearColor = aColor;
    }

    VkLightManager* GetLightManager(GraphicsView *aView)
    {
      return &GetViewData(aView)->mLightManager;
    }

    VkWaterInfluenceMapManager* GetWaterInfluenceMapManager(GraphicsView *aView)
    {
      return &GetViewData(aView)->mWaterInfluenceMapManager;
    }

    std::vector<VkInstantiatedModel*>& GetInstantiatedModels(GraphicsView *aView, VkMesh *aMesh)
    {
      return GetViewData(aView)->mInstantiatedModels[aMesh];
    }

    std::map<GraphicsView*, ViewData>& GetViews()
    {
      return mViewData;
    }

    ViewData* GetViewData(GraphicsView *aView)
    {
      auto it = mViewData.find(aView);

      if (it != mViewData.end())
      {
        return &(it->second);
      }

      return nullptr;
    }

    vk::Extent2D GetExtent()
    {
      return mRenderToScreen->GetExtent();
    }

    VkRenderTarget* GetRenderTarget(GraphicsView* aView)
    {
      return GetViewData(aView)->mRenderTarget.get();
    }

    std::shared_ptr<vkhlf::CommandBuffer> const& GetRenderingCBOB()
    {
      return mRenderingCBOB->GetCurrentCBO();
    }

  private:
    void ResizeInternal(bool aConstructing = false);


    void RenderFrameForSurface();
    std::unique_ptr<VkRenderTarget> CreateRenderTarget(DrawerTypes aDrawerType,
                                                       ViewData *view,
                                                       DrawerTypeCombination aCombination);
    
    Window *mWindow;
    Dx12Renderer *mRenderer;

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
    std::unique_ptr<Dx12CBOB<3, false>> mAnimationUpdateCBOB;
    std::unique_ptr<Dx12CBOB<3, false>> mGraphicsDataUpdateCBOB;
    std::unique_ptr<Dx12CBOB<3, false>> mRenderingCBOB;

    // rendering blocks
    //std::shared_ptr<vkhlf::Semaphore> mRenderPass1;
    //std::shared_ptr<vkhlf::Semaphore> mRenderPass2;
    //std::shared_ptr<vkhlf::Semaphore> mRenderPass3;
    std::shared_ptr<vkhlf::Semaphore> mRenderCompleteSemaphore;
    std::shared_ptr<vkhlf::Semaphore> mCubemapComplete;
    // final semaphore is in the swapchain


    // Engine Data
    bool mDataUpdateRequired;
    bool mCanPresent = false;
  };
}


#endif
