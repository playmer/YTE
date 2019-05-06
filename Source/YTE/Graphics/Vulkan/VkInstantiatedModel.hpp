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

    YTE_Shared void CreateShader();
    YTE_Shared void SurfaceLostEvent(ViewChanged *aEvent);
    YTE_Shared void SurfaceGainedEvent(ViewChanged *aEvent);

    // Takes the submesh, as well as the index of the submesh.
    YTE_Shared void CreateDescriptorSet(VkSubmesh *aMesh, size_t mIndex);
    
    VkRenderedSurface *mSurface;
    GraphicsView *mView;

    // These are only needed if we're not instanced, otherwise lives on VkSubmesh.
    std::unordered_map<VkSubmesh*, SubMeshPipelineData> mPipelineData;

    VkMesh* GetVkMesh()
    {
      return mVkMesh;
    }

  private:
    VkMesh *mVkMesh;
  };
}

#endif
