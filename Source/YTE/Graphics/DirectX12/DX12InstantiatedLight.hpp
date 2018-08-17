#pragma once

#ifndef YTE_Graphics_Vulkan_VkInstantiatedLight_hpp
#define YTE_Graphics_Vulkan_VkInstantiatedLight_hpp


#include "YTE/Graphics/Generics/InstantiatedLight.hpp"
#include "YTE/Graphics/ForwardDeclarations.hpp"
#include "YTE/Graphics/UBOs.hpp"
#include "YTE/Graphics/DirectX12/DX12ForwardDeclarations.hpp"
#include "YTE/Graphics/DirectX12/DX12FunctionLoader.hpp"

namespace YTE
{
  class DX12InstantiatedLight : public InstantiatedLight
  {
  public:
    YTEDeclareType(DX12InstantiatedLight);

    DX12InstantiatedLight(Dx12RenderedSurface *aSurface, DX12LightManager* aLightManager, GraphicsView* aView);
    ~DX12InstantiatedLight() override;

    void DX12GraphicsDataUpdate(DX12GraphicsDataUpdate *aEvent);
    void SurfaceLostEvent(DX12ViewChanged *aEvent);
    void SurfaceGainedEvent(DX12ViewChanged *aEvent);

    void SetIndex(unsigned int aIndex)
    {
      mIndex = aIndex;
    }


    Dx12RenderedSurface *mSurface;
    GraphicsView *mGraphicsView;
    DX12LightManager *mManager;
    unsigned int mIndex;
  };
}

#endif
