#pragma once

#ifndef YTE_Graphics_Vulkan_VkCommandBufferBuffer_hpp
#define YTE_Graphics_Vulkan_VkCommandBufferBuffer_hpp

#include "YTE/Graphics/Vulkan/VkFunctionLoader.hpp"

namespace YTE
{
  // Command Buffer Object Buffered
  // Buffers based on BufferCount the CBOs available
  // reuses CBOs
  template<size_t tBufferCount, bool tIsSecondary>
  class VkCBOB
  {
  public:
    // ctor
    VkCBOB(std::shared_ptr<vkhlf::CommandPool>& aPool)
    {
      mSize = tBufferCount;
      mIndex = 0;
      mIsSecondary = tIsSecondary;
      auto& device = aPool->get<vkhlf::Device>();

      for (size_t i = 0; i < mSize; ++i)
      {
        if (mIsSecondary)
        {
          mCBOs[i] = aPool->allocateCommandBuffer(vk::CommandBufferLevel::eSecondary);
        }
        else
        {
          mCBOs[i] = aPool->allocateCommandBuffer(vk::CommandBufferLevel::ePrimary);
        }

        // We don't require fences on secondary buffers.
        if constexpr (false == tIsSecondary)
        {
          mFences[i] = device->createFence(false);
        }

        mHasBeenUsed[i] = false;
      }
    }


    // invalid, command buffers dont do well if there are more copies being used
    VkCBOB(VkCBOB& rhs) = delete;
    VkCBOB& operator=(VkCBOB& rhs) = delete;


    // dtor
    ~VkCBOB()
    {
      //for (size_t i = 0; i < mSize; ++i)
      //{
      //  mCBOs[i].reset();
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

    //std::shared_ptr<vkhlf::CommandBuffer>& GetCBO()
    //{
    //  return mCBOs[mIndex];
    //}
    //
    //std::shared_ptr<vkhlf::CommandBuffer>& operator*()
    //{
    //  return mCBOs[mIndex];
    //}


    std::pair<std::shared_ptr<vkhlf::CommandBuffer>&, std::shared_ptr<vkhlf::Fence>&> operator*()
    {
      return { mCBOs[mIndex], mFences[mIndex] };
    }

    size_t size()
    {
      return mSize;
    }

  private:
    // Increments the current CBO and resets the one before it
    void NextCommandBuffer()
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

      mCBOs[mIndex]->reset(vk::CommandBufferResetFlagBits::eReleaseResources);


      if constexpr (false == tIsSecondary)
      {
        mFences[mIndex]->reset();
      }
    }




    std::array<std::shared_ptr<vkhlf::CommandBuffer>, tBufferCount> mCBOs;
    std::array<std::shared_ptr<vkhlf::Fence>, tBufferCount> mFences;
    std::array<bool, tBufferCount> mHasBeenUsed;
    size_t mIndex;
    size_t mSize = tBufferCount;
    bool mIsSecondary = tIsSecondary;
  };
}


#endif