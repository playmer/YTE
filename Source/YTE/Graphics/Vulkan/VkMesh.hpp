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
    VkSubmesh(VkMesh *aMesh, Submesh *aSubmesh, VkRenderedSurface *aSurface);
    ~VkSubmesh();

    void Create();
    void Destroy();

    void LoadToVulkan(GraphicsDataUpdateVk *aEvent);

    std::shared_ptr<vkhlf::DescriptorPool> MakePool();
    SubMeshPipelineData CreatePipelineData(std::shared_ptr<vkhlf::Buffer> &aUBOModel,
                                           std::shared_ptr<vkhlf::Buffer> &aUBOAnimation,
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

    VkShader *mShader;
    VkRenderedSurface *mSurface;
    
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

    VkRenderedSurface *mSurface;
  };

  class VkMesh : public Mesh
  {
  public:
    YTEDeclareType(VkMesh);

    VkMesh(Window *aWindow,
           VkRenderedSurface *aSurface,
           std::string &aFile,
           CreateInfo *aCreateInfo = nullptr);

    VkMesh(Window *aWindow,
           VkRenderedSurface *aSurface,
           std::string &aFile,
           std::vector<Submesh> &aSubmeshes);

    ~VkMesh();
    
    VkMesh(const VkMesh &aMesh) = delete;

    void RemoveOffset(VkInstantiatedModel *aModel);
    void RequestOffset(VkInstantiatedModel *aModel);

    u32 GetOffset(VkInstantiatedModel *aModel)
    {
      return mInstanceManager.GetIndex(aModel);
    }

    void SetInstanced(bool aInstanced);
    bool GetInstanced()
    {
      return mInstanced;
    }

    void LoadToVulkan(GraphicsDataUpdateVk *aEvent);

    void RecreateShader() override;

    std::unordered_multimap<VkShader*, std::unique_ptr<VkSubmesh>> mSubmeshes;
    InstanceManager mInstanceManager;
    VkRenderedSurface *mSurface;
  };
}

#endif
