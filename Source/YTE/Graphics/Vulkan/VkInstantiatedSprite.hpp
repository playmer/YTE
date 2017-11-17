///////////////////////////////////////////////////////////
// Author: Joshua T.Fisher
// YTE - Graphics - Vulkan
///////////////////////////////////////////////////////////

#pragma once

#ifndef YTE_Graphics_Vulkan_VkInstantiatedSprite_hpp
#define YTE_Graphics_Vulkan_VkInstantiatedSprite_hpp

#include "YTE/Graphics/Generics/InstantiatedSprite.hpp"
#include "YTE/Graphics/UBOs.hpp"
#include "YTE/Graphics/Vulkan/ForwardDeclarations.hpp"
#include "YTE/Graphics/Vulkan/VkFunctionLoader.hpp"

namespace YTE
{
  class VkInstantiatedSprite : public InstantiatedSprite
  {
  public:
    YTEDeclareType(VkInstantiatedSprite);

    VkInstantiatedSprite(std::string &aModelFile, VkRenderedSurface *aSurface);
    ~VkInstantiatedSprite() override;

    void UpdateTransformation(glm::mat4 &aTransformation) override;

    void GraphicsDataUpdate(GraphicsDataUpdateVk *aEvent);

  private:
  };
}

#endif
