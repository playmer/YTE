#pragma once
#ifndef YTE_Graphics_GPUBuffer_hpp
#define YTE_Graphics_GPUBuffer_hpp

#include "YTE/Core/Utilities.hpp"

#include "YTE/StandardLibrary/PrivateImplementation.hpp"

namespace YTE
{
  class GPUBufferBase
  {
  public:
    GPUBufferBase(size_t aSize)
      : mArraySize{ aSize }
    {

    }

    PrivateImplementationLocal<32>& GetData()
    {
      return mData;
    }

    virtual void Update(u8 const* aPointer, size_t aBytes, size_t aOffset) = 0;

  protected:
    PrivateImplementationLocal<32> mData;
    size_t mArraySize;
  };

  template <typename tType>
  class GPUBuffer
  {
  public:


    GPUBuffer(std::unique_ptr<GPUBufferBase> aUBO)
      : mUBO{ std::move(aUBO) }
    {

    }

    GPUBufferBase& GetBase()
    {
      return *mUBO;
    }

    void Update(tType const& aData)
    {
      mUBO->Update(reinterpret_cast<u8 const*>(&aData), sizeof(tType), 0);
    }

  private:
    std::unique_ptr<GPUBufferBase> mUBO;
  };

  struct GPUAllocator
  {
    enum class MemoryProperty
    {
      DeviceLocal = 0x00000001,
      HostVisible = 0x00000002,
      HostCoherent = 0x00000004,
      HostCached = 0x00000008,
      LazilyAllocated = 0x00000010,
      Protected = 0x00000020,
    };

    enum class BufferUsage
    {
      TransferSrc = 0x00000001,
      TransferDst = 0x00000002,
      UniformTexelBuffer = 0x00000004,
      StorageTexelBuffer = 0x00000008,
      UniformBuffer = 0x00000010,
      StorageBuffer = 0x00000020,
      IndexBuffer = 0x00000040,
      VertexBuffer = 0x00000080,
      IndirectBuffer = 0x00000100,
    };

    GPUAllocator(size_t aBlockSize)
      : mBlockSize{ aBlockSize }
    {

    }

    // Creates a ubo of the given type, aSize allows you to make an array of them.
    // Passing 0 to aSize will result in returning nullptr.
    virtual std::unique_ptr<GPUBufferBase> CreateBuffer(size_t aSize, 
                                                        BufferUsage aUse, 
                                                        MemoryProperty aProperties) = 0;

    PrivateImplementationLocal<32> mData;
    size_t mBlockSize;
  };

  namespace AllocatorTypes
  {
    extern const std::string Mesh;
    extern const std::string Texture;
    extern const std::string UniformBufferObject;
    extern const std::string BufferUpdates;
  }
}

#endif