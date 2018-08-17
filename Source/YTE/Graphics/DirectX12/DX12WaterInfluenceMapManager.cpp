#include "YTE/Graphics/DirectX12/DX12WaterInfluenceMapManager.hpp"
#include "YTE/Graphics/DirectX12/DX12RenderedSurface.hpp"
#include "YTE/Graphics/DirectX12/DX12DeviceInfo.hpp"
#include "YTE/Graphics/DirectX12/DX12InstantiatedInfluenceMap.hpp"

namespace YTE
{
  DX12WaterInfluenceMapManager::DX12WaterInfluenceMapManager()
  {
    mWaterInformationData.mNumberOfInfluences = 0;
  }

  DX12WaterInfluenceMapManager::DX12WaterInfluenceMapManager(Dx12RenderedSurface* aSurface) : mSurface(aSurface)
  {
    mSurface->RegisterEvent<&DX12WaterInfluenceMapManager::GraphicsDataUpdateVkEvent>(Events::DX12GraphicsDataUpdate, this);

    auto allocator = mSurface->GetAllocator(AllocatorTypes::UniformBufferObject);

    mBuffer = mSurface->GetDevice()->createBuffer(sizeof(UBOWaterInformationMan),
                                                  vk::BufferUsageFlagBits::eTransferDst |
                                                  vk::BufferUsageFlagBits::eUniformBuffer,
                                                  vk::SharingMode::eExclusive,
                                                  nullptr,
                                                  vk::MemoryPropertyFlagBits::eDeviceLocal,
                                                  allocator);

    mMaps.reserve(YTE_Graphics_WaterInformationCount);

    for (int i = 0; i < YTE_Graphics_WaterInformationCount; ++i)
    {
      mMapUse[i] = 0;
    }

    mUpdateRequired = true;
    mWaterInformationData.mNumberOfInfluences = 0;
  }

  void DX12WaterInfluenceMapManager::SetSurfaceAndView(Dx12RenderedSurface* aSurface, GraphicsView* aView)
  {
    mSurface = aSurface;
    mGraphicsView = aView;
    mSurface->RegisterEvent<&DX12WaterInfluenceMapManager::GraphicsDataUpdateVkEvent>(Events::DX12GraphicsDataUpdate, this);

    auto allocator = mSurface->GetAllocator(AllocatorTypes::UniformBufferObject);

    mBuffer = mSurface->GetDevice()->createBuffer(sizeof(UBOWaterInformationMan),
                                                  vk::BufferUsageFlagBits::eTransferDst |
                                                  vk::BufferUsageFlagBits::eUniformBuffer,
                                                  vk::SharingMode::eExclusive,
                                                  nullptr,
                                                  vk::MemoryPropertyFlagBits::eDeviceLocal,
                                                  allocator);

    mMaps.reserve(YTE_Graphics_WaterInformationCount);

    for (int i = 0; i < YTE_Graphics_WaterInformationCount; ++i)
    {
      mMapUse[i] = 0;
    }

    mUpdateRequired = true;
    mWaterInformationData.mNumberOfInfluences = 0;
  }

  void DX12WaterInfluenceMapManager::GraphicsDataUpdateVkEvent(DX12GraphicsDataUpdate* aEvent)
  {
    SendEvent(Events::DX12GraphicsDataUpdate, aEvent);

    if (mUpdateRequired)
    {
      auto update = aEvent->mCBO;
      mBuffer->update<UBOWaterInformationMan>(0, mWaterInformationData, update);
      mUpdateRequired = false;
    }
  }

  void DX12WaterInfluenceMapManager::AddMap(DX12InstantiatedInfluenceMap *aMap)
  {
    if (mWaterInformationData.mNumberOfInfluences == YTE_Graphics_WaterInformationCount)
    {
      DebugObjection(true, "Water Influence Map Manager is full, no new maps can be added, you may safely continue, no map was added");
    }

    mMaps.push_back(aMap);

    aMap->SetIndex(mWaterInformationData.mNumberOfInfluences);
    mWaterInformationData.mNumberOfInfluences++;
    mUpdateRequired = true;
  }

  std::unique_ptr<DX12InstantiatedInfluenceMap> DX12WaterInfluenceMapManager::CreateMap()
  {
    if (mWaterInformationData.mNumberOfInfluences == YTE_Graphics_WaterInformationCount)
    {
      DebugObjection(true , "Water Influence Map Manager is full, no new maps can be added, you may safely continue, no map was added");
      return nullptr;
    }

    auto map = std::make_unique<DX12InstantiatedInfluenceMap>(mSurface, this, mGraphicsView);
    mMaps.push_back(map.get());

    map->SetIndex(mWaterInformationData.mNumberOfInfluences);
    mWaterInformationData.mNumberOfInfluences++;
    mUpdateRequired = true;

    return std::move(map);
  }



  void DX12WaterInfluenceMapManager::DestroyMap(DX12InstantiatedInfluenceMap* aMap)
  {
    int index = aMap->mIndex;

    for (size_t i = index; i < mWaterInformationData.mNumberOfInfluences - 1; ++i)
    {
      mWaterInformationData.mInformation[i] = mWaterInformationData.mInformation[i + 1];
      mMaps[i + 1]->mIndex -= 1;
      DX12InstantiatedInfluenceMap* temp = mMaps[i];
      mMaps[i] = mMaps[i + 1];
      mMaps[i + 1] = temp;
    }

    mMaps.pop_back();
    mWaterInformationData.mNumberOfInfluences -= 1;

    mUpdateRequired = true;
  }



  void DX12WaterInfluenceMapManager::UpdateMapValue(unsigned aIndex, UBOWaterInfluenceMap& aMapValue)
  {
//#ifdef _DEBUG
//    if (aIndex > mWaterInformationData.mNumberOfInfluences || aIndex < 0)
//    {
//      DebugObjection(true, "Water Influence Map Manager cannot access a value at the index of %d. Safe to Continue", aIndex);
//      return;
//    }
//#endif

    mWaterInformationData.mInformation[aIndex] = aMapValue;
    mUpdateRequired = true;
  }
}
