///////////////////
// Author: Andrew Griffin
// YTE - Graphics - Vulkan
///////////////////

#include "YTE/Graphics/Vulkan/VkInstantiatedLight.hpp"
#include "YTE/Graphics/Vulkan/VkLightManager.hpp"
#include "YTE/Graphics/Vulkan/VkRenderedSurface.hpp"

namespace YTE
{
  YTEDefineType(VkInstantiatedLight)
  {
    YTERegisterType(VkInstantiatedLight);
  }



  VkInstantiatedLight::VkInstantiatedLight(VkRenderedSurface* aSurface, VkLightManager* aLightManager, GraphicsView* aView)
    : InstantiatedLight()
    , mSurface(aSurface)
    , mManager(aLightManager)
    , mGraphicsView(aView)
  {
    mManager->YTERegister(Events::GraphicsDataUpdateVk, this, &VkInstantiatedLight::GraphicsDataUpdateVk);
  }



  VkInstantiatedLight::~VkInstantiatedLight()
  {
    mManager->DestroyLight(this);
  }



  void VkInstantiatedLight::GraphicsDataUpdateVk(YTE::GraphicsDataUpdateVk* aEvent)
  {
    YTEUnusedArgument(aEvent);
    if (mDataChanged)
    {
      mManager->UpdateLightValue(mIndex, mLightUBOData);
      mDataChanged = false;
    }
  }
}
