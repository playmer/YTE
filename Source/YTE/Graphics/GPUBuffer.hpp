#pragma once
#ifndef YTE_Graphics_GPUBuffer_hpp
#define YTE_Graphics_GPUBuffer_hpp

#include <type_traits>

#include "YTE/Core/Utilities.hpp"

#include "YTE/StandardLibrary/PrivateImplementation.hpp"

namespace YTE
{
  class GPUBufferBase
  {
  public:
    GPUBufferBase(size_t aSizeInBytes)
      : mSizeInBytes{ aSizeInBytes }
    {

    }

    virtual ~GPUBufferBase()
    {

    }

    PrivateImplementationLocal<32>& GetData()
    {
      return mData;
    }

    virtual void Update(u8 const* aPointer, size_t aBytes, size_t aOffset) = 0;

    size_t GetSizeInBytes()
    {
      return mSizeInBytes;
    }

  protected:
    PrivateImplementationLocal<32> mData;
    size_t mSizeInBytes;
  };

  template <typename tType>
  class GPUBuffer
  {
  public:
    GPUBuffer()
    {

    }

    GPUBuffer(std::unique_ptr<GPUBufferBase> aBuffer)
      : mBuffer{ std::move(aBuffer) }
    {

    }

    size_t GetSizeInBytes()
    {
        return mBuffer->GetSizeInBytes();
    }

    GPUBufferBase& GetBase()
    {
      return *mBuffer;
    }

    void Update(tType const& aData)
    {
      auto bytes = sizeof(tType);

      DebugAssert(bytes <= mBuffer->GetSizeInBytes(), "We should always have enough bytes.");

      mBuffer->Update(reinterpret_cast<u8 const*>(&aData), bytes, 0);
    }

    void Update(tType const* aData, size_t aSize)
    {
      auto bytes = sizeof(tType) * aSize;

      DebugAssert(bytes <= mBuffer->GetSizeInBytes(), "We should always have enough bytes.");

      mBuffer->Update(reinterpret_cast<u8 const*>(aData), bytes, 0);
    }

    void Update(ContiguousRange<tType> aData)
    {
      auto bytes = sizeof(tType) * aData.size();

      DebugAssert(bytes <= mBuffer->GetSizeInBytes(), "We should always have enough bytes.");

      mBuffer->Update(reinterpret_cast<u8 const*>(aData.begin()), bytes, 0);
    }

    operator bool()
    {
      //return (nullptr != mBuffer) && (0 != mBuffer->GetSizeInBytes());
      return nullptr != mBuffer;
    }

    void reset()
    {
      mBuffer.reset();
    }

    std::unique_ptr<GPUBufferBase> Steal()
    {
      return std::move(mBuffer);
    }

  private:
    std::unique_ptr<GPUBufferBase> mBuffer;
  };



  template <typename tType>
  class GPUBufferRef
  {
  public:
    GPUBufferRef()
    {

    }

    GPUBufferRef(GPUBuffer<tType>& aBuffer)
      : mBuffer{ &(aBuffer.GetBase()) }
    {

    }

    GPUBufferRef(GPUBufferBase& aBuffer)
      : mBuffer{ &aBuffer }
    {

    }

    GPUBufferRef(GPUBufferBase* aBuffer)
      : mBuffer{ aBuffer }
    {

    }

    GPUBufferBase& GetBase()
    {
      return *mBuffer;
    }

    void Update(tType const& aData)
    {
      mBuffer->Update(reinterpret_cast<u8 const*>(&aData), sizeof(tType), 0);
    }

    void Update(tType const* aData, size_t aSize)
    {
      mBuffer->Update(reinterpret_cast<u8 const*>(aData), sizeof(tType) * aSize, 0);
    }

    void Update(ContiguousRange<tType> aData)
    {
      mBuffer->Update(reinterpret_cast<u8 const*>(aData.begin()), sizeof(tType) * aData.size(), 0);
    }

    operator bool()
    {
      return mBuffer != nullptr;
    }

  private:
    GPUBufferBase* mBuffer;
  };

  namespace GPUAllocation
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


    // https://softwareengineering.stackexchange.com/a/204566
    inline MemoryProperty operator | (MemoryProperty lhs, MemoryProperty rhs)
    {
      using T = std::underlying_type_t <MemoryProperty>;
      return static_cast<MemoryProperty>(static_cast<T>(lhs) | static_cast<T>(rhs));
    }

    inline MemoryProperty& operator |= (MemoryProperty& lhs, MemoryProperty rhs)
    {
      lhs = lhs | rhs;
      return lhs;
    }

    inline BufferUsage operator | (BufferUsage lhs, BufferUsage rhs)
    {
      using T = std::underlying_type_t <BufferUsage>;
      return static_cast<BufferUsage>(static_cast<T>(lhs) | static_cast<T>(rhs));
    }

    inline YTE::GPUAllocation::BufferUsage& operator |= (BufferUsage& lhs, BufferUsage rhs)
    {
      lhs = lhs | rhs;
      return lhs;
    }
  }

  struct GPUAllocator
  {
    GPUAllocator(size_t aBlockSize)
      : mBlockSize{ aBlockSize }
    {

    }

    // Creates a ubo of the given type, aSize allows you to make an array of them.
    // Size must be at least 1
    template <typename tType>
    GPUBuffer<tType> CreateBuffer(size_t aSize,
                                  GPUAllocation::BufferUsage aUse,
                                  GPUAllocation::MemoryProperty aProperties)
    {
      size_t sizeOfObject = sizeof(tType) * aSize;

      auto buffer = CreateBufferInternal(sizeOfObject, aUse, aProperties);

      return GPUBuffer<tType>(std::move(buffer));
    }

    PrivateImplementationLocal<64>& GetData()
    {
      return mData;
    }

  private:
    virtual std::unique_ptr<GPUBufferBase> CreateBufferInternal(size_t aSize,
                                                                GPUAllocation::BufferUsage aUse,
                                                                GPUAllocation::MemoryProperty aProperties) = 0;

  protected:
    PrivateImplementationLocal<64> mData;
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