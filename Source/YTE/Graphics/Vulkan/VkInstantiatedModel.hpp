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

    VkInstantiatedModel(std::string &aModelFile, VkRenderedSurface *aSurface);
    VkInstantiatedModel(Mesh *aMesh, VkRenderedSurface *aSurface);
    ~VkInstantiatedModel() override;

    void Create();

    void UpdateUBOModel(UBOModel &aUBO) override;
    void UpdateUBOAnimation(UBOAnimation *aUBO) override;
    void SetDefaultAnimationOffset() override;

    void CreateDescriptorSet(VkSubmesh *aMesh);
    void GraphicsDataUpdateVk(GraphicsDataUpdateVk *aEvent);

    struct SubMeshPipelineData
    {
      std::shared_ptr<vkhlf::DescriptorSet> mDescriptorSet;
      std::shared_ptr<vkhlf::PipelineLayout> mPipelineLayout;
    };

    std::shared_ptr<vkhlf::Buffer> mUBOModel;
    VkRenderedSurface *mSurface;

    std::shared_ptr<vkhlf::Buffer> mUBOAnimation;
    std::unordered_map<VkSubmesh*, SubMeshPipelineData> mPipelineData;
    VkMesh *mLoadedMesh;
    
  private:
    bool mLoadUBOModel;
    bool mLoadUBOAnimation;
  };
}

#endif
