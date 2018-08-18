#pragma once

#ifndef YTE_Graphics_Vulkan_VkCommandBufferBuffer_hpp
#define YTE_Graphics_Vulkan_VkCommandBufferBuffer_hpp

#include "YTE/Graphics/DirectX12/DX12FunctionLoader.hpp"

namespace YTE
{
  // Command Buffer Object Buffered
  // Buffers based on BufferCount the CBOs available
  // reuses CBOs
  template<size_t tBufferCount, bool tIsSecondary>
  class Dx12CBOB
  {
  public:
    Dx12CBOB(/*std::shared_ptr<vkhlf::CommandPool>& aPool*/)
    {
      //mSize = tBufferCount;
      //mIndex = 0;
      //mIsSecondary = tIsSecondary;
      //for (size_t i = 0; i < mSize; ++i)
      //{
      //  if (mIsSecondary)
      //  {
      //    mCBOs[i] = aPool->allocateCommandBuffer(vk::CommandBufferLevel::eSecondary);
      //  }
      //  else
      //  {
      //    mCBOs[i] = aPool->allocateCommandBuffer(vk::CommandBufferLevel::ePrimary);
      //  }
      //  mHasBeenUsed[i] = false;
      //}
    }


    // invalid, command buffers dont do well if there are more copies being used
    Dx12CBOB(Dx12CBOB& rhs) = delete;
    Dx12CBOB& operator=(Dx12CBOB& rhs) = delete;


    // dtor
    ~Dx12CBOB()
    {
      //for (size_t i = 0; i < mSize; ++i)
      //{
      //  mCBOs[i].reset();
      //}
    }


    
    // Increments the current CBO and resets the one before it
    void NextCommandBuffer()
    {
      //mIndex += 1;
      //if (mIndex >= mSize)
      //{
      //  mIndex = 0;
      //}
      //
      //if (mHasBeenUsed[mIndex] == false)
      //{
      //  mHasBeenUsed[mIndex] = true;
      //  return; // dont reset uninitialized data
      //}
      //if (mIsSecondary)
      //{
      //  mCBOs[mIndex]->reset(vk::CommandBufferResetFlagBits::eReleaseResources);
      //}
      //else
      //{
      //  mCBOs[mIndex]->reset(vk::CommandBufferResetFlagBits::eReleaseResources);
      //}
    }

    void operator++()
    {
      NextCommandBuffer();
    }

    void operator++(int)
    {
      NextCommandBuffer();
    }

    //std::shared_ptr<vkhlf::CommandBuffer>& GetCurrentCBO()
    //{
    //  return mCBOs[mIndex];
    //}
    //
    //std::shared_ptr<vkhlf::CommandBuffer>& operator*()
    //{
    //  return mCBOs[mIndex];
    //}

    size_t size()
    {
      return mSize;
    }

  private:
    //std::array<std::shared_ptr<vkhlf::CommandBuffer>, tBufferCount> mCBOs;
    std::array<bool, tBufferCount> mHasBeenUsed;
    size_t mIndex;
    size_t mSize = tBufferCount;
    bool mIsSecondary = tIsSecondary;
  };
}


#endif