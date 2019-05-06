#pragma once

#ifndef YTE_Graphics_Vulkan_VkInstantiatedLight_hpp
#define YTE_Graphics_Vulkan_VkInstantiatedLight_hpp


#include "YTE/Graphics/Generics/InstantiatedLight.hpp"
#include "YTE/Graphics/ForwardDeclarations.hpp"
#include "YTE/Graphics/UBOs.hpp"
#include "YTE/Graphics/Vulkan/ForwardDeclarations.hpp"
#include "YTE/Graphics/Vulkan/VkFunctionLoader.hpp"

namespace YTE
{
  class VkInstantiatedLight : public InstantiatedLight
  {
  public:
    YTEDeclareType(VkInstantiatedLight);

    YTE_Shared VkInstantiatedLight(VkRenderedSurface *aSurface, VkLightManager* aLightManager, GraphicsView* aView);
    YTE_Shared ~VkInstantiatedLight() override;

    YTE_Shared void SurfaceLostEvent(ViewChanged *aEvent);
    YTE_Shared void SurfaceGainedEvent(ViewChanged *aEvent);

    void SetIndex(unsigned int aIndex)
    {
      mIndex = aIndex;
    }


    VkRenderedSurface *mSurface;
    GraphicsView *mGraphicsView;
    VkLightManager *mManager;
    unsigned int mIndex;
  };
}

#endif
