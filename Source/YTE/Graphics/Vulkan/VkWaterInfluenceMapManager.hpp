///////////////////
// Author: Andrew Griffin
// YTE - Graphics - Vulkan
///////////////////

#pragma once

#ifndef YTE_Graphics_Vulkan_VKWaterInfluenceMapManager_hpp
#define YTE_Graphics_Vulkan_VKWaterInfluenceMapManager_hpp

#include "YTE/Core/EventHandler.hpp"
#include "YTE/Core/Utilities.hpp"

#include "YTE/Graphics/Vulkan/ForwardDeclarations.hpp"
#include "YTE/Graphics/Vulkan/VkFunctionLoader.hpp"
#include "YTE/Graphics/ForwardDeclarations.hpp"
#include "YTE/Graphics/UBOs.hpp"

namespace YTE
{
  class VkWaterInfluenceMapManager : public EventHandler
  {
  public:
    VkWaterInfluenceMapManager();
    VkWaterInfluenceMapManager(VkRenderedSurface* aSurface);

    ~VkWaterInfluenceMapManager()
    {
      mBuffer.reset();
      mMaps.clear();
    }

    void SetSurfaceAndView(VkRenderedSurface* aSurface, GraphicsView* aView);

    void GraphicsDataUpdateVkEvent(VkGraphicsDataUpdate* aEvent);

    void AddMap(VkInstantiatedInfluenceMap *aMap);
    std::unique_ptr<VkInstantiatedInfluenceMap> CreateMap();
    void DestroyMap(VkInstantiatedInfluenceMap* aMap);

    void UpdateMapValue(unsigned int aIndex, UBOs::WaterInfluenceMap &aMapValue);

    void SetBaseHeight(float aHeight)
    {
      mWaterInformationData.mBaseHeight = aHeight;
    }

    std::shared_ptr<vkhlf::Buffer> GetUBOMapBuffer()
    {
      return mBuffer;
    }

    UBOs::WaterInformationManager mWaterInformationData;

  private:
    std::vector<VkInstantiatedInfluenceMap*> mMaps;
    std::shared_ptr<vkhlf::Buffer> mBuffer;
    VkRenderedSurface* mSurface;
    GraphicsView* mGraphicsView;
    unsigned char mMapUse[YTE_Graphics_WaterInformationCount];
    bool mUpdateRequired;
  };
}

#endif
