///////////////////
// Author: Andrew Griffin
// YTE - Graphics - Vulkan
///////////////////

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

    VkInstantiatedLight(VkRenderedSurface *aSurface, VkLightManager* aLightManager, GraphicsView* aView);
    ~VkInstantiatedLight() override;

    void GraphicsDataUpdateVk(GraphicsDataUpdateVk *aEvent);

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
