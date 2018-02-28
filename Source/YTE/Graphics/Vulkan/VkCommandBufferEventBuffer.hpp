///////////////////
// Author: Andrew Griffin
// YTE - Graphics - Vulkan
///////////////////

#pragma once

#ifndef YTE_Graphics_Vulkan_VkCommandBufferEventBuffer_hpp
#define YTE_Graphics_Vulkan_VkCommandBufferEventBuffer_hpp

#include "YTE/Graphics/Vulkan/VkFunctionLoader.hpp"

namespace YTE
{
  // Command Buffer Event Buffered
  // Buffers based on BufferCount the CBOs available
  // reuses CBOs
  template<size_t T_BufferCount>
  class VkCBEB
  {
  public:
    // ctor
    VkCBEB(std::shared_ptr<vkhlf::Device>& aDevice)
    {
      mSize = T_BufferCount;
      mIndex = 0;
      for (size_t i = 0; i < mSize; ++i)
      {
        mEvents[i] = aDevice->createEvent();
        mHasBeenUsed[i] = false;
      }
    }


    // invalid, command buffers dont do well if there are more copies being used
    VkCBEB(VkCBEB& rhs) = delete;
    VkCBEB& operator=(VkCBEB& rhs) = delete;


    // dtor
    ~VkCBEB()
    {
      //for (size_t i = 0; i < mSize; ++i)
      //{
      //  mEvents[i].reset();
      //}
    }



    // Increments the current CBO and resets the one before it
    void NextEvent()
    {
      mIndex += 1;
      if (mIndex >= mSize)
      {
        mIndex = 0;
      }

      if (mHasBeenUsed[mIndex] == false)
      {
        mHasBeenUsed[mIndex] = true;
        return; // dont reset uninitialized data
      }
      mEvents[mIndex]->reset();
    }
    void operator++()
    {
      NextEvent();
    }
    void operator++(int)
    {
      NextEvent();
    }



    std::shared_ptr<vkhlf::Event>& GetCurrentEvent()
    {
      return mEvents[mIndex];
    }
    std::shared_ptr<vkhlf::Event>& operator*()
    {
      return mEvents[mIndex];
    }



    size_t size()
    {
      return mSize;
    }



  private:
    std::array<std::shared_ptr<vkhlf::Event>, T_BufferCount> mEvents;
    std::array<bool, T_BufferCount> mHasBeenUsed;
    size_t mIndex;
    size_t mSize = T_BufferCount;
  };
}


#endif