#include "YTE/Graphics/DirectX12/DX12LightManager.hpp"
#include "YTE/Graphics/DirectX12/DX12RenderedSurface.hpp"
#include "YTE/Graphics/DirectX12/DX12DeviceInfo.hpp"
#include "YTE/Graphics/DirectX12/DX12InstantiatedLight.hpp"

namespace YTE
{
  DX12LightManager::DX12LightManager()
  {
    mLightData.mActive = 0.0f; // false
    mLightData.mNumOfLights = 0;
  }

  DX12LightManager::DX12LightManager(Dx12RenderedSurface* aSurface) : mSurface(aSurface)
  {
    mSurface->RegisterEvent<&DX12LightManager::GraphicsDataUpdateVkEvent>(Events::DX12GraphicsDataUpdate, this);

    auto allocator = mSurface->GetAllocator(AllocatorTypes::UniformBufferObject);

    mBuffer = mSurface->GetDevice()->createBuffer(sizeof(UBOLightMan),
                                                  vk::BufferUsageFlagBits::eTransferDst |
                                                  vk::BufferUsageFlagBits::eUniformBuffer,
                                                  vk::SharingMode::eExclusive,
                                                  nullptr,
                                                  vk::MemoryPropertyFlagBits::eDeviceLocal,
                                                  allocator);

    mLightData.mActive = 0.0f; // false

    mLights.reserve(YTE_Graphics_LightCount);

    for (int i = 0; i < YTE_Graphics_LightCount; ++i)
    {
      mLightUse[i] = 0;
    }

    mUpdateRequired = true;
    mLightData.mNumOfLights = 0;
  }

  void DX12LightManager::SetSurfaceAndView(Dx12RenderedSurface* aSurface, GraphicsView* aView)
  {
    mSurface = aSurface;
    mGraphicsView = aView;
    mSurface->RegisterEvent<&DX12LightManager::GraphicsDataUpdateVkEvent>(Events::DX12GraphicsDataUpdate, this);

    auto allocator = mSurface->GetAllocator(AllocatorTypes::UniformBufferObject);

    mBuffer = mSurface->GetDevice()->createBuffer(sizeof(UBOLightMan),
                                                  vk::BufferUsageFlagBits::eTransferDst |
                                                  vk::BufferUsageFlagBits::eUniformBuffer,
                                                  vk::SharingMode::eExclusive,
                                                  nullptr,
                                                  vk::MemoryPropertyFlagBits::eDeviceLocal,
                                                  allocator);

    mLights.reserve(YTE_Graphics_LightCount);

    for (int i = 0; i < YTE_Graphics_LightCount; ++i)
    {
      mLightUse[i] = 0;
    }

    mUpdateRequired = true;
    mLightData.mNumOfLights = 0;
    mLightData.mActive = 0.0f; // false
  }

  void DX12LightManager::GraphicsDataUpdateVkEvent(DX12GraphicsDataUpdate* aEvent)
  {
    SendEvent(Events::DX12GraphicsDataUpdate, aEvent);

    if (mUpdateRequired)
    {
      auto update = aEvent->mCBO;
      mBuffer->update<UBOLightMan>(0, mLightData, update);
      mUpdateRequired = false;
    }
  }

  void DX12LightManager::AddLight(DX12InstantiatedLight *aLight)
  {
    if (mLightData.mNumOfLights == YTE_Graphics_LightCount)
    {
      DebugObjection(true, "Light Manager is full, no new lights can be added, you may safely continue, no light was added");
    }

    if (mLightData.mNumOfLights == 0)
    {
      mLightData.mActive = 10.0f; // true
    }

    mLights.push_back(aLight);

    aLight->SetIndex(mLightData.mNumOfLights);
    mLightData.mNumOfLights++;
    mUpdateRequired = true;
  }

  std::unique_ptr<DX12InstantiatedLight> DX12LightManager::CreateLight()
  {
    if (mLightData.mNumOfLights == YTE_Graphics_LightCount)
    {
      DebugObjection(true , "Light Manager is full, no new lights can be added, you may safely continue, no light was added");
      return nullptr;
    }

    if (mLightData.mNumOfLights == 0)
    {
      mLightData.mActive = 10.0f; // true
    }

    auto light = std::make_unique<DX12InstantiatedLight>(mSurface, this, mGraphicsView);
    mLights.push_back(light.get());

    light->SetIndex(mLightData.mNumOfLights);
    mLightData.mNumOfLights++;
    mUpdateRequired = true;

    return std::move(light);
  }



  void DX12LightManager::DestroyLight(DX12InstantiatedLight* aLight)
  {
    int index = aLight->mIndex;

    for (size_t i = index; i < mLightData.mNumOfLights - 1; ++i)
    {
      mLightData.mLights[i] = mLightData.mLights[i + 1];
      mLights[i + 1]->mIndex -= 1;
      DX12InstantiatedLight* temp = mLights[i];
      mLights[i] = mLights[i + 1];
      mLights[i + 1] = temp;
    }

    mLights.pop_back();
    mLightData.mNumOfLights -= 1;

    if (mLightData.mNumOfLights == 0)
    {
      mLightData.mActive = 0.0f; // false
    }

    mUpdateRequired = true;
  }



  void DX12LightManager::UpdateLightValue(unsigned aIndex, UBOLight& aLightValue)
  {
//#ifdef _DEBUG
//    if (aIndex > mLightData.mNumOfLights || aIndex < 0)
//    {
//      DebugObjection(true, "Light Manager cannot access a value at the index of %d. Safe to Continue", aIndex);
//      return;
//    }
//#endif

    mLightData.mLights[aIndex] = aLightValue;
    mUpdateRequired = true;
  }

  void DX12LightManager::SetLights(bool aOnOrOff)
  {
    mLightData.mActive = aOnOrOff ? 10.0f : 0.0f;
    mUpdateRequired = true;
  }

}
