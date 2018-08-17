#include "YTE/Graphics/DirectX12/DX12InstantiatedInfluenceMap.hpp"
#include "YTE/Graphics/DirectX12/DX12WaterInfluenceMapManager.hpp"
#include "YTE/Graphics/DirectX12/DX12RenderedSurface.hpp"
#include "YTE/Graphics/DirectX12/DX12RenderedSurface.hpp"
#include "YTE/Graphics/DirectX12/DX12Renderer.hpp"

namespace YTE
{
  YTEDefineType(DX12InstantiatedInfluenceMap)
  {
    RegisterType<DX12InstantiatedInfluenceMap>();
    TypeBuilder<DX12InstantiatedInfluenceMap> builder;
  }



  DX12InstantiatedInfluenceMap::DX12InstantiatedInfluenceMap(Dx12RenderedSurface* aSurface, DX12WaterInfluenceMapManager* aMapManager, GraphicsView* aView)
    : InstantiatedInfluenceMap()
    , mSurface(aSurface)
    , mManager(aMapManager)
    , mGraphicsView(aView)
  {
    mManager->RegisterEvent<&DX12InstantiatedInfluenceMap::DX12GraphicsDataUpdate>(Events::DX12GraphicsDataUpdate, this);

    mGraphicsView->RegisterEvent<&DX12InstantiatedInfluenceMap::SurfaceLostEvent>(Events::SurfaceLost, this);
    mGraphicsView->RegisterEvent<&DX12InstantiatedInfluenceMap::SurfaceGainedEvent>(Events::SurfaceGained, this);
  }

  DX12InstantiatedInfluenceMap::~DX12InstantiatedInfluenceMap()
  {
    mManager->DestroyMap(this);
  }

  void DX12InstantiatedInfluenceMap::SurfaceLostEvent(DX12ViewChanged *aEvent)
  {
    UnusedArguments(aEvent);
    mManager->DestroyMap(this);
  }

  void DX12InstantiatedInfluenceMap::SurfaceGainedEvent(DX12ViewChanged *aEvent)
  {
    auto view = aEvent->View;
    mSurface = static_cast<Dx12Renderer*>(view->GetRenderer())->GetSurface(view->GetWindow());

    mManager = &(mSurface->GetViewData(view)->mWaterInfluenceMapManager);
    mManager->AddMap(this);
    mManager->RegisterEvent<&DX12InstantiatedInfluenceMap::DX12GraphicsDataUpdate>(Events::DX12GraphicsDataUpdate, this);
    mDataChanged = true;
  }

  void DX12InstantiatedInfluenceMap::DX12GraphicsDataUpdate(YTE::DX12GraphicsDataUpdate* aEvent)
  {
    UnusedArguments(aEvent);
    if (mDataChanged)
    {
      mManager->UpdateMapValue(mIndex, mInfluenceMapUBOData);
      mDataChanged = false;
    }
  }
}
