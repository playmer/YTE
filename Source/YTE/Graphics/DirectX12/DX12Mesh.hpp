///////////////////
// Author: Andrew Griffin
// YTE - Graphics - Vulkan
///////////////////

#pragma once

#ifndef YTE_Graphics_Vulkan_VkMesh_hpp
#define YTE_Graphics_Vulkan_VkMesh_hpp

#include <unordered_map>

#include "YTE/Graphics/Generics/Mesh.hpp"
#include "YTE/Graphics/DirectX12/DX12ForwardDeclarations.hpp"
#include "YTE/Graphics/DirectX12/DX12VkFunctionLoader.hpp"
#include "YTE/Graphics/DirectX12/DX12Dx12ShaderDescriptions.hpp"

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
    Dx12Submesh(VkMesh *aMesh, Submesh *aSubmesh, Dx12Renderer *aRenderer);
    ~Dx12Submesh();

    void Create();
    void Destroy();

    void LoadToVulkan(GraphicsDataUpdateVk *aEvent);

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

    VkTexture *mDiffuseTexture;
    VkTexture *mSpecularTexture;
    VkTexture *mNormalTexture;

    Dx12ShaderDescriptions mDescriptions;
    Dx12Renderer *mRenderer;
    
    VkMesh *mMesh;
    Submesh *mSubmesh;

    u64 mIndexCount;
  };


  // Manages instances of models. Able to map between InstantiatedModel and an index into the
  // instance buffer.
  struct InstanceManager
  {
  public:
    void AddModel(VkInstantiatedModel *aModel);
    void RemoveModel(VkInstantiatedModel *aModel);

    u32 GetIndex(VkInstantiatedModel *aModel);
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

    std::vector<VkInstantiatedModel*> mModels;
    std::vector<u32> mIndexes;

    std::shared_ptr<vkhlf::Buffer> mInstanceBuffer;
    u32 mInstances = 0; // Number of Instances available in the buffer.

    Dx12Renderer *mRenderer;
  };

  class VkMesh : public EventHandler
  {
  public:
    YTEDeclareType(VkMesh);

    VkMesh(Mesh *aMesh,
           Dx12Renderer *aRenderer);

    ~VkMesh();
    
    VkMesh(const VkMesh &aMesh) = delete;

    void RemoveOffset(VkInstantiatedModel *aModel);
    void RequestOffset(VkInstantiatedModel *aModel);

    void UpdateVertices(size_t aSubmeshIndex, std::vector<Vertex>& aVertices);
    void UpdateVerticesAndIndices(size_t aSubmeshIndex, std::vector<Vertex>& aVertices, std::vector<u32>& aIndices);

    u32 GetOffset(VkInstantiatedModel *aModel)
    {
      return mInstanceManager.GetIndex(aModel);
    }

    void SetInstanced(bool aInstanced);
    bool GetInstanced()
    {
      return mMesh->mInstanced;
    }

    void LoadToVulkan(GraphicsDataUpdateVk *aEvent);

    std::unordered_multimap<std::string, std::unique_ptr<Dx12Submesh>> mSubmeshes;
    InstanceManager mInstanceManager;
    Dx12Renderer *mRenderer;
    Mesh *mMesh;
  };
}

#endif
