#pragma once

#ifndef YTE_Graphics_Generics_Renderer_hpp
#define YTE_Graphics_Generics_Renderer_hpp

#include <future>
#include <shared_mutex>
#include <unordered_set>

#include "YTE/Core/Threading/JobSystem.hpp"
#include "YTE/Core/EventHandler.hpp"
#include "YTE/Core/Utilities.hpp"

#include "YTE/Graphics/ForwardDeclarations.hpp"
#include "YTE/Graphics/GPUBuffer.hpp"
#include "YTE/Graphics/GraphicsView.hpp"
#include "YTE/Graphics/UBOs.hpp"

#include "YTE/Graphics/Generics/ForwardDeclarations.hpp"
#include "YTE/Graphics/Generics/Texture.hpp"

#include "YTE/Platform/ForwardDeclarations.hpp"

namespace YTE
{
  class Renderer : public EventHandler
  {
  public:
    YTEDeclareType(Renderer);

    Renderer(Engine *aEngine);
    virtual ~Renderer();
    virtual void DeregisterWindowFromDraw(Window *aWindow);
    virtual void RegisterWindowForDraw(Window *aWindow);
    virtual std::unique_ptr<InstantiatedModel> CreateModel(GraphicsView *aView,
                                                           std::string &aMeshFile);
    virtual std::unique_ptr<InstantiatedModel> CreateModel(GraphicsView *aView, Mesh *aMesh);
    virtual void DestroyMeshAndModel(GraphicsView *aView, InstantiatedModel *aModel);

    virtual Texture* CreateTexture(std::string &aFilename, TextureType aType);
    virtual Texture* CreateTexture(std::string aName,
                                   std::vector<u8> aData,
                                   TextureLayout aLayout,
                                   u32 aWidth,
                                   u32 aHeight,
                                   u32 aMipLevels,
                                   u32 aLayerCount,
                                   TextureType aType);

    virtual Mesh* CreateSimpleMesh(std::string const& aName,
                                   ContiguousRange<SubmeshData> aSubmeshes,
			                             bool aForceUpdate = false);

    template <typename tType>
    GPUBuffer<tType> CreateUBO(
      size_t aSize = 1, 
      GPUAllocation::MemoryProperty aProperty = GPUAllocation::MemoryProperty::DeviceLocal)
    {
      auto allocator = GetAllocator(AllocatorTypes::UniformBufferObject);

      return allocator->CreateBuffer<tType>(aSize,
                                            GPUAllocation::BufferUsage::TransferDst |
                                            GPUAllocation::BufferUsage::UniformBuffer,
                                            aProperty);
    }

    virtual void SetLights(bool aOnOrOff);  // true for on, false for off
    virtual void RegisterView(GraphicsView *aView);
    virtual void RegisterView(GraphicsView *aView, DrawerTypes aDrawerType, DrawerTypeCombination aCombination);
    virtual void SetViewDrawingType(GraphicsView *aView, DrawerTypes aDrawerType, DrawerTypeCombination aCombination);
    virtual void SetViewCombinationType(GraphicsView *aView, DrawerTypeCombination aCombination);
    virtual void DeregisterView(GraphicsView *aView);
    virtual void ViewOrderChanged(GraphicsView *aView, float aNewOrder);

    virtual void ResetView(GraphicsView *aView);

    Mesh* RequestMesh(const std::string &aMeshFile);
    Texture* RequestTexture(const std::string &aFilename);

    Mesh* GetBaseMesh(const std::string &aFilename);
    Texture* GetBaseTexture(const std::string &aFilename);

    GPUAllocator* GetAllocator(std::string const& aAllocatorType)
    {
      if (auto it = mAllocators.find(aAllocatorType); it != mAllocators.end())
      {
        return it->second.get();
      }

      return nullptr;
    }

    virtual GPUAllocator* MakeAllocator(std::string const& aAllocatorType, size_t aBlockSize) = 0;

  protected:

    std::unordered_map<std::string, JobHandle> mRequestedMeshes;
    std::shared_mutex mRequestedMeshesMutex;
    std::unordered_map<std::string, std::unique_ptr<Mesh>> mBaseMeshes;
    std::shared_mutex mBaseMeshesMutex;

    std::unordered_map<std::string, JobHandle> mRequestedTextures;
    std::shared_mutex mRequestedTexturesMutex;
    std::unordered_map<std::string, std::unique_ptr<Texture>> mBaseTextures;
    std::shared_mutex mBaseTexturesMutex;

    std::unordered_map<std::string, std::unique_ptr<GPUAllocator>> mAllocators;

    std::unordered_set<std::string> mRequests;
    JobSystem *mJobSystem;
  };
}

#endif
