///////////////////
// Author: Andrew Griffin
// YTE - Graphics - Vulkan
///////////////////

#pragma once

#ifndef YTE_Graphics_Vulkan_VkMesh_hpp
#define YTE_Graphics_Vulkan_VkMesh_hpp

#include "YTE/Graphics/Generics/Mesh.hpp"
#include "YTE/Graphics/Vulkan/ForwardDeclarations.hpp"
#include "YTE/Graphics/Vulkan/VkFunctionLoader.hpp"

#include "YTE/StandardLibrary/Utilities.hpp"

namespace YTE
{
  class VkSubmesh
  {
  public:
    YTEDeclareType(VkSubmesh);
    VkSubmesh(Submesh *aSubmesh, VkRenderedSurface *aSurface);
    ~VkSubmesh();

    void LoadToVulkan(GraphicsDataUpdateVk *aEvent);

    std::shared_ptr<vkhlf::Buffer> mVertexBuffer;
    std::shared_ptr<vkhlf::Buffer> mIndexBuffer;
    std::shared_ptr<vkhlf::Buffer> mUBOMaterial;

    VkTexture *mDiffuseTexture;
    VkTexture *mSpecularTexture;
    VkTexture *mNormalTexture;

    VkShader *mShader;

    Submesh *mSubmesh;

    u64 mIndexCount;
  };



  class VkMesh : public Mesh
  {
  public:
    YTEDeclareType(VkMesh);

    VkMesh(Window *aWindow,
           VkRenderedSurface *aSurface,
           std::string &aFile,
           CreateInfo *aCreateInfo = nullptr);
    ~VkMesh();
    
    VkMesh(const VkMesh &aMesh) = delete;

    void LoadToVulkan(GraphicsDataUpdateVk *aEvent);

    std::vector<std::unique_ptr<VkSubmesh>> mSubmeshes;
    VkRenderedSurface *mSurface;
  };
}

#endif
