#pragma once

#ifndef YTE_Graphics_Vulkan_VKLightManager_hpp
#define YTE_Graphics_Vulkan_VKLightManager_hpp

#include "YTE/Core/EventHandler.hpp"
#include "YTE/Core/Utilities.hpp"

#include "YTE/Graphics/DirectX12/ForwardDeclarations.hpp"
#include "YTE/Graphics/DirectX12/DX12FunctionLoader.hpp"
#include "YTE/Graphics/ForwardDeclarations.hpp"
#include "YTE/Graphics/UBOs.hpp"

namespace YTE
{
  class DX12LightManager : public EventHandler
  {
  public:
    DX12LightManager();
    DX12LightManager(Dx12RenderedSurface* aSurface);

    ~DX12LightManager()
    {
      mBuffer.reset();
      mLights.clear();
    }

    void SetSurfaceAndView(Dx12RenderedSurface* aSurface, GraphicsView* aView);

    void GraphicsDataUpdateVkEvent(DX12GraphicsDataUpdate* aEvent);

    void AddLight(DX12InstantiatedLight *aLight);
    std::unique_ptr<DX12InstantiatedLight> CreateLight();
    void DestroyLight(DX12InstantiatedLight* aLight);

    void UpdateLightValue(unsigned int aIndex, UBOLight &aLightValue);

    void SetLights(bool aOnOrOff);

    std::shared_ptr<vkhlf::Buffer> GetUBOLightBuffer()
    {
      return mBuffer;
    }

  private:
    std::vector<DX12InstantiatedLight*> mLights;
    UBOLightMan mLightData;
    std::shared_ptr<vkhlf::Buffer> mBuffer;
    Dx12RenderedSurface* mSurface;
    GraphicsView* mGraphicsView;
    unsigned char mLightUse[YTE_Graphics_LightCount];
    bool mUpdateRequired;
  };
}

#endif
