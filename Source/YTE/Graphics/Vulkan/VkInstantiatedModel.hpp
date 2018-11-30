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

    YTE_Shared VkInstantiatedModel(std::string &aModelFile, VkRenderedSurface *aSurface, GraphicsView *aView);
    YTE_Shared VkInstantiatedModel(VkMesh *aMesh, VkRenderedSurface *aSurface, GraphicsView *aView);
    YTE_Shared ~VkInstantiatedModel() override;

    YTE_Shared void Create();
    YTE_Shared void CreateShader();
    YTE_Shared void SurfaceLostEvent(ViewChanged *aEvent);
    YTE_Shared void SurfaceGainedEvent(ViewChanged *aEvent);

    YTE_Shared void UpdateMesh(size_t aIndex, std::vector<Vertex>& aVertices) override;
    YTE_Shared void UpdateMesh(size_t aIndex, std::vector<Vertex>& aVertices, std::vector<u32>& aIndices) override;

    YTE_Shared void UpdateUBOModel() override;
    YTE_Shared void UpdateUBOModel(UBOs::Model &aUBO) override;
    YTE_Shared void UpdateUBOAnimation(UBOs::Animation *aUBO) override;
    YTE_Shared void UpdateUBOMaterial(UBOs::Material *aUBO) override;
    YTE_Shared void UpdateUBOSubmeshMaterial(UBOs::Material *aUBO, size_t aIndex) override;

    YTE_Shared void SetDefaultAnimationOffset() override;

    // Takes the submesh, as well as the index of the submesh.
    YTE_Shared void CreateDescriptorSet(VkSubmesh *aMesh, size_t mIndex);
    YTE_Shared void VkGraphicsDataUpdate(VkGraphicsDataUpdate *aEvent);
    
    std::shared_ptr<vkhlf::Buffer> mUBOModel;
    VkRenderedSurface *mSurface;
    GraphicsView *mView;

    std::shared_ptr<vkhlf::Buffer> mUBOAnimation;

    std::shared_ptr<vkhlf::Buffer> mUBOModelMaterial;
    std::vector<std::pair<std::shared_ptr<vkhlf::Buffer>, UBOs::Material>> mUBOSubmeshMaterials;

    // These are only needed if we're not instanced, otherwise lives on VkSubmesh.
    std::unordered_map<VkSubmesh*, SubMeshPipelineData> mPipelineData;

    VkMesh* GetVkMesh()
    {
      return mVkMesh;
    }

  private:
    bool mLoadUBOModel;
    bool mLoadUBOAnimation;
    bool mLoadUBOMaterial;
    VkMesh *mVkMesh;

    static UBOs::Animation cAnimation;
  };
}

#endif
