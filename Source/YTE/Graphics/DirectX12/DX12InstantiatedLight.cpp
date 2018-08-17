#include "YTE/Graphics/DirectX12/DX12InstantiatedLight.hpp"
#include "YTE/Graphics/DirectX12/DX12LightManager.hpp"
#include "YTE/Graphics/DirectX12/DX12RenderedSurface.hpp"
#include "YTE/Graphics/DirectX12/DX12RenderedSurface.hpp"
#include "YTE/Graphics/DirectX12/DX12Renderer.hpp"

namespace YTE
{
  YTEDefineType(DX12InstantiatedLight)
  {
    RegisterType<DX12InstantiatedLight>();
    TypeBuilder<DX12InstantiatedLight> builder;
  }



  DX12InstantiatedLight::DX12InstantiatedLight(Dx12RenderedSurface* aSurface, DX12LightManager* aLightManager, GraphicsView* aView)
    : InstantiatedLight()
    , mSurface(aSurface)
    , mManager(aLightManager)
    , mGraphicsView(aView)
  {
    mManager->RegisterEvent<&DX12InstantiatedLight::DX12GraphicsDataUpdate>(Events::DX12GraphicsDataUpdate, this);

    mGraphicsView->RegisterEvent<&DX12InstantiatedLight::SurfaceLostEvent>(Events::SurfaceLost, this);
    mGraphicsView->RegisterEvent<&DX12InstantiatedLight::SurfaceGainedEvent>(Events::SurfaceGained, this);
  }

  DX12InstantiatedLight::~DX12InstantiatedLight()
  {
    mManager->DestroyLight(this);
  }

  void DX12InstantiatedLight::SurfaceLostEvent(DX12ViewChanged *aEvent)
  {
    UnusedArguments(aEvent);
    mManager->DestroyLight(this);
  }

  void DX12InstantiatedLight::SurfaceGainedEvent(DX12ViewChanged *aEvent)
  {
    auto view = aEvent->View;
    mSurface = static_cast<Dx12Renderer*>(view->GetRenderer())->GetSurface(view->GetWindow());

    mManager = &(mSurface->GetViewData(view)->mLightManager);
    mManager->AddLight(this);
    mManager->RegisterEvent<&DX12InstantiatedLight::DX12GraphicsDataUpdate>(Events::DX12GraphicsDataUpdate, this);
    mDataChanged = true;
  }

  void DX12InstantiatedLight::DX12GraphicsDataUpdate(YTE::DX12GraphicsDataUpdate* aEvent)
  {
    UnusedArguments(aEvent);
    if (mDataChanged)
    {
      mManager->UpdateLightValue(mIndex, mLightUBOData);
      mDataChanged = false;
    }
  }
}
