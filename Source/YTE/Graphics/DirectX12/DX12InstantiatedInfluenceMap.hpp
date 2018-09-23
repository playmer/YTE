#pragma once

#ifndef YTE_Graphics_Vulkan_VkInstantiatedInfluenceMap_hpp
#define YTE_Graphics_Vulkan_VkInstantiatedInfluenceMap_hpp

#include "YTE/Graphics/Generics/InstantiatedInfluenceMap.hpp"
#include "YTE/Graphics/ForwardDeclarations.hpp"
#include "YTE/Graphics/UBOs.hpp"
#include "YTE/Graphics/DirectX12/ForwardDeclarations.hpp"
#include "YTE/Graphics/DirectX12/DX12FunctionLoader.hpp"

namespace YTE
{
  class DX12InstantiatedInfluenceMap : public InstantiatedInfluenceMap
  {
  public:
    YTEDeclareType(DX12InstantiatedInfluenceMap);

    DX12InstantiatedInfluenceMap(Dx12RenderedSurface *aSurface, DX12WaterInfluenceMapManager* aMapManager, GraphicsView* aView);
    ~DX12InstantiatedInfluenceMap() override;

    void GraphicsDataUpdate(DX12GraphicsDataUpdate *aEvent);
    void SurfaceLostEvent(ViewChanged *aEvent);
    void SurfaceGainedEvent(ViewChanged *aEvent);

    void SetIndex(unsigned int aIndex)
    {
      mIndex = aIndex;
    }

    GraphicsView *mGraphicsView;
    Dx12RenderedSurface *mSurface;
    DX12WaterInfluenceMapManager *mManager;
    unsigned int mIndex;
  };
}

#endif
