///////////////////
// Author: Andrew Griffin
// YTE - Graphics - Vulkan
///////////////////

#pragma once

#ifndef YTE_Graphics_Vulkan_VkShader_hpp
#define YTE_Graphics_Vulkan_VkShader_hpp

#include "YTE/Graphics/Vulkan/VkFunctionLoader.hpp"
#include "YTE/Graphics/Vulkan/ForwardDeclarations.hpp"
#include "YTE/Graphics/Generics/Shader.hpp"

namespace YTE
{
  class VkShader : public Shader
  {
  public:
    YTEDeclareType(VkShader);

    VkShader(std::string &aName,
             std::shared_ptr<VkRenderedSurface> aSurface,
             std::shared_ptr<vkhlf::PipelineLayout> aLayout);
    ~VkShader() override;

    void LoadToVulkan(GraphicsDataUpdateVk *aEvent);
    
    
    
    std::shared_ptr<vkhlf::Pipeline> mShader;
    std::shared_ptr<VkRenderedSurface> mSurface;

    std::shared_ptr<vkhlf::PipelineLayout> mPipelineLayout;
  };
}

#endif