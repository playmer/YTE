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

    std::shared_ptr<InstantiatedModel> CreateModel(Window *aWindow, std::string &aMeshFile) override;
    void DestroyModel(Window *aWindow, std::shared_ptr<InstantiatedModel> aModel) override;
    
    void UpdateWindowViewBuffer(Window *aWindow, UBOView &aView) override;



    /////////////////////////////////
    // Events
    /////////////////////////////////
    void GraphicsDataUpdate(LogicUpdate *aEvent) override;
    void FrameUpdate(LogicUpdate *aEvent) override;
    void PresentFrame(LogicUpdate *aEvent) override;



    /////////////////////////////////
    // Getter / Setter
    /////////////////////////////////
    glm::vec4 GetClearColor(Window *aWindow) const;
    std::shared_ptr<VkRenderedSurface>& GetSurface(Window *aWindow);

    Engine* GetEngine() const
    {
      return mEngine;
    }

    std::unordered_map<Window*, std::shared_ptr<VkRenderedSurface>>& GetSurfaces()
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
    std::unordered_map<Window*, std::shared_ptr<VkRenderedSurface>> mSurfaces;
    Engine *mEngine;
  };
}


#endif
