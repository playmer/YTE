///////////////////
// Author: Andrew Griffin
// YTE - Graphics - Vulkan
///////////////////

#include "YTE/Graphics/Vulkan/VkInstantiatedLight.hpp"
#include "YTE/Graphics/Vulkan/VkLightManager.hpp"
#include "YTE/Graphics/Vulkan/VkRenderedSurface.hpp"
#include "YTE/Graphics/Vulkan/VkRenderedSurface.hpp"
#include "YTE/Graphics/Vulkan/VkRenderer.hpp"

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

    mGraphicsView->YTERegister(Events::SurfaceLost, this, &VkInstantiatedLight::SurfaceLostEvent);
    mGraphicsView->YTERegister(Events::SurfaceGained, this, &VkInstantiatedLight::SurfaceGainedEvent);
  }

  VkInstantiatedLight::~VkInstantiatedLight()
  {
    mManager->DestroyLight(this);
  }

  void VkInstantiatedLight::SurfaceLostEvent(ViewChanged *aEvent)
  {
    YTEUnusedArgument(aEvent);
    mManager->DestroyLight(this);
  }

  void VkInstantiatedLight::SurfaceGainedEvent(ViewChanged *aEvent)
  {
    auto view = aEvent->View;
    mSurface = static_cast<VkRenderer*>(view->GetRenderer())->GetSurface(view->GetWindow());

    mManager = &(mSurface->GetViewData(view)->mLightManager);
    mManager->AddLight(this);
    mManager->YTERegister(Events::GraphicsDataUpdateVk, this, &VkInstantiatedLight::GraphicsDataUpdateVk);
    mDataChanged = true;
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
