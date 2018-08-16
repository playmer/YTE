///////////////////
// Author: Andrew Griffin
// YTE - Graphics - Vulkan
///////////////////

#pragma once

#ifndef YTE_Graphics_Vulkan_VkInstantiatedInfluenceMap_hpp
#define YTE_Graphics_Vulkan_VkInstantiatedInfluenceMap_hpp


#include "YTE/Graphics/Generics/InstantiatedInfluenceMap.hpp"
#include "YTE/Graphics/ForwardDeclarations.hpp"
#include "YTE/Graphics/UBOs.hpp"
#include "YTE/Graphics/DirectX12/DX12ForwardDeclarations.hpp"
#include "YTE/Graphics/DirectX12/DX12VkFunctionLoader.hpp"

namespace YTE
{
  class VkInstantiatedInfluenceMap : public InstantiatedInfluenceMap
  {
  public:
    YTEDeclareType(VkInstantiatedInfluenceMap);

    VkInstantiatedInfluenceMap(Dx12RenderedSurface *aSurface, VkWaterInfluenceMapManager* aMapManager, GraphicsView* aView);
    ~VkInstantiatedInfluenceMap() override;

    void GraphicsDataUpdateVk(GraphicsDataUpdateVk *aEvent);
    void SurfaceLostEvent(ViewChanged *aEvent);
    void SurfaceGainedEvent(ViewChanged *aEvent);

    void SetIndex(unsigned int aIndex)
    {
      mIndex = aIndex;
    }


    Dx12RenderedSurface *mSurface;
    GraphicsView *mGraphicsView;
    VkWaterInfluenceMapManager *mManager;
    unsigned int mIndex;
  };
}

#endif
