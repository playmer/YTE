#pragma once

#ifndef YTE_Graphics_Vulkan_VkCommandBufferEventBuffer_hpp
#define YTE_Graphics_Vulkan_VkCommandBufferEventBuffer_hpp

#include "YTE/Graphics/DirectX12/DX12FunctionLoader.hpp"

namespace YTE
{
  // Command Buffer Event Buffered Buffers, based on tBufferCount the CBOs
  // available reuses CBOs
  template<size_t tBufferCount>
  class DX12CBEB
  {
  public:
    // ctor
    DX12CBEB(/*std::shared_ptr<vkhlf::Device>& aDevice*/)
    {
      //mSize = tBufferCount;
      //mIndex = 0;
      //for (size_t i = 0; i < mSize; ++i)
      //{
      //  mEvents[i] = aDevice->createEvent();
      //  mHasBeenUsed[i] = false;
      //}
    }

    // invalid, command buffers don't do well if there are more copies being used
    DX12CBEB(DX12CBEB& rhs) = delete;
    DX12CBEB& operator=(DX12CBEB& rhs) = delete;

    ~DX12CBEB()
    {
      //for (size_t i = 0; i < mSize; ++i)
      //{
      //  mEvents[i].reset();
      //}
    }

    // Increments the current CBO and resets the one before it
    void NextEvent()
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
      //mEvents[mIndex]->reset();
    }

    void operator++()
    {
      NextEvent();
    }

    void operator++(int)
    {
      NextEvent();
    }

    //std::shared_ptr<vkhlf::Event>& GetCurrentEvent()
    //{
    //  return mEvents[mIndex];
    //}
    //
    //std::shared_ptr<vkhlf::Event>& operator*()
    //{
    //  return mEvents[mIndex];
    //}

    size_t size()
    {
      return mSize;
    }

  private:
    //std::array<std::shared_ptr<vkhlf::Event>, tBufferCount> mEvents;
    std::array<bool, tBufferCount> mHasBeenUsed;
    size_t mIndex;
    size_t mSize = tBufferCount;
  };
}


#endif