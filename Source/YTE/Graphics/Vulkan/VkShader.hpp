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
             VkRenderedSurface *aSurface,
             std::shared_ptr<vkhlf::PipelineLayout> aLayout,
             VkShaderDescriptions &aDescriptions);
    ~VkShader() override;

    void Load();

    void Reload() override;

    void LoadToVulkan(GraphicsDataUpdateVk *aEvent);
    
    std::shared_ptr<vkhlf::Pipeline> mShader;
    VkRenderedSurface *mSurface;

    std::shared_ptr<vkhlf::PipelineLayout> mPipelineLayout;
    VkShaderDescriptions mDescriptions;
  };
}

#endif
