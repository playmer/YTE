#pragma once

#ifndef YTE_Graphics_Vulkan_Dx12Renderer_hpp
#define YTE_Graphics_Vulkan_Dx12Renderer_hpp

#include <unordered_map>

#include "YTE/Graphics/Generics/Texture.hpp"
#include "YTE/Graphics/Generics/Renderer.hpp"
#include "YTE/Graphics/DirectX12/ForwardDeclarations.hpp"
#include "YTE/Graphics/DirectX12/DX12CommandBufferBuffer.hpp"

namespace YTE
{
  class Dx12Renderer : public Renderer
  {
  public:
    YTEDeclareType(Dx12Renderer);

    Dx12Renderer(Engine *aEngine);
    ~Dx12Renderer() override;

    void DeregisterWindowFromDraw(Window *aWindow) override;
    void RegisterWindowForDraw(Window *aWindow) override;

    std::unique_ptr<InstantiatedModel> CreateModel(GraphicsView *aView, std::string &aMeshFile) override;
    std::unique_ptr<InstantiatedModel> CreateModel(GraphicsView *aView, Mesh *aMesh) override;
    void DestroyMeshAndModel(GraphicsView *aView, InstantiatedModel *aModel) override;
    std::unique_ptr<InstantiatedLight> CreateLight(GraphicsView *aView) override;
    std::unique_ptr<InstantiatedInfluenceMap> CreateWaterInfluenceMap(GraphicsView *aView) override;

    DX12Texture* CreateTexture(std::string &aFilename/*, vk::ImageViewType aType*/);
    DX12Texture* CreateTexture(std::string aName,
                               std::vector<u8> aData,
                               TextureLayout aType,
                               u32 aWidth,
                               u32 aHeight,
                               u32 aMipLevels,
                               u32 aLayerCount/*,
                               vk::ImageViewType aVulkanType*/);


    Texture* CreateTexture(std::string &aFilename, TextureType aType) override;
    Texture* CreateTexture(std::string aName,
                           std::vector<u8> aData,
                           TextureLayout aLayout,
                           u32 aWidth,
                           u32 aHeight,
                           u32 aMipLevels,
                           u32 aLayerCount,
                           TextureType aType) override;
        
    void UpdateWindowViewBuffer(GraphicsView *aView, UBOView &aUBOView) override;
    void UpdateWindowIlluminationBuffer(GraphicsView *aView, UBOIllumination &aIllumination) override;

    DX12Mesh* CreateMesh(std::string &aFilename);
    Mesh* CreateSimpleMesh(std::string &aName,
                           std::vector<Submesh> &aSubmeshes,
		                       bool aForceUpdate = false) override;

    void ResetView(GraphicsView *aView);


    /////////////////////////////////
    // Events
    /////////////////////////////////
    void GraphicsDataUpdate(LogicUpdate *aEvent) override;
    void FrameUpdate(LogicUpdate *aEvent) override;
    void PresentFrame(LogicUpdate *aEvent) override;
    void AnimationUpdate(LogicUpdate *aEvent) override;

    void SetLights(bool aOnOrOff);
    void RegisterView(GraphicsView *aView) override;
    void RegisterView(GraphicsView *aView, DrawerTypes aDrawerType, DrawerTypeCombination aCombination) override;
    void SetViewDrawingType(GraphicsView *aView, DrawerTypes aDrawerType, DrawerTypeCombination aCombination) override;
    void SetViewCombinationType(GraphicsView *aView, DrawerTypeCombination aCombination) override;
    void DeregisterView(GraphicsView *aView) override;
    void ViewOrderChanged(GraphicsView *aView, float aNewOrder) override;

    /////////////////////////////////
    // Getter / Setter
    /////////////////////////////////
    glm::vec4 GetClearColor(GraphicsView *aView);
    void SetClearColor(GraphicsView *aView, const glm::vec4 &aColor) override;
    Dx12RenderedSurface* GetSurface(Window *aWindow);

    DX12WaterInfluenceMapManager* GetAllWaterInfluenceMaps(GraphicsView *aView);

    Engine* GetEngine() const
    {
      return mEngine;
    }

    std::unordered_map<Window*, std::unique_ptr<Dx12RenderedSurface>>& GetSurfaces()
    {
      return mSurfaces;
    }

    Dx12Internals* GetDx12Internals() const
    {
      return mVulkanInternals.get();
    }

    //std::shared_ptr<vkhlf::Device> mDevice;
    //std::unordered_map<std::string, std::shared_ptr<vkhlf::DeviceMemoryAllocator>> mAllocators;
    //std::unordered_map<std::string, std::unique_ptr<DX12Texture>> mTextures;
    //std::unordered_map<std::string, std::unique_ptr<DX12Mesh>> mMeshes;
    //std::shared_ptr<vkhlf::Queue> mGraphicsQueue;
    //std::shared_ptr<vkhlf::CommandPool> mCommandPool;
  private:
    bool mDataUpdateRequired = false;
    // create a command pool for command buffer allocation
    std::unique_ptr<Dx12CBOB<3, false>> mGraphicsDataUpdateCBOB;

    std::unique_ptr<Dx12Internals> mVulkanInternals;
    std::unordered_map<Window*, std::unique_ptr<Dx12RenderedSurface>> mSurfaces;
    Engine *mEngine;
  };
}


#endif
