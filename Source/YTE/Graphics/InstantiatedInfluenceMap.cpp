#include "YTE/Graphics/GraphicsView.hpp"
#include "YTE/Graphics/InstantiatedInfluenceMap.hpp"
#include "YTE/Graphics/WaterInfluenceMapManager.hpp"

namespace YTE
{
  YTEDefineType(InstantiatedInfluenceMap)
  {
    RegisterType<InstantiatedInfluenceMap>();
    TypeBuilder<InstantiatedInfluenceMap> builder;
  }

  InstantiatedInfluenceMap::InstantiatedInfluenceMap(GraphicsView* aView)
    : mView{ aView }
    , mManager{ aView->GetWaterInfluenceMapManager() }
  {
    mDataChanged = true;
    mInfluenceMapUBOData.mColor = glm::vec3(1.0f, 1.0f, 1.0f);
    mInfluenceMapUBOData.mCenter = glm::vec3(0.0f, 0.0f, 0.0f);
    mInfluenceMapUBOData.mRadius = 1.0f;
    mInfluenceMapUBOData.mColorIntensity = 1.0f;
    mInfluenceMapUBOData.mColorInfluenceFunction = 0;
    mInfluenceMapUBOData.mWaveInfluenceFunction = 0;
    mInfluenceMapUBOData.mActive = 10;
    mInfluenceMapUBOData.mWaveIntensity = 1.0f;

    mView->RegisterEvent<&InstantiatedInfluenceMap::SurfaceLostEvent>(Events::SurfaceLost, this);
    mView->RegisterEvent<&InstantiatedInfluenceMap::SurfaceGainedEvent>(Events::SurfaceGained, this);
  }

  InstantiatedInfluenceMap::~InstantiatedInfluenceMap()
  {
    mManager->DestroyMap(this);
  }

  void InstantiatedInfluenceMap::SurfaceLostEvent(ViewChanged* aEvent)
  {
    UnusedArguments(aEvent);
    mManager->DestroyMap(this);
  }

  void InstantiatedInfluenceMap::SurfaceGainedEvent(ViewChanged* aEvent)
  {
    auto view = aEvent->View;

    mManager = view->GetWaterInfluenceMapManager();
    mManager->AddMap(this);
    mDataChanged = true;
  }
}


