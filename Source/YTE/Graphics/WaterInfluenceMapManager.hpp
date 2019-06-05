#pragma once

#ifndef YTE_Graphics_Vulkan_VKWaterInfluenceMapManager_hpp
#define YTE_Graphics_Vulkan_VKWaterInfluenceMapManager_hpp

#include "YTE/Core/EventHandler.hpp"
#include "YTE/Core/Utilities.hpp"

#include "YTE/Graphics/ForwardDeclarations.hpp"
#include "YTE/Graphics/GPUBuffer.hpp"
#include "YTE/Graphics/UBOs.hpp"

namespace YTE
{
  class WaterInfluenceMapManager : public EventHandler
  {
  public:
    WaterInfluenceMapManager();
    WaterInfluenceMapManager(GraphicsView* aView);

    ~WaterInfluenceMapManager()
    {
      mBuffer.reset();
      mMaps.clear();
    }

    void SetView(GraphicsView* aView);

    void LoadToGPU(LogicUpdate* aEvent);

    void AddMap(InstantiatedInfluenceMap* aMap);
    std::unique_ptr<InstantiatedInfluenceMap> CreateMap();
    void DestroyMap(InstantiatedInfluenceMap* aMap);

    void UpdateMapValue(unsigned int aIndex, UBOs::WaterInfluenceMap &aMapValue);

    void SetBaseHeight(float aHeight)
    {
      mWaterInformationData.mBaseHeight = aHeight;
    }

    GPUBuffer<UBOs::WaterInformationManager>& GetUBOMapBuffer()
    {
      return mBuffer;
    }

    UBOs::WaterInformationManager mWaterInformationData;

  private:
    std::vector<InstantiatedInfluenceMap*> mMaps;
    GPUBuffer<UBOs::WaterInformationManager> mBuffer;
    GraphicsView* mView;
    Renderer* mRenderer;
    unsigned char mMapUse[UBOs::WaterInformationCount];
    bool mUpdateRequired;
  };
}

#endif
