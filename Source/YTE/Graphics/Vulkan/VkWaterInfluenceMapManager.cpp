/////////////////
// Author: Andrew Griffin
// YTE - Graphics - Vulkan
///////////////////

#include "YTE/Graphics/Vulkan/VkWaterInfluenceMapManager.hpp"
#include "YTE/Graphics/Vulkan/VkRenderedSurface.hpp"
#include "YTE/Graphics/Vulkan/VkRenderer.hpp"
#include "YTE/Graphics/Vulkan/VkDeviceInfo.hpp"
#include "YTE/Graphics/Vulkan/VkInstantiatedInfluenceMap.hpp"

namespace YTE
{
  VkWaterInfluenceMapManager::VkWaterInfluenceMapManager()
  {
    mWaterInformationData.mNumberOfInfluences = 0;
  }

  VkWaterInfluenceMapManager::VkWaterInfluenceMapManager(VkRenderedSurface* aSurface) : mSurface(aSurface)
  {
    mSurface->RegisterEvent<&VkWaterInfluenceMapManager::GraphicsDataUpdateVkEvent>(Events::VkGraphicsDataUpdate, this);
    
    auto allocator = mSurface->GetRenderer()->GetAllocator(AllocatorTypes::UniformBufferObject);
    
    mBuffer = allocator->CreateBuffer<UBOs::WaterInformationManager>(1,
                                                                     GPUAllocation::BufferUsage::TransferDst | 
                                                                     GPUAllocation::BufferUsage::UniformBuffer,
                                                                     GPUAllocation::MemoryProperty::DeviceLocal);


    mMaps.reserve(YTE_Graphics_WaterInformationCount);

    for (int i = 0; i < YTE_Graphics_WaterInformationCount; ++i)
    {
      mMapUse[i] = 0;
    }

    mUpdateRequired = true;
    mWaterInformationData.mNumberOfInfluences = 0;
  }

  void VkWaterInfluenceMapManager::SetSurfaceAndView(VkRenderedSurface* aSurface, GraphicsView* aView)
  {
    mSurface = aSurface;
    mGraphicsView = aView;
    mSurface->RegisterEvent<&VkWaterInfluenceMapManager::GraphicsDataUpdateVkEvent>(Events::VkGraphicsDataUpdate, this);

    auto allocator = mSurface->GetRenderer()->GetAllocator(AllocatorTypes::UniformBufferObject);
    
    mBuffer = allocator->CreateBuffer<UBOs::WaterInformationManager>(1,
                                                                     GPUAllocation::BufferUsage::TransferDst | 
                                                                     GPUAllocation::BufferUsage::UniformBuffer,
                                                                     GPUAllocation::MemoryProperty::DeviceLocal);

    mMaps.reserve(YTE_Graphics_WaterInformationCount);

    for (int i = 0; i < YTE_Graphics_WaterInformationCount; ++i)
    {
      mMapUse[i] = 0;
    }

    mUpdateRequired = true;
    mWaterInformationData.mNumberOfInfluences = 0;
  }

  void VkWaterInfluenceMapManager::GraphicsDataUpdateVkEvent(VkGraphicsDataUpdate* aEvent)
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

  void VkWaterInfluenceMapManager::AddMap(VkInstantiatedInfluenceMap *aMap)
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

  std::unique_ptr<VkInstantiatedInfluenceMap> VkWaterInfluenceMapManager::CreateMap()
  {
    if (mWaterInformationData.mNumberOfInfluences == YTE_Graphics_WaterInformationCount)
    {
      DebugObjection(true , "Water Influence Map Manager is full, no new maps can be added, you may safely continue, no map was added");
      return nullptr;
    }

    auto map = std::make_unique<VkInstantiatedInfluenceMap>(mSurface, this, mGraphicsView);
    mMaps.push_back(map.get());

    map->SetIndex(mWaterInformationData.mNumberOfInfluences);
    mWaterInformationData.mNumberOfInfluences++;
    mUpdateRequired = true;

    return std::move(map);
  }



  void VkWaterInfluenceMapManager::DestroyMap(VkInstantiatedInfluenceMap* aMap)
  {
    int index = aMap->mIndex;

    for (size_t i = index; i < mWaterInformationData.mNumberOfInfluences - 1; ++i)
    {
      mWaterInformationData.mInformation[i] = mWaterInformationData.mInformation[i + 1];
      mMaps[i + 1]->mIndex -= 1;
      VkInstantiatedInfluenceMap* temp = mMaps[i];
      mMaps[i] = mMaps[i + 1];
      mMaps[i + 1] = temp;
    }

    mMaps.pop_back();
    mWaterInformationData.mNumberOfInfluences -= 1;

    mUpdateRequired = true;
  }
}
