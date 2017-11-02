///////////////////
// Author: Andrew Griffin
// YTE - Graphics - Vulkan
///////////////////

#pragma once

#ifndef YTE_Graphics_Vulkan_VkInstantiatedModel_hpp
#define YTE_Graphics_Vulkan_VkInstantiatedModel_hpp

#include "YTE/Graphics/Generics/InstantiatedModel.hpp"
#include "YTE/Graphics/UBOs.hpp"
#include "YTE/Graphics/Vulkan/ForwardDeclarations.hpp"
#include "YTE/Graphics/Vulkan/VkFunctionLoader.hpp"

namespace YTE
{
  class VkInstantiatedModel : public InstantiatedModel
  {
  public:
    YTEDeclareType(VkInstantiatedModel);

    VkInstantiatedModel(std::string &aModelFile, std::shared_ptr<VkRenderedSurface> aSurface);
    ~VkInstantiatedModel() override;

    void UpdateUBOModel(UBOModel &aUBO) override;

    void CreateDescriptorSet(std::shared_ptr<VkSubmesh> &mesh);
    void GraphicsDataUpdateVk(GraphicsDataUpdateVk *aEvent);



    std::shared_ptr<vkhlf::Buffer> mUBOModel;
    std::shared_ptr<VkRenderedSurface> mSurface;
    std::unordered_map<std::shared_ptr<VkSubmesh>, 
                       std::shared_ptr<vkhlf::DescriptorSet>> mDescriptorSets;
    std::unordered_map<std::shared_ptr<VkSubmesh>, 
                       std::shared_ptr<vkhlf::PipelineLayout>> mPipelineLayouts;
    std::shared_ptr<VkMesh> mLoadedMesh;
  };
}

#endif
