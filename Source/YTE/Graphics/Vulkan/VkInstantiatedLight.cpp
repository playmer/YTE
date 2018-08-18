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
    RegisterType<VkInstantiatedLight>();
    TypeBuilder<VkInstantiatedLight> builder;
  }



  VkInstantiatedLight::VkInstantiatedLight(VkRenderedSurface* aSurface, VkLightManager* aLightManager, GraphicsView* aView)
    : InstantiatedLight()
    , mSurface(aSurface)
    , mManager(aLightManager)
    , mGraphicsView(aView)
  {
    mManager->RegisterEvent<&VkInstantiatedLight::VkGraphicsDataUpdate>(Events::VkGraphicsDataUpdate, this);

    mGraphicsView->RegisterEvent<&VkInstantiatedLight::SurfaceLostEvent>(Events::SurfaceLost, this);
    mGraphicsView->RegisterEvent<&VkInstantiatedLight::SurfaceGainedEvent>(Events::SurfaceGained, this);
  }

  VkInstantiatedLight::~VkInstantiatedLight()
  {
    mManager->DestroyLight(this);
  }

  void VkInstantiatedLight::SurfaceLostEvent(ViewChanged *aEvent)
  {
    UnusedArguments(aEvent);
    mManager->DestroyLight(this);
  }

  void VkInstantiatedLight::SurfaceGainedEvent(ViewChanged *aEvent)
  {
    auto view = aEvent->View;
    mSurface = static_cast<VkRenderer*>(view->GetRenderer())->GetSurface(view->GetWindow());

    mManager = &(mSurface->GetViewData(view)->mLightManager);
    mManager->AddLight(this);
    mManager->RegisterEvent<&VkInstantiatedLight::VkGraphicsDataUpdate>(Events::VkGraphicsDataUpdate, this);
    mDataChanged = true;
  }

  void VkInstantiatedLight::VkGraphicsDataUpdate(YTE::VkGraphicsDataUpdate* aEvent)
  {
    UnusedArguments(aEvent);
    if (mDataChanged)
    {
      mManager->UpdateLightValue(mIndex, mLightUBOData);
      mDataChanged = false;
    }
  }
}
