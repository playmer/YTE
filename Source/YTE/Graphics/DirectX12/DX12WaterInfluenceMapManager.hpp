#pragma once

#ifndef YTE_Graphics_Vulkan_VKWaterInfluenceMapManager_hpp
#define YTE_Graphics_Vulkan_VKWaterInfluenceMapManager_hpp

#include "YTE/Core/EventHandler.hpp"
#include "YTE/Core/Utilities.hpp"

#include "YTE/Graphics/DirectX12/ForwardDeclarations.hpp"
#include "YTE/Graphics/DirectX12/DX12FunctionLoader.hpp"
#include "YTE/Graphics/ForwardDeclarations.hpp"
#include "YTE/Graphics/UBOs.hpp"

namespace YTE
{
  class DX12WaterInfluenceMapManager : public EventHandler
  {
  public:
    DX12WaterInfluenceMapManager();
    DX12WaterInfluenceMapManager(Dx12RenderedSurface* aSurface);

    ~DX12WaterInfluenceMapManager()
    {
      //mBuffer.reset();
      mMaps.clear();
    }

    void SetSurfaceAndView(Dx12RenderedSurface* aSurface, GraphicsView* aView);

    void GraphicsDataUpdateEvent(DX12GraphicsDataUpdate* aEvent);

    void AddMap(DX12InstantiatedInfluenceMap *aMap);
    std::unique_ptr<DX12InstantiatedInfluenceMap> CreateMap();
    void DestroyMap(DX12InstantiatedInfluenceMap* aMap);

    void UpdateMapValue(unsigned int aIndex, UBOWaterInfluenceMap &aMapValue);

    void SetBaseHeight(float aHeight)
    {
      mWaterInformationData.mBaseHeight = aHeight;
    }

    //std::shared_ptr<vkhlf::Buffer> GetUBOMapBuffer()
    //{
    //  return mBuffer;
    //}

    UBOWaterInformationMan mWaterInformationData;

  private:
    std::vector<DX12InstantiatedInfluenceMap*> mMaps;
    //std::shared_ptr<vkhlf::Buffer> mBuffer;
    Dx12RenderedSurface* mSurface;
    GraphicsView* mGraphicsView;
    unsigned char mMapUse[YTE_Graphics_WaterInformationCount];
    bool mUpdateRequired;
  };
}

#endif
