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
    YTE_Shared virtual void DeregisterWindowFromDraw(Window *aWindow);
    YTE_Shared virtual void RegisterWindowForDraw(Window *aWindow);
    YTE_Shared virtual std::unique_ptr<InstantiatedModel> CreateModel(GraphicsView *aView,
                                                                      std::string &aMeshFile);
    YTE_Shared virtual std::unique_ptr<InstantiatedModel> CreateModel(GraphicsView *aView, Mesh *aMesh);
    YTE_Shared virtual void DestroyMeshAndModel(GraphicsView *aView, InstantiatedModel *aModel);

    YTE_Shared virtual Texture* CreateTexture(std::string &aFilename, TextureType aType);
    YTE_Shared virtual Texture* CreateTexture(std::string aName,
                                              std::vector<u8> aData,
                                              TextureLayout aLayout,
                                              u32 aWidth,
                                              u32 aHeight,
                                              u32 aMipLevels,
                                              u32 aLayerCount,
                                              TextureType aType);

    YTE_Shared virtual Mesh* CreateSimpleMesh(std::string const& aName,
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

    YTE_Shared virtual void SetLights(bool aOnOrOff);  // true for on, false for off
    YTE_Shared virtual void RegisterView(GraphicsView *aView);
    YTE_Shared virtual void RegisterView(GraphicsView *aView, DrawerTypes aDrawerType, DrawerTypeCombination aCombination);
    YTE_Shared virtual void SetViewDrawingType(GraphicsView *aView, DrawerTypes aDrawerType, DrawerTypeCombination aCombination);
    YTE_Shared virtual void SetViewCombinationType(GraphicsView *aView, DrawerTypeCombination aCombination);
    YTE_Shared virtual void DeregisterView(GraphicsView *aView);
    YTE_Shared virtual void ViewOrderChanged(GraphicsView *aView, float aNewOrder);

    YTE_Shared virtual void ResetView(GraphicsView *aView);

    /////////////////////////////////
    // Assets
    YTE_Shared Mesh* RequestMesh(const std::string &aMeshFile);
    YTE_Shared Texture* RequestTexture(const std::string &aFilename);

    YTE_Shared Mesh* GetBaseMesh(const std::string &aFilename);
    YTE_Shared Texture* GetBaseTexture(const std::string &aFilename);

    /////////////////////////////////
    // Allocation
    GPUAllocator* GetAllocator(std::string const& aAllocatorType)
    {
      std::shared_lock<std::shared_mutex> reqLock(mAllocatorsMutex);

      if (auto it = mAllocators.find(aAllocatorType); it != mAllocators.end())
      {
        return it->second.get();
      }

      return nullptr;
    }

    virtual GPUAllocator* MakeAllocator(std::string const& aAllocatorType, size_t aBlockSize) = 0;

    
    /////////////////////////////////
    // Command Buffers
    
    //class CommandBuffer
    //{
    //public:
    // void SetViewport(glm::vec2 aPosition, glm::vec2 aDimensions, float aMinDepth, float aMaxDepth);
    // void SetScissor(glm::ivec2 aOffset, glm::uvec2 aExtent);
    // void SetLineWidth(float aWidth);
    // void BindVertexBuffers(ContiguousArray<GPUBufferBase> aBuffers, ContiguousArray<u64> aBufferOffset);
    // void BindIndexBuffers(ContiguousArray<GPUBufferBase> aBuffers, ContiguousArray<u64> aBufferOffset);
    // void DrawIndexed(u32 aIndexCount, u32 aInstanceCount, u32 aFirstIndex, i32 aVertexOffset, u32 aFirstInstance);
    //private:
    //
    //  std::vector<>
    //};

  protected:

    std::unordered_map<std::string, JobHandle> mRequestedMeshes;
    std::shared_mutex mRequestedMeshesMutex;
    std::unordered_map<std::string, std::unique_ptr<Mesh>> mBaseMeshes;
    std::shared_mutex mBaseMeshesMutex;

    std::vector<std::unique_ptr<Mesh>> mMeshesToDelete;

    std::unordered_map<std::string, JobHandle> mRequestedTextures;
    std::shared_mutex mRequestedTexturesMutex;
    std::unordered_map<std::string, std::unique_ptr<Texture>> mBaseTextures;
    std::shared_mutex mBaseTexturesMutex;

    std::unordered_map<std::string, std::unique_ptr<GPUAllocator>> mAllocators;
    std::shared_mutex mAllocatorsMutex;

    std::unordered_set<std::string> mRequests;
    JobSystem *mJobSystem;
  };
}

#endif
