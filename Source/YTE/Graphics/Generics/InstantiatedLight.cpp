#include "YTE/Graphics/Generics/InstantiatedLight.hpp"

#include "YTE/Graphics/GraphicsView.hpp"
#include "YTE/Graphics/LightManager.hpp"

namespace YTE
{
  YTEDefineType(InstantiatedLight)
  {
    RegisterType<InstantiatedLight>();
    TypeBuilder<InstantiatedLight> builder;
  }

  InstantiatedLight::InstantiatedLight(GraphicsView* aView)
    : mGraphicsView{ aView }
    , mManager{ aView->GetLightManager() }
  {
    mDataChanged = true;
    mLightUBOData.mPosition = glm::vec3(0.0f, 0.0f, 0.0f);
    mLightUBOData.mActive = 10.0f;
    mLightUBOData.mDirection = glm::vec4(0.0f, 0.0f, 0.0f, 0.0f);
    mLightUBOData.mDiffuse = glm::vec4(1.0f, 1.0f, 1.0f, 1.0f);
    mLightUBOData.mAmbient = glm::vec4(0.0f, 0.0f, 0.0f, 1.0f);
    mLightUBOData.mSpecular = glm::vec3(1.0f, 1.0f, 1.0f);
    mLightUBOData.mIntensity = 1.0f;
    mLightUBOData.mSpotLightConeAngles = glm::vec2(glm::radians(10.0f), glm::radians(20.0f));
    mLightUBOData.mSpotLightFalloff = 10.0f;
    mLightUBOData.mLightType = static_cast<unsigned int>(LightType::Point);

    mGraphicsView->RegisterEvent<&InstantiatedLight::SurfaceLostEvent>(Events::SurfaceLost, this);
    mGraphicsView->RegisterEvent<&InstantiatedLight::SurfaceGainedEvent>(Events::SurfaceGained, this);

    mManager->AddLight(this);
  }

  InstantiatedLight::~InstantiatedLight()
  {
    mManager->DestroyLight(this);
  }


  void InstantiatedLight::SurfaceLostEvent(ViewChanged* aEvent)
  {
    UnusedArguments(aEvent);
    mManager->DestroyLight(this);
  }

  void InstantiatedLight::SurfaceGainedEvent(ViewChanged* aEvent)
  {
    auto view = aEvent->View;

    mManager = view->GetLightManager();
    mManager->AddLight(this);
    mDataChanged = true;
  }
}


