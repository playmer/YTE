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
    YTERegisterType(VkInstantiatedInfluenceMap);
  }



  VkInstantiatedInfluenceMap::VkInstantiatedInfluenceMap(VkRenderedSurface* aSurface, VkWaterInfluenceMapManager* aMapManager, GraphicsView* aView)
    : InstantiatedInfluenceMap()
    , mSurface(aSurface)
    , mManager(aMapManager)
    , mGraphicsView(aView)
  {
    mManager->YTERegister(Events::GraphicsDataUpdateVk, this, &VkInstantiatedInfluenceMap::GraphicsDataUpdateVk);

    mGraphicsView->YTERegister(Events::SurfaceLost, this, &VkInstantiatedInfluenceMap::SurfaceLostEvent);
    mGraphicsView->YTERegister(Events::SurfaceGained, this, &VkInstantiatedInfluenceMap::SurfaceGainedEvent);
  }

  VkInstantiatedInfluenceMap::~VkInstantiatedInfluenceMap()
  {
    mManager->DestroyMap(this);
  }

  void VkInstantiatedInfluenceMap::SurfaceLostEvent(ViewChanged *aEvent)
  {
    YTEUnusedArgument(aEvent);
    mManager->DestroyMap(this);
  }

  void VkInstantiatedInfluenceMap::SurfaceGainedEvent(ViewChanged *aEvent)
  {
    auto view = aEvent->View;
    mSurface = static_cast<VkRenderer*>(view->GetRenderer())->GetSurface(view->GetWindow());

    mManager = &(mSurface->GetViewData(view)->mWaterInfluenceMapManager);
    mManager->AddMap(this);
    mManager->YTERegister(Events::GraphicsDataUpdateVk, this, &VkInstantiatedInfluenceMap::GraphicsDataUpdateVk);
    mDataChanged = true;
  }

  void VkInstantiatedInfluenceMap::GraphicsDataUpdateVk(YTE::GraphicsDataUpdateVk* aEvent)
  {
    YTEUnusedArgument(aEvent);
    if (mDataChanged)
    {
      mManager->UpdateMapValue(mIndex, mInfluenceMapUBOData);
      mDataChanged = false;
    }
  }
}
