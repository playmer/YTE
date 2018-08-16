///////////////////
// Author: Andrew Griffin
// YTE - Graphics - Vulkan
///////////////////

#pragma once

#ifndef YTE_Graphics_Vulkan_VKWaterInfluenceMapManager_hpp
#define YTE_Graphics_Vulkan_VKWaterInfluenceMapManager_hpp

#include "YTE/Core/EventHandler.hpp"
#include "YTE/Core/Utilities.hpp"

#include "YTE/Graphics/DirectX12/DX12ForwardDeclarations.hpp"
#include "YTE/Graphics/DirectX12/DX12VkFunctionLoader.hpp"
#include "YTE/Graphics/ForwardDeclarations.hpp"
#include "YTE/Graphics/UBOs.hpp"

namespace YTE
{
  class VkWaterInfluenceMapManager : public EventHandler
  {
  public:
    VkWaterInfluenceMapManager();
    VkWaterInfluenceMapManager(Dx12RenderedSurface* aSurface);

    ~VkWaterInfluenceMapManager()
    {
      mBuffer.reset();
      mMaps.clear();
    }

    void SetSurfaceAndView(Dx12RenderedSurface* aSurface, GraphicsView* aView);

    void GraphicsDataUpdateVkEvent(GraphicsDataUpdateVk* aEvent);

    void AddMap(VkInstantiatedInfluenceMap *aMap);
    std::unique_ptr<VkInstantiatedInfluenceMap> CreateMap();
    void DestroyMap(VkInstantiatedInfluenceMap* aMap);

    void UpdateMapValue(unsigned int aIndex, UBOWaterInfluenceMap &aMapValue);

    void SetBaseHeight(float aHeight)
    {
      mWaterInformationData.mBaseHeight = aHeight;
    }

    std::shared_ptr<vkhlf::Buffer> GetUBOMapBuffer()
    {
      return mBuffer;
    }

    UBOWaterInformationMan mWaterInformationData;

  private:
    std::vector<VkInstantiatedInfluenceMap*> mMaps;
    std::shared_ptr<vkhlf::Buffer> mBuffer;
    Dx12RenderedSurface* mSurface;
    GraphicsView* mGraphicsView;
    unsigned char mMapUse[YTE_Graphics_WaterInformationCount];
    bool mUpdateRequired;
  };
}

#endif
