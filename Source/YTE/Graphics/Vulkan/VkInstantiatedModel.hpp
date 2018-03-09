///////////////////
// Author: Andrew Griffin
// YTE - Graphics - Vulkan
///////////////////

#pragma once

#ifndef YTE_Graphics_Vulkan_VkInstantiatedModel_hpp
#define YTE_Graphics_Vulkan_VkInstantiatedModel_hpp

#include "YTE/Graphics/ForwardDeclarations.hpp"
#include "YTE/Graphics/UBOs.hpp"

#include "YTE/Graphics/Generics/InstantiatedModel.hpp"

#include "YTE/Graphics/Vulkan/ForwardDeclarations.hpp"
#include "YTE/Graphics/Vulkan/VkFunctionLoader.hpp"
#include "YTE/Graphics/Vulkan/VkMesh.hpp"

namespace YTE
{
  class VkInstantiatedModel : public InstantiatedModel
  {
  public:
    YTEDeclareType(VkInstantiatedModel);

    VkInstantiatedModel(std::string &aModelFile, VkRenderedSurface *aSurface, GraphicsView *aView);
    VkInstantiatedModel(Mesh *aMesh, VkRenderedSurface *aSurface, GraphicsView *aView);
    ~VkInstantiatedModel() override;

    void Create();
    void CreateShader();
    void SurfaceLostEvent(ViewChanged *aEvent);
    void SurfaceGainedEvent(ViewChanged *aEvent);

    void UpdateUBOModel() override;
    void UpdateUBOModel(UBOModel &aUBO) override;
    void UpdateUBOAnimation(UBOAnimation *aUBO) override;
    void UpdateUBOMaterial(UBOMaterial *aUBO) override;
    void UpdateUBOSubmeshMaterial(UBOMaterial *aUBO, size_t aIndex) override;

    void SetDefaultAnimationOffset() override;

    // Takes the submesh, as well as the index of the submesh.
    void CreateDescriptorSet(VkSubmesh *aMesh, size_t mIndex);
    void GraphicsDataUpdateVk(GraphicsDataUpdateVk *aEvent);

    void SetInstanced(bool aInstanced) override;
    bool GetInstanced() override;

    std::shared_ptr<vkhlf::Buffer> mUBOModel;
    VkRenderedSurface *mSurface;
    GraphicsView *mView;

    std::shared_ptr<vkhlf::Buffer> mUBOAnimation;

    std::shared_ptr<vkhlf::Buffer> mUBOModelMaterial;
    std::vector<std::pair<std::shared_ptr<vkhlf::Buffer>, UBOMaterial>> mUBOSubmeshMaterials;

    // These are only needed if we're not instanced, otherwise lives on VkSubmesh.
    std::unordered_map<VkSubmesh*, SubMeshPipelineData> mPipelineData;

  private:
    bool mLoadUBOModel;
    bool mLoadUBOAnimation;
    bool mLoadUBOMaterial;
  };
}

#endif
