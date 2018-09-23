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
  struct DX12SubMeshPipelineData
  {
    //std::shared_ptr<vkhlf::DescriptorSet> mDescriptorSet;
    //std::shared_ptr<vkhlf::PipelineLayout> mPipelineLayout;
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
    //std::shared_ptr<vkhlf::DescriptorPool> MakePool();
    DX12SubMeshPipelineData CreatePipelineData(/*std::shared_ptr<vkhlf::Buffer> &aUBOModel,
                                               std::shared_ptr<vkhlf::Buffer> &aUBOAnimation,
                                               std::shared_ptr<vkhlf::Buffer> &aUBOModelMaterial,
                                               std::shared_ptr<vkhlf::Buffer> &aUBOSubmeshMaterial,*/
                                               GraphicsView *aView);

    //std::shared_ptr<vkhlf::Buffer> mVertexBuffer;
    //std::shared_ptr<vkhlf::Buffer> mIndexBuffer;
    //
    //std::shared_ptr<vkhlf::DescriptorSetLayout> mDescriptorSetLayout;
    //std::vector<vk::DescriptorPoolSize> mDescriptorTypes;

    // Needed if instanced, otherwise this lives per-model.
    DX12SubMeshPipelineData mPipelineData;

    DX12Texture *mDiffuseTexture;
    DX12Texture *mSpecularTexture;
    DX12Texture *mNormalTexture;

    Dx12ShaderDescriptions mDescriptions;
    Dx12Renderer *mRenderer;
    
    DX12Mesh *mMesh;
    Submesh *mSubmesh;

    u64 mIndexCount;
  };

  class DX12Mesh : public EventHandler
  {
  public:
    YTEDeclareType(DX12Mesh);

    DX12Mesh(Mesh *aMesh,
           Dx12Renderer *aRenderer);

    ~DX12Mesh();
    
    DX12Mesh(const DX12Mesh &aMesh) = delete;

    void UpdateVertices(size_t aSubmeshIndex, std::vector<Vertex>& aVertices);
    void UpdateVerticesAndIndices(size_t aSubmeshIndex, std::vector<Vertex>& aVertices, std::vector<u32>& aIndices);

    void LoadToVulkan(DX12GraphicsDataUpdate *aEvent);

    std::unordered_multimap<std::string, std::unique_ptr<Dx12Submesh>> mSubmeshes;
    Dx12Renderer *mRenderer;
    Mesh *mMesh;
  };
}

#endif
