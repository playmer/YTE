///////////////////
// Author: Andrew Griffin
// YTE - Graphics - Vulkan
///////////////////

#pragma once

#ifndef YTE_Graphics_Vulkan_VKLightManager_hpp
#define YTE_Graphics_Vulkan_VKLightManager_hpp

#include "YTE/Core/EventHandler.hpp"
#include "YTE/Core/Utilities.hpp"

#include "YTE/Graphics/DirectX12/DX12ForwardDeclarations.hpp"
#include "YTE/Graphics/DirectX12/DX12VkFunctionLoader.hpp"
#include "YTE/Graphics/ForwardDeclarations.hpp"
#include "YTE/Graphics/UBOs.hpp"

namespace YTE
{
  class VkLightManager : public EventHandler
  {
  public:
    VkLightManager();
    VkLightManager(Dx12RenderedSurface* aSurface);

    ~VkLightManager()
    {
      mBuffer.reset();
      mLights.clear();
    }

    void SetSurfaceAndView(Dx12RenderedSurface* aSurface, GraphicsView* aView);

    void GraphicsDataUpdateVkEvent(GraphicsDataUpdateVk* aEvent);

    void AddLight(VkInstantiatedLight *aLight);
    std::unique_ptr<VkInstantiatedLight> CreateLight();
    void DestroyLight(VkInstantiatedLight* aLight);

    void UpdateLightValue(unsigned int aIndex, UBOLight &aLightValue);

    void SetLights(bool aOnOrOff);

    std::shared_ptr<vkhlf::Buffer> GetUBOLightBuffer()
    {
      return mBuffer;
    }

  private:
    std::vector<VkInstantiatedLight*> mLights;
    UBOLightMan mLightData;
    std::shared_ptr<vkhlf::Buffer> mBuffer;
    Dx12RenderedSurface* mSurface;
    GraphicsView* mGraphicsView;
    unsigned char mLightUse[YTE_Graphics_LightCount];
    bool mUpdateRequired;
  };
}

#endif
