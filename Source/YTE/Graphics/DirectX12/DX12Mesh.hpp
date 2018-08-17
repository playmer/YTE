#pragma once

#ifndef YTE_Graphics_Vulkan_VkMesh_hpp
#define YTE_Graphics_Vulkan_VkMesh_hpp

#include <unordered_map>

#include "YTE/Graphics/Generics/Mesh.hpp"
#include "YTE/Graphics/DirectX12/ForwardDeclarations.hpp"
#include "YTE/Graphics/DirectX12/DX12FunctionLoader.hpp"
#include "YTE/Graphics/DirectX12/DX12ShaderDescriptions.hpp"

#include "YTE/StandardLibrary/Utilities.hpp"

namespace YTE
{
  struct SubMeshPipelineData
  {
    std::shared_ptr<vkhlf::DescriptorSet> mDescriptorSet;
    std::shared_ptr<vkhlf::PipelineLayout> mPipelineLayout;
  };

  class Dx12Submesh
  {
  public:
    YTEDeclareType(Dx12Submesh);
    Dx12Submesh(DX12Mesh *aMesh, Submesh *aSubmesh, Dx12Renderer *aRenderer);
    ~Dx12Submesh();

    void Create();
    void Destroy();

    void LoadToVulkan(DX12GraphicsDataUpdate *aEvent);

    void CreateShader(GraphicsView *aView);
    std::shared_ptr<vkhlf::DescriptorPool> MakePool();
    SubMeshPipelineData CreatePipelineData(std::shared_ptr<vkhlf::Buffer> &aUBOModel,
                                           std::shared_ptr<vkhlf::Buffer> &aUBOAnimation,
                                           std::shared_ptr<vkhlf::Buffer> &aUBOModelMaterial,
                                           std::shared_ptr<vkhlf::Buffer> &aUBOSubmeshMaterial,
                                           GraphicsView *aView);

    std::shared_ptr<vkhlf::Buffer> mVertexBuffer;
    std::shared_ptr<vkhlf::Buffer> mIndexBuffer;

    std::shared_ptr<vkhlf::DescriptorSetLayout> mDescriptorSetLayout;
    std::vector<vk::DescriptorPoolSize> mDescriptorTypes;

    // Needed if instanced, otherwise this lives per-model.
    SubMeshPipelineData mPipelineData;

    DX12Texture *mDiffuseTexture;
    DX12Texture *mSpecularTexture;
    DX12Texture *mNormalTexture;

    Dx12ShaderDescriptions mDescriptions;
    Dx12Renderer *mRenderer;
    
    DX12Mesh *mMesh;
    Submesh *mSubmesh;

    u64 mIndexCount;
  };


  // Manages instances of models. Able to map between InstantiatedModel and an index into the
  // instance buffer.
  struct InstanceManager
  {
  public:
    void AddModel(DX12InstantiatedModel *aModel);
    void RemoveModel(DX12InstantiatedModel *aModel);

    u32 GetIndex(DX12InstantiatedModel *aModel);
    void Clear();

    std::shared_ptr<vkhlf::Buffer>& InstanceBuffer();
    u32 Instances();

  private:
    void GrowBuffer();

    u32 FreeIndex()
    {
      return static_cast<u32>(mModels.size());
    }

    std::vector<u32>::iterator GetFreeBegin()
    {
      return mIndexes.begin() + mModels.size();
    }

    std::vector<u32>::iterator GetFreeEnd()
    {
      return mIndexes.end();
    }

    std::vector<DX12InstantiatedModel*> mModels;
    std::vector<u32> mIndexes;

    std::shared_ptr<vkhlf::Buffer> mInstanceBuffer;
    u32 mInstances = 0; // Number of Instances available in the buffer.

    Dx12Renderer *mRenderer;
  };

  class DX12Mesh : public EventHandler
  {
  public:
    YTEDeclareType(DX12Mesh);

    DX12Mesh(Mesh *aMesh,
           Dx12Renderer *aRenderer);

    ~DX12Mesh();
    
    DX12Mesh(const DX12Mesh &aMesh) = delete;

    void RemoveOffset(DX12InstantiatedModel *aModel);
    void RequestOffset(DX12InstantiatedModel *aModel);

    void UpdateVertices(size_t aSubmeshIndex, std::vector<Vertex>& aVertices);
    void UpdateVerticesAndIndices(size_t aSubmeshIndex, std::vector<Vertex>& aVertices, std::vector<u32>& aIndices);

    u32 GetOffset(DX12InstantiatedModel *aModel)
    {
      return mInstanceManager.GetIndex(aModel);
    }

    void SetInstanced(bool aInstanced);
    bool GetInstanced()
    {
      return mMesh->mInstanced;
    }

    void LoadToVulkan(DX12GraphicsDataUpdate *aEvent);

    std::unordered_multimap<std::string, std::unique_ptr<Dx12Submesh>> mSubmeshes;
    InstanceManager mInstanceManager;
    Dx12Renderer *mRenderer;
    Mesh *mMesh;
  };
}

#endif
