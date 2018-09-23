///////////////////
// Author: Andrew Griffin
// YTE - Graphics - Vulkan
///////////////////

#pragma once

#ifndef YTE_Graphics_Vulkan_VkMesh_hpp
#define YTE_Graphics_Vulkan_VkMesh_hpp

#include <unordered_map>

#include "YTE/Graphics/Generics/Mesh.hpp"
#include "YTE/Graphics/Vulkan/ForwardDeclarations.hpp"
#include "YTE/Graphics/Vulkan/VkFunctionLoader.hpp"
#include "YTE/Graphics/Vulkan/VkShaderDescriptions.hpp"

#include "YTE/StandardLibrary/Utilities.hpp"

namespace YTE
{
  struct SubMeshPipelineData
  {
    std::shared_ptr<vkhlf::DescriptorSet> mDescriptorSet;
    std::shared_ptr<vkhlf::PipelineLayout> mPipelineLayout;
  };

  class VkSubmesh
  {
  public:
    YTEDeclareType(VkSubmesh);
    VkSubmesh(VkMesh *aMesh, Submesh *aSubmesh, VkRenderer *aRenderer);
    ~VkSubmesh();

    void Create();
    void Destroy();

    void LoadToVulkan(VkGraphicsDataUpdate *aEvent);

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

    VkShaderDescriptions mDescriptions;
    VkRenderer *mRenderer;
    
    VkMesh *mMesh;
    Submesh *mSubmesh;

    u64 mIndexCount;
  };

  class VkMesh : public EventHandler
  {
  public:
    YTEDeclareType(VkMesh);

    VkMesh(Mesh *aMesh,
           VkRenderer *aRenderer);

    ~VkMesh();
    
    VkMesh(const VkMesh &aMesh) = delete;

    void UpdateVertices(size_t aSubmeshIndex, std::vector<Vertex>& aVertices);
    void UpdateVerticesAndIndices(size_t aSubmeshIndex, std::vector<Vertex>& aVertices, std::vector<u32>& aIndices);

    void LoadToVulkan(VkGraphicsDataUpdate *aEvent);

    std::unordered_multimap<std::string, std::unique_ptr<VkSubmesh>> mSubmeshes;
    VkRenderer *mRenderer;
    Mesh *mMesh;
  };
}

#endif
