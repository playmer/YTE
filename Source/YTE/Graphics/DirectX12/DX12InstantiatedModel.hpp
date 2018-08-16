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

#include "YTE/Graphics/DirectX12/DX12ForwardDeclarations.hpp"
#include "YTE/Graphics/DirectX12/DX12VkFunctionLoader.hpp"
#include "YTE/Graphics/DirectX12/DX12VkMesh.hpp"

namespace YTE
{
  class VkInstantiatedModel : public InstantiatedModel
  {
  public:
    YTEDeclareType(VkInstantiatedModel);

    VkInstantiatedModel(std::string &aModelFile, Dx12RenderedSurface *aSurface, GraphicsView *aView);
    VkInstantiatedModel(VkMesh *aMesh, Dx12RenderedSurface *aSurface, GraphicsView *aView);
    ~VkInstantiatedModel() override;

    void Create();
    void CreateShader();
    void SurfaceLostEvent(ViewChanged *aEvent);
    void SurfaceGainedEvent(ViewChanged *aEvent);

    void UpdateMesh(size_t aIndex, std::vector<Vertex>& aVertices) override;
    void UpdateMesh(size_t aIndex, std::vector<Vertex>& aVertices, std::vector<u32>& aIndices) override;

    void UpdateUBOModel() override;
    void UpdateUBOModel(UBOModel &aUBO) override;
    void UpdateUBOAnimation(UBOAnimation *aUBO) override;
    void UpdateUBOMaterial(UBOMaterial *aUBO) override;
    void UpdateUBOSubmeshMaterial(UBOMaterial *aUBO, size_t aIndex) override;

    void SetDefaultAnimationOffset() override;

    // Takes the submesh, as well as the index of the submesh.
    void CreateDescriptorSet(Dx12Submesh *aMesh, size_t mIndex);
    void GraphicsDataUpdateVk(GraphicsDataUpdateVk *aEvent);

    void SetInstanced(bool aInstanced) override;
    bool GetInstanced() override;

    std::shared_ptr<vkhlf::Buffer> mUBOModel;
    Dx12RenderedSurface *mSurface;
    GraphicsView *mView;

    std::shared_ptr<vkhlf::Buffer> mUBOAnimation;

    std::shared_ptr<vkhlf::Buffer> mUBOModelMaterial;
    std::vector<std::pair<std::shared_ptr<vkhlf::Buffer>, UBOMaterial>> mUBOSubmeshMaterials;

    // These are only needed if we're not instanced, otherwise lives on Dx12Submesh.
    std::unordered_map<Dx12Submesh*, SubMeshPipelineData> mPipelineData;

    VkMesh* GetVkMesh()
    {
      return mVkMesh;
    }

  private:
    bool mLoadUBOModel;
    bool mLoadUBOAnimation;
    bool mLoadUBOMaterial;
    VkMesh *mVkMesh;

    static UBOAnimation cAnimation;
  };
}

#endif
