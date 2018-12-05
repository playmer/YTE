///////////////////
// Author: Andrew Griffin
// YTE - Graphics - Vulkan
///////////////////

#pragma once

#ifndef YTE_Graphics_Vulkan_VKLightManager_hpp
#define YTE_Graphics_Vulkan_VKLightManager_hpp

#include "YTE/Core/EventHandler.hpp"
#include "YTE/Core/Utilities.hpp"

#include "YTE/Graphics/Vulkan/ForwardDeclarations.hpp"
#include "YTE/Graphics/Vulkan/VkFunctionLoader.hpp"
#include "YTE/Graphics/ForwardDeclarations.hpp"
#include "YTE/Graphics/UBOs.hpp"

namespace YTE
{
  class VkLightManager : public EventHandler
  {
  public:
    VkLightManager();
    VkLightManager(VkRenderedSurface* aSurface);

    ~VkLightManager()
    {
      mBuffer.reset();
      mLights.clear();
    }

    void SetSurfaceAndView(VkRenderedSurface* aSurface, GraphicsView* aView);

    void LoadToVulkan(VkGraphicsDataUpdate* aEvent);

    void AddLight(VkInstantiatedLight *aLight);
    std::unique_ptr<VkInstantiatedLight> CreateLight();
    void DestroyLight(VkInstantiatedLight* aLight);

    void SetLights(bool aOnOrOff);

    std::shared_ptr<vkhlf::Buffer> GetUBOLightBuffer()
    {
      return mBuffer;
    }

  private:
    std::vector<VkInstantiatedLight*> mLights;
    UBOs::LightManager mLightData;
    std::shared_ptr<vkhlf::Buffer> mBuffer;
    VkRenderedSurface* mSurface;
    GraphicsView* mGraphicsView;
    unsigned char mLightUse[YTE_Graphics_LightCount];
    bool mUpdateRequired;
  };
}

#endif
