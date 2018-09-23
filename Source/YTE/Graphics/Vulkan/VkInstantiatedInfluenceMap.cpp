///////////////////
// Author: Andrew Griffin
// YTE - Graphics - Vulkan
///////////////////

#include "YTE/Graphics/Vulkan/VkInstantiatedInfluenceMap.hpp"
#include "YTE/Graphics/Vulkan/VkWaterInfluenceMapManager.hpp"
#include "YTE/Graphics/Vulkan/VkRenderedSurface.hpp"
#include "YTE/Graphics/Vulkan/VkRenderedSurface.hpp"
#include "YTE/Graphics/Vulkan/VkRenderer.hpp"

namespace YTE
{
  YTEDefineType(VkInstantiatedInfluenceMap)
  {
    RegisterType<VkInstantiatedInfluenceMap>();
    TypeBuilder<VkInstantiatedInfluenceMap> builder;
  }



  VkInstantiatedInfluenceMap::VkInstantiatedInfluenceMap(VkRenderedSurface* aSurface, VkWaterInfluenceMapManager* aMapManager, GraphicsView* aView)
    : InstantiatedInfluenceMap()
    , mSurface(aSurface)
    , mManager(aMapManager)
    , mGraphicsView(aView)
  {
    mManager->RegisterEvent<&VkInstantiatedInfluenceMap::VkGraphicsDataUpdate>(Events::VkGraphicsDataUpdate, this);

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
    mSurface = static_cast<VkRenderer*>(view->GetRenderer())->GetSurface(view->GetWindow());

    mManager = &(mSurface->GetViewData(view)->mWaterInfluenceMapManager);
    mManager->AddMap(this);
    mManager->RegisterEvent<&VkInstantiatedInfluenceMap::VkGraphicsDataUpdate>(Events::VkGraphicsDataUpdate, this);
    mDataChanged = true;
  }

  void VkInstantiatedInfluenceMap::VkGraphicsDataUpdate(YTE::VkGraphicsDataUpdate* aEvent)
  {
    UnusedArguments(aEvent);
    if (mDataChanged)
    {
      mManager->UpdateMapValue(mIndex, mInfluenceMapUBOData);
      mDataChanged = false;
    }
  }
}
