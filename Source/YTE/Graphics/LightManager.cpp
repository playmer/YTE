#include "YTE/Graphics/LightManager.hpp"
#include "YTE/Graphics/Generics/Renderer.hpp"
#include "YTE/Graphics/Generics/InstantiatedLight.hpp"

namespace YTE
{
  LightManager::LightManager()
  {
    mLightData.mActive = 0.0f; // false
    mLightData.mNumOfLights = 0;
  }

  LightManager::LightManager(GraphicsView* aGraphicsView)
    : mGraphicsView{ aGraphicsView }
    , mRenderer{ mGraphicsView->GetRenderer() }
  {
    //mSurface->RegisterEvent<&LightManager::LoadToVulkan>(Events::VkGraphicsDataUpdate, this);
    
    auto allocator = mRenderer->GetAllocator(AllocatorTypes::UniformBufferObject);

    mBuffer = allocator->CreateBuffer<UBOs::LightManager>(1, 
                                                          GPUAllocation::BufferUsage::TransferDst | 
                                                          GPUAllocation::BufferUsage::UniformBuffer,
                                                          GPUAllocation::MemoryProperty::DeviceLocal);

    mLightData.mActive = 0.0f; // false

    mLights.reserve(UBOs::LightCount);

    for (int i = 0; i < UBOs::LightCount; ++i)
    {
      mLightUse[i] = 0;
    }

    mUpdateRequired = true;
    mLightData.mNumOfLights = 0;
  }

  void LightManager::SetView(GraphicsView* aView)
  {
    mGraphicsView = aView;
    //mSurface->RegisterEvent<&LightManager::LoadToVulkan>(Events::GraphicsDataUpdate, this);

    auto allocator = mRenderer->GetAllocator(AllocatorTypes::UniformBufferObject);

    mBuffer = allocator->CreateBuffer<UBOs::LightManager>(1, 
                                                          GPUAllocation::BufferUsage::TransferDst | 
                                                          GPUAllocation::BufferUsage::UniformBuffer,
                                                          GPUAllocation::MemoryProperty::DeviceLocal);


    mLights.reserve(UBOs::LightCount);

    for (int i = 0; i < UBOs::LightCount; ++i)
    {
      mLightUse[i] = 0;
    }

    mUpdateRequired = true;
    mLightData.mNumOfLights = 0;
    mLightData.mActive = 0.0f; // false
  }

  void LightManager::LoadToGPU(GraphicsDataUpdate* aEvent)
  {
    UnusedArguments(aEvent);

    // Check to see if our lights need to update.
    // TODO: Maybe make this happen at a higher level once Buffers are API generic?
    for (auto& [lightIt, i] : enumerate(mLights))
    {
      auto& light = *lightIt;

      if (light->mDataChanged)
      {
        mLightData.mLights[i] = light->mLightUBOData;
        light->mDataChanged = false;
        mUpdateRequired = true;
      }
    }

    if (mUpdateRequired)
    {
      mBuffer.Update(mLightData);
      mUpdateRequired = false;
    }
  }

  void LightManager::AddLight(InstantiatedLight *aLight)
  {
    if (mLightData.mNumOfLights == UBOs::LightCount)
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

  std::unique_ptr<InstantiatedLight> LightManager::CreateLight()
  {
    if (mLightData.mNumOfLights == UBOs::LightCount)
    {
      DebugObjection(true , "Light Manager is full, no new lights can be added, you may safely continue, no light was added");
      return nullptr;
    }

    if (mLightData.mNumOfLights == 0)
    {
      mLightData.mActive = 10.0f; // true
    }

    auto light = std::make_unique<InstantiatedLight>(mGraphicsView);
    mLights.push_back(light.get());

    light->SetIndex(mLightData.mNumOfLights);
    mLightData.mNumOfLights++;
    mUpdateRequired = true;

    return std::move(light);
  }



  void LightManager::DestroyLight(InstantiatedLight* aLight)
  {
    int index = aLight->mIndex;

    for (size_t i = index; i < mLightData.mNumOfLights - 1; ++i)
    {
      mLightData.mLights[i] = mLightData.mLights[i + 1];
      mLights[i + 1]->mIndex -= 1;

      // TODO: std::swap
      InstantiatedLight* temp = mLights[i];
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

  void LightManager::SetLights(bool aOnOrOff)
  {
    mLightData.mActive = aOnOrOff ? 10.0f : 0.0f;
    mUpdateRequired = true;
  }

}
