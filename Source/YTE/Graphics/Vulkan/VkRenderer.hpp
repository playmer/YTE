///////////////////
// Author: Andrew Griffin
// YTE - Graphics - Vulkan
///////////////////

#pragma once

#ifndef YTE_Graphics_Vulkan_VkRenderer_hpp
#define YTE_Graphics_Vulkan_VkRenderer_hpp

#include <unordered_map>

#include "YTE/Graphics/Generics/Renderer.hpp"
#include "YTE/Graphics/Vulkan/ForwardDeclarations.hpp"
#include "YTE/Graphics/Vulkan/VkFunctionLoader.hpp"
#include "YTE/Graphics/Vulkan/VkCommandBufferBuffer.hpp"

namespace YTE
{
  class VkRenderer : public Renderer
  {
  public:
    YTEDeclareType(VkRenderer);

    VkRenderer(Engine *aEngine);
    ~VkRenderer() override;

    void DeregisterWindowFromDraw(Window *aWindow) override;
    void RegisterWindowForDraw(Window *aWindow) override;

    std::unique_ptr<InstantiatedModel> CreateModel(GraphicsView *aView, std::string &aMeshFile) override;
    std::unique_ptr<InstantiatedModel> CreateModel(GraphicsView *aView, Mesh *aMesh) override;
    void DestroyMeshAndModel(GraphicsView *aView, InstantiatedModel *aModel) override;
    std::unique_ptr<InstantiatedLight> CreateLight(GraphicsView *aView) override;
    std::unique_ptr<InstantiatedInfluenceMap> CreateWaterInfluenceMap(GraphicsView *aView) override;

    VkTexture* CreateTexture(std::string &aFilename, vk::ImageViewType aType);
        
    void UpdateWindowViewBuffer(GraphicsView *aView, UBOView &aUBOView) override;
    void UpdateWindowIlluminationBuffer(GraphicsView *aView, UBOIllumination &aIllumination) override;

    VkMesh* CreateMesh(std::string &aFilename);
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
    void RegisterView(GraphicsView *aView, YTEDrawerTypes aDrawerType, YTEDrawerTypeCombination aCombination) override;
    void SetViewDrawingType(GraphicsView *aView, YTEDrawerTypes aDrawerType, YTEDrawerTypeCombination aCombination) override;
    void SetViewCombinationType(GraphicsView *aView, YTEDrawerTypeCombination aCombination) override;
    void DeregisterView(GraphicsView *aView) override;
    void ViewOrderChanged(GraphicsView *aView, float aNewOrder) override;

    /////////////////////////////////
    // Getter / Setter
    /////////////////////////////////
    glm::vec4 GetClearColor(GraphicsView *aView);
    void SetClearColor(GraphicsView *aView, const glm::vec4 &aColor) override;
    VkRenderedSurface* GetSurface(Window *aWindow);

    VkWaterInfluenceMapManager* GetAllWaterInfluenceMaps(GraphicsView *aView);

    Engine* GetEngine() const
    {
      return mEngine;
    }

    std::unordered_map<Window*, std::unique_ptr<VkRenderedSurface>>& GetSurfaces()
    {
      return mSurfaces;
    }

    VkInternals* GetVkInternals() const
    {
      return mVulkanInternals.get();
    }

    std::shared_ptr<vkhlf::Device> mDevice;
    std::unordered_map<std::string, std::shared_ptr<vkhlf::DeviceMemoryAllocator>> mAllocators;
    std::unordered_map<std::string, std::unique_ptr<VkTexture>> mTextures;
    std::unordered_map<std::string, std::unique_ptr<VkMesh>> mMeshes;
    std::shared_ptr<vkhlf::Queue> mGraphicsQueue;
    std::shared_ptr<vkhlf::CommandPool> mCommandPool;
  private:
    bool mDataUpdateRequired = false;
    // create a command pool for command buffer allocation
    std::unique_ptr<VkCBOB<3, false>> mGraphicsDataUpdateCBOB;

    std::unique_ptr<VkInternals> mVulkanInternals;
    std::unordered_map<Window*, std::unique_ptr<VkRenderedSurface>> mSurfaces;
    Engine *mEngine;
  };
}


#endif
