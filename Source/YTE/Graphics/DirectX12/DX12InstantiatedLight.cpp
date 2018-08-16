///////////////////
// Author: Andrew Griffin
// YTE - Graphics - Vulkan
///////////////////

#include "YTE/Graphics/DirectX12/DX12VkInstantiatedLight.hpp"
#include "YTE/Graphics/DirectX12/DX12VkLightManager.hpp"
#include "YTE/Graphics/DirectX12/DX12Dx12RenderedSurface.hpp"
#include "YTE/Graphics/DirectX12/DX12Dx12RenderedSurface.hpp"
#include "YTE/Graphics/DirectX12/DX12Dx12Renderer.hpp"

namespace YTE
{
  YTEDefineType(VkInstantiatedLight)
  {
    RegisterType<VkInstantiatedLight>();
    TypeBuilder<VkInstantiatedLight> builder;
  }



  VkInstantiatedLight::VkInstantiatedLight(Dx12RenderedSurface* aSurface, VkLightManager* aLightManager, GraphicsView* aView)
    : InstantiatedLight()
    , mSurface(aSurface)
    , mManager(aLightManager)
    , mGraphicsView(aView)
  {
    mManager->RegisterEvent<&VkInstantiatedLight::GraphicsDataUpdateVk>(Events::GraphicsDataUpdateVk, this);

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
    mSurface = static_cast<Dx12Renderer*>(view->GetRenderer())->GetSurface(view->GetWindow());

    mManager = &(mSurface->GetViewData(view)->mLightManager);
    mManager->AddLight(this);
    mManager->RegisterEvent<&VkInstantiatedLight::GraphicsDataUpdateVk>(Events::GraphicsDataUpdateVk, this);
    mDataChanged = true;
  }

  void VkInstantiatedLight::GraphicsDataUpdateVk(YTE::GraphicsDataUpdateVk* aEvent)
  {
    UnusedArguments(aEvent);
    if (mDataChanged)
    {
      mManager->UpdateLightValue(mIndex, mLightUBOData);
      mDataChanged = false;
    }
  }
}
