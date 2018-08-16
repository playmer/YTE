///////////////////
// Author: Andrew Griffin
// YTE - Graphics - Vulkan
///////////////////

#include "YTE/Graphics/DirectX12/DX12VkInstantiatedInfluenceMap.hpp"
#include "YTE/Graphics/DirectX12/DX12VkWaterInfluenceMapManager.hpp"
#include "YTE/Graphics/DirectX12/DX12Dx12RenderedSurface.hpp"
#include "YTE/Graphics/DirectX12/DX12Dx12RenderedSurface.hpp"
#include "YTE/Graphics/DirectX12/DX12Dx12Renderer.hpp"

namespace YTE
{
  YTEDefineType(VkInstantiatedInfluenceMap)
  {
    RegisterType<VkInstantiatedInfluenceMap>();
    TypeBuilder<VkInstantiatedInfluenceMap> builder;
  }



  VkInstantiatedInfluenceMap::VkInstantiatedInfluenceMap(Dx12RenderedSurface* aSurface, VkWaterInfluenceMapManager* aMapManager, GraphicsView* aView)
    : InstantiatedInfluenceMap()
    , mSurface(aSurface)
    , mManager(aMapManager)
    , mGraphicsView(aView)
  {
    mManager->RegisterEvent<&VkInstantiatedInfluenceMap::GraphicsDataUpdateVk>(Events::GraphicsDataUpdateVk, this);

    mGraphicsView->RegisterEvent<&VkInstantiatedInfluenceMap::SurfaceLostEvent>(Events::SurfaceLost, this);
    mGraphicsView->RegisterEvent<&VkInstantiatedInfluenceMap::SurfaceGainedEvent>(Events::SurfaceGained, this);
  }

  VkInstantiatedInfluenceMap::~VkInstantiatedInfluenceMap()
  {
    mManager->DestroyMap(this);
  }

  void VkInstantiatedInfluenceMap::SurfaceLostEvent(ViewChanged *aEvent)
  {
    UnusedArguments(aEvent);
    mManager->DestroyMap(this);
  }

  void VkInstantiatedInfluenceMap::SurfaceGainedEvent(ViewChanged *aEvent)
  {
    auto view = aEvent->View;
    mSurface = static_cast<Dx12Renderer*>(view->GetRenderer())->GetSurface(view->GetWindow());

    mManager = &(mSurface->GetViewData(view)->mWaterInfluenceMapManager);
    mManager->AddMap(this);
    mManager->RegisterEvent<&VkInstantiatedInfluenceMap::GraphicsDataUpdateVk>(Events::GraphicsDataUpdateVk, this);
    mDataChanged = true;
  }

  void VkInstantiatedInfluenceMap::GraphicsDataUpdateVk(YTE::GraphicsDataUpdateVk* aEvent)
  {
    UnusedArguments(aEvent);
    if (mDataChanged)
    {
      mManager->UpdateMapValue(mIndex, mInfluenceMapUBOData);
      mDataChanged = false;
    }
  }
}
