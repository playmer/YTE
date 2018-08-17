#pragma once

#ifndef YTE_Graphics_Vulkan_VkInstantiatedModel_hpp
#define YTE_Graphics_Vulkan_VkInstantiatedModel_hpp

#include "YTE/Graphics/ForwardDeclarations.hpp"
#include "YTE/Graphics/UBOs.hpp"

#include "YTE/Graphics/Generics/InstantiatedModel.hpp"

#include "YTE/Graphics/DirectX12/ForwardDeclarations.hpp"
#include "YTE/Graphics/DirectX12/DX12FunctionLoader.hpp"
#include "YTE/Graphics/DirectX12/DX12Mesh.hpp"

namespace YTE
{
  class DX12InstantiatedModel : public InstantiatedModel
  {
  public:
    YTEDeclareType(DX12InstantiatedModel);

    DX12InstantiatedModel(std::string &aModelFile, Dx12RenderedSurface *aSurface, GraphicsView *aView);
    DX12InstantiatedModel(DX12Mesh *aMesh, Dx12RenderedSurface *aSurface, GraphicsView *aView);
    ~DX12InstantiatedModel() override;

    void Create();
    void CreateShader();
    void SurfaceLostEvent(DX12ViewChanged *aEvent);
    void SurfaceGainedEvent(DX12ViewChanged *aEvent);

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
    void DX12GraphicsDataUpdate(DX12GraphicsDataUpdate *aEvent);

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

    DX12Mesh* GetVkMesh()
    {
      return mVkMesh;
    }

  private:
    bool mLoadUBOModel;
    bool mLoadUBOAnimation;
    bool mLoadUBOMaterial;
    DX12Mesh *mVkMesh;

    static UBOAnimation cAnimation;
  };
}

#endif
