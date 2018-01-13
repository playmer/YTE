///////////////////
// Author: Andrew Griffin
// YTE - Graphics - Vulkan
///////////////////

#pragma once

#ifndef YTE_Graphics_Vulkan_VkRenderer_hpp
#define YTE_Graphics_Vulkan_VkRenderer_hpp

#include "YTE/Graphics/Generics/Renderer.hpp"
#include "YTE/Graphics/Vulkan/ForwardDeclarations.hpp"

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
    std::unique_ptr<InstantiatedLight> CreateLight(GraphicsView *aView) override;
        
    void UpdateWindowViewBuffer(GraphicsView *aView, UBOView &aUBOView) override;
    void UpdateWindowIlluminationBuffer(GraphicsView *aView, UBOIllumination &aIllumination) override;

    Mesh* CreateSimpleMesh(GraphicsView *aView,
                           std::string &aName,
                           std::vector<Submesh> &aSubmeshes) override;


    /////////////////////////////////
    // Events
    /////////////////////////////////
    void GraphicsDataUpdate(LogicUpdate *aEvent) override;
    void FrameUpdate(LogicUpdate *aEvent) override;
    void PresentFrame(LogicUpdate *aEvent) override;
    void AnimationUpdate(LogicUpdate *aEvent) override;

    void SetLights(bool aOnOrOff);
    void RegisterView(GraphicsView *aView) override;
    void DeregisterView(GraphicsView *aView) override;
    void ViewOrderChanged(GraphicsView *aView, float aOldOrder, float aNewOrder) override;

    /////////////////////////////////
    // Getter / Setter
    /////////////////////////////////
    glm::vec4 GetClearColor(GraphicsView *aView);
    void SetClearColor(GraphicsView *aView, const glm::vec4 &aColor) override;
    VkRenderedSurface* GetSurface(Window *aWindow);

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

  private:
    std::unique_ptr<VkInternals> mVulkanInternals;
    std::unordered_map<Window*, std::unique_ptr<VkRenderedSurface>> mSurfaces;
    Engine *mEngine;
  };
}


#endif
