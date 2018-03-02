/////////////////
// Author: Andrew Griffin
// YTE - Graphics - Vulkan
///////////////////

#include "YTE/Graphics/Vulkan/VkLightManager.hpp"
#include "YTE/Graphics/Vulkan/VkRenderedSurface.hpp"
#include "YTE/Graphics/Vulkan/VkDeviceInfo.hpp"
#include "YTE/Graphics/Vulkan/VkInstantiatedLight.hpp"



namespace YTE
{
  VkLightManager::VkLightManager()
  {
    mLightData.mActive = 0.0f; // false
    mLightData.mNumOfLights = 0;
  }

  VkLightManager::VkLightManager(VkRenderedSurface* aSurface) : mSurface(aSurface)
  {
    mSurface->YTERegister(Events::GraphicsDataUpdateVk, this, &VkLightManager::GraphicsDataUpdateVkEvent);

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

  void VkLightManager::SetSurfaceAndView(VkRenderedSurface* aSurface, GraphicsView* aView)
  {
    mSurface = aSurface;
    mGraphicsView = aView;
    mSurface->YTERegister(Events::GraphicsDataUpdateVk, this, &VkLightManager::GraphicsDataUpdateVkEvent);

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

  void VkLightManager::GraphicsDataUpdateVkEvent(GraphicsDataUpdateVk* aEvent)
  {
    SendEvent(Events::GraphicsDataUpdateVk, aEvent);

    if (mUpdateRequired)
    {
      auto update = aEvent->mCBO;
      mBuffer->update<UBOLightMan>(0, mLightData, update);
      mUpdateRequired = false;
    }
  }

  void VkLightManager::AddLight(VkInstantiatedLight *aLight)
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

  std::unique_ptr<VkInstantiatedLight> VkLightManager::CreateLight()
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

    auto light = std::make_unique<VkInstantiatedLight>(mSurface, this, mGraphicsView);
    mLights.push_back(light.get());

    light->SetIndex(mLightData.mNumOfLights);
    mLightData.mNumOfLights++;
    mUpdateRequired = true;

    return std::move(light);
  }



  void VkLightManager::DestroyLight(VkInstantiatedLight* aLight)
  {
    int index = aLight->mIndex;

    for (size_t i = index; i < mLightData.mNumOfLights - 1; ++i)
    {
      mLightData.mLights[i] = mLightData.mLights[i + 1];
      mLights[i + 1]->mIndex -= 1;
      VkInstantiatedLight* temp = mLights[i];
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



  void VkLightManager::UpdateLightValue(unsigned aIndex, UBOLight& aLightValue)
  {
#ifdef _DEBUG
    if (aIndex > mLightData.mNumOfLights || aIndex < 0)
    {
      DebugObjection(true, "Light Manager cannot access a value at the index of %d. Safe to Continue", aIndex);
      return;
    }
#endif

    mLightData.mLights[aIndex] = aLightValue;
    mUpdateRequired = true;
  }

  void VkLightManager::SetLights(bool aOnOrOff)
  {
    mLightData.mActive = aOnOrOff ? 10.0f : 0.0f;
    mUpdateRequired = true;
  }

}
