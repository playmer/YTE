#include <algorithm>

#include "YTE/Core/Engine.hpp"

#include "YTE/Graphics/Generics/Renderer.hpp"

#include "YTE/Graphics/GraphicsView.hpp"
#include "YTE/Graphics/InstantiatedInfluenceMap.hpp"
#include "YTE/Graphics/WaterInfluenceMapManager.hpp"

namespace YTE
{
  WaterInfluenceMapManager::WaterInfluenceMapManager()
  {
    mWaterInformationData.mNumberOfInfluences = 0;
  }

  WaterInfluenceMapManager::WaterInfluenceMapManager(GraphicsView* aView)
    : mView{ aView }
    , mRenderer{ aView->GetRenderer() }
  {
    mRenderer->RegisterEvent<&WaterInfluenceMapManager::LoadToGPU>(Events::GraphicsDataUpdate, this);
    
    auto allocator = mRenderer->GetAllocator(AllocatorTypes::UniformBufferObject);
    
    mBuffer = allocator->CreateBuffer<UBOs::WaterInformationManager>(1,
                                                                     GPUAllocation::BufferUsage::TransferDst | 
                                                                     GPUAllocation::BufferUsage::UniformBuffer,
                                                                     GPUAllocation::MemoryProperty::DeviceLocal);


    mMaps.reserve(UBOs::WaterInformationCount);

    for (int i = 0; i < UBOs::WaterInformationCount; ++i)
    {
      mMapUse[i] = 0;
    }

    mUpdateRequired = true;
    mWaterInformationData.mNumberOfInfluences = 0;
  }

  void WaterInfluenceMapManager::SetView(GraphicsView* aView)
  {
    mView = aView;
    mRenderer->RegisterEvent<&WaterInfluenceMapManager::LoadToGPU>(Events::GraphicsDataUpdate, this);

    auto allocator = mRenderer->GetAllocator(AllocatorTypes::UniformBufferObject);
    
    mBuffer = allocator->CreateBuffer<UBOs::WaterInformationManager>(1,
                                                                     GPUAllocation::BufferUsage::TransferDst | 
                                                                     GPUAllocation::BufferUsage::UniformBuffer,
                                                                     GPUAllocation::MemoryProperty::DeviceLocal);

    mMaps.reserve(UBOs::WaterInformationCount);

    for (int i = 0; i < UBOs::WaterInformationCount; ++i)
    {
      mMapUse[i] = 0;
    }

    mUpdateRequired = true;
    mWaterInformationData.mNumberOfInfluences = 0;
  }

  void WaterInfluenceMapManager::LoadToGPU(LogicUpdate* aEvent)
  {
    UnusedArguments(aEvent);

    for (auto map : mMaps)
    {
      if (map->mDataChanged)
      {
        mWaterInformationData.mInformation[map->mIndex] = map->mInfluenceMapUBOData;
        mUpdateRequired = true;
      }
    }

    if (mUpdateRequired)
    {
      mBuffer.Update(mWaterInformationData);
      mUpdateRequired = false;
    }
  }

  void WaterInfluenceMapManager::AddMap(InstantiatedInfluenceMap*aMap)
  {
    if (mWaterInformationData.mNumberOfInfluences == UBOs::WaterInformationCount)
    {
      DebugObjection(true, "Water Influence Map Manager is full, no new maps can be added, you may safely continue, no map was added");
    }

    mMaps.push_back(aMap);

    aMap->SetIndex(mWaterInformationData.mNumberOfInfluences);
    mWaterInformationData.mNumberOfInfluences++;
    mUpdateRequired = true;
  }

  std::unique_ptr<InstantiatedInfluenceMap> WaterInfluenceMapManager::CreateMap()
  {
    if (mWaterInformationData.mNumberOfInfluences == UBOs::WaterInformationCount)
    {
      DebugObjection(true , "Water Influence Map Manager is full, no new maps can be added, you may safely continue, no map was added");
      return nullptr;
    }

    auto map = std::make_unique<InstantiatedInfluenceMap>(mView);
    mMaps.push_back(map.get());

    map->SetIndex(mWaterInformationData.mNumberOfInfluences);
    mWaterInformationData.mNumberOfInfluences++;
    mUpdateRequired = true;

    return std::move(map);
  }



  void WaterInfluenceMapManager::DestroyMap(InstantiatedInfluenceMap* aMap)
  {
    int index = aMap->mIndex;

    for (size_t i = index; i < mWaterInformationData.mNumberOfInfluences - 1; ++i)
    {
      mWaterInformationData.mInformation[i] = mWaterInformationData.mInformation[i + 1];
      mMaps[i + 1]->mIndex -= 1;

      std::swap(mMaps[i], mMaps[i + 1]);
    }

    mMaps.pop_back();
    mWaterInformationData.mNumberOfInfluences -= 1;

    mUpdateRequired = true;
  }
}
