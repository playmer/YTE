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

namespace YTE
{
  YTEDeclareEvent(GraphicsDataUpdateVk);
  YTEDeclareEvent(AnimationUpdateVk);



  // vulkan specific graphics data update event so that it can contain the CBO object
  class GraphicsDataUpdateVk : public Event
  {
  public:
    YTEDeclareType(GraphicsDataUpdateVk);
    std::shared_ptr<vkhlf::CommandBuffer> mCBO;
  };
  
  class VkRenderedSurface : public EventHandler
  {
  public:
    YTEDeclareType(VkRenderedSurface);

    VkRenderedSurface(Window *aWindow,
                      VkRenderer *aRenderer,
                      std::shared_ptr<vkhlf::Surface> &aSurface);

    ~VkRenderedSurface();

    void UpdateSurfaceViewBuffer(UBOView &aView);
    void PrintSurfaceFormats(std::vector<vk::SurfaceFormatKHR> &aFormats);


    //template<typename tType, typename tMap, typename ...tArguments>
    //static tType* MakeThing(std::string &aName, tMap aContainer, tArguments aArguments...)
    //{
    //  auto it = aContainer.find(aName);
    //  tType *ptr{ nullptr };
    //
    //  if (it == aContainer.end())
    //  {
    //    auto value = std::make_unique<tType>();
    //
    //    ptr = value.get();
    //
    //    aContainer[aName] = std::move(value);
    //    mDataUpdateRequired = true;
    //  }
    //  else
    //  {
    //    ptr = shaderIt->second.get();
    //  }
    //
    //  return ptr;
    //
    //}


    /////////////////////////////////
    // Creation / Destruction
    /////////////////////////////////
    void CreateSpritePipeline();

    std::unique_ptr<InstantiatedSprite> CreateSprite(std::string &aTextureFile);
    void DestroySprite(std::unique_ptr<VkInstantiatedSprite> aSprite);

    std::unique_ptr<VkInstantiatedModel> CreateModel(std::string &aModelFile);
    std::unique_ptr<VkInstantiatedModel> CreateModel(Mesh *aMesh);
    void DestroyModel(VkInstantiatedModel *aModel);

    VkMesh* CreateMesh(std::string &aFilename);

    Mesh* CreateSimpleMesh(std::string &aName,
                           std::vector<Submesh> &aSubmeshes);

    VkTexture* CreateTexture(std::string &aFilename, vk::ImageViewType aType);

    VkShader* CreateShader(std::string &aShaderSetName,
                           std::shared_ptr<vkhlf::PipelineLayout> &aPipelineLayout,
                           VkShaderDescriptions &aDescription,
                           bool aCullBackFaces,
                           std::string &aDefines);


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

    glm::vec4 GetClearColor() const
    {
      return mClearColor;
    }

    std::shared_ptr<vkhlf::Device>& GetDevice()
    {
      return mDevice;
    }

    std::shared_ptr<vkhlf::DeviceMemoryAllocator>& GetAllocator(const std::string aName)
    {
      return mAllocators[aName];
    }

    std::shared_ptr<vkhlf::RenderPass>& GetRenderPass()
    {
      return mRenderPass;
    }

    std::shared_ptr<vkhlf::Buffer>& GetUBOViewBuffer()
    {
      return mViewUBO;
    }

    std::shared_ptr<vkhlf::CommandPool>& GetCommandPool()
    {
      return mCommandPool;
    }

    std::shared_ptr<vkhlf::Queue>& GetGraphicsQueue()
    {
      return mGraphicsQueue;
    }

    void SetWindow(Window *aWindow)
    {
      mWindow = aWindow;
    }

    void SetRenderer(VkRenderer *aRenderer)
    {
      mRenderer = aRenderer;
    }

    void SetClearColor(glm::vec4 aColor)
    {
      mClearColor = aColor;
    }

    std::vector<VkInstantiatedModel*>& GetInstantiatedModels(VkMesh *aMesh)
    {
      return mInstantiatedModels[aMesh];
    }

  private:
    void RenderFrameForSurface();
    
    Window *mWindow;
    VkRenderer *mRenderer;

    // Vkhlf stuff
    std::unordered_map<std::string, std::shared_ptr<vkhlf::DeviceMemoryAllocator>> mAllocators;
    std::unique_ptr<vkhlf::FramebufferSwapchain> mFrameBufferSwapChain;
    std::shared_ptr<vkhlf::Surface> mSurface;
    std::shared_ptr<vkhlf::Queue> mGraphicsQueue;
    std::shared_ptr<vkhlf::RenderPass> mRenderPass;
    std::shared_ptr<vkhlf::Semaphore> mRenderCompleteSemaphore;
    std::shared_ptr<vkhlf::CommandPool> mCommandPool;
    std::shared_ptr<vkhlf::CommandBuffer> mRenderingCommandBuffer;
    std::shared_ptr<vkhlf::Device> mDevice;
    vk::Format mColorFormat;
    vk::Format mDepthFormat;

    // Buffers
    std::shared_ptr<vkhlf::Buffer> mViewUBO;

    // loaded data
    std::unordered_map<std::string, std::unique_ptr<VkTexture>> mTextures;
    std::unordered_map<std::string, std::unique_ptr<VkMesh>> mMeshes;
    std::unordered_map<std::string, std::unique_ptr<VkShader>> mShaders;
    std::unordered_map<VkMesh*, std::vector<VkInstantiatedModel*>> mInstantiatedModels;

    // Engine Data
    glm::vec4 mClearColor;
    UBOView mViewUBOData;
    bool mDataUpdateRequired;
  };
}


#endif
