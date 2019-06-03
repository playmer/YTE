#pragma once

#ifndef YTE_Graphics_LightManager_hpp
#define YTE_Graphics_LightManager_hpp

#include "YTE/Core/EventHandler.hpp"
#include "YTE/Core/Utilities.hpp"

#include "YTE/Graphics/ForwardDeclarations.hpp"
#include "YTE/Graphics/GPUBuffer.hpp"
#include "YTE/Graphics/UBOs.hpp"

namespace YTE
{
  class LightManager : public EventHandler
  {
  public:
    LightManager();
    LightManager(GraphicsView* mGraphicsView);

    ~LightManager()
    {
      mBuffer.reset();
      mLights.clear();
    }

    void SetView(GraphicsView* aView);

    void LoadToGPU(GraphicsDataUpdate* aEvent);

    void AddLight(InstantiatedLight *aLight);
    std::unique_ptr<InstantiatedLight> CreateLight();
    void DestroyLight(InstantiatedLight* aLight);

    void SetLights(bool aOnOrOff);

    GPUBuffer<UBOs::LightManager>& GetUBOLightBuffer()
    {
      return mBuffer;
    }

  private:
    std::vector<InstantiatedLight*> mLights;
    UBOs::LightManager mLightData;
    GPUBuffer<UBOs::LightManager> mBuffer;
    GraphicsView* mGraphicsView;
    Renderer* mRenderer;
    unsigned char mLightUse[UBOs::LightCount];
    bool mUpdateRequired;
  };
}

#endif
