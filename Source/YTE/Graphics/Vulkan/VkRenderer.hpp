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

    void RegisterWindowForDraw(Window *aWindow) override;
    void DeregisterWindowFromDraw(Window *aWindow) override;

    std::unique_ptr<InstantiatedSprite> CreateSprite(Window *aWindow, std::string &aTextureFile) override;
    void DestroySprite(Window *aWindow, std::unique_ptr<InstantiatedSprite> aSprite) override;

    std::unique_ptr<InstantiatedModel> CreateModel(Window *aWindow, std::string &aMeshFile) override;
    std::unique_ptr<InstantiatedModel> CreateModel(Window *aWindow, Mesh *aMesh) override;
        
    void UpdateWindowViewBuffer(Window *aWindow, UBOView &aView) override;

    Mesh* CreateSimpleMesh(Window *aWindow, 
                           std::string &aName,
                           std::vector<Submesh> &aSubmeshes) override;


    /////////////////////////////////
    // Events
    /////////////////////////////////
    void GraphicsDataUpdate(LogicUpdate *aEvent) override;
    void FrameUpdate(LogicUpdate *aEvent) override;
    void PresentFrame(LogicUpdate *aEvent) override;
    void AnimationUpdate(LogicUpdate *aEvent) override;

    /////////////////////////////////
    // Getter / Setter
    /////////////////////////////////
    glm::vec4 GetClearColor(Window *aWindow);
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



    void SetClearColor(Window *aWindow, const glm::vec4 &aColor) override;



  private:
    std::unique_ptr<VkInternals> mVulkanInternals;
    std::unordered_map<Window*, std::unique_ptr<VkRenderedSurface>> mSurfaces;
    Engine *mEngine;
  };
}


#endif
