///////////////////
// Author: Andrew Griffin
// YTE - Graphics - Vulkan
///////////////////

#pragma once

#ifndef YTE_Graphics_Vulkan_VkInstantiatedLight_hpp
#define YTE_Graphics_Vulkan_VkInstantiatedLight_hpp


#include "YTE/Graphics/Generics/InstantiatedLight.hpp"
#include "YTE/Graphics/ForwardDeclarations.hpp"
#include "YTE/Graphics/UBOs.hpp"
#include "YTE/Graphics/DirectX12/DX12ForwardDeclarations.hpp"
#include "YTE/Graphics/DirectX12/DX12VkFunctionLoader.hpp"

namespace YTE
{
  class VkInstantiatedLight : public InstantiatedLight
  {
  public:
    YTEDeclareType(VkInstantiatedLight);

    VkInstantiatedLight(Dx12RenderedSurface *aSurface, VkLightManager* aLightManager, GraphicsView* aView);
    ~VkInstantiatedLight() override;

    void GraphicsDataUpdateVk(GraphicsDataUpdateVk *aEvent);
    void SurfaceLostEvent(ViewChanged *aEvent);
    void SurfaceGainedEvent(ViewChanged *aEvent);

    void SetIndex(unsigned int aIndex)
    {
      mIndex = aIndex;
    }


    Dx12RenderedSurface *mSurface;
    GraphicsView *mGraphicsView;
    VkLightManager *mManager;
    unsigned int mIndex;
  };
}

#endif
