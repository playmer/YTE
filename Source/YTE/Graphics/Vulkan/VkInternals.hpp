///////////////////
// Author: Andrew Griffin
// YTE - Graphics - Vulkan
///////////////////

#pragma once

#ifndef YTE_Graphics_Vulkan_VkInternals_hpp
#define YTE_Graphics_Vulkan_VkInternals_hpp

#include "YTE/Graphics/Vulkan/VkFunctionLoader.hpp"
#include "YTE/Graphics/Vulkan/VkRenderer.hpp"
#include "YTE/Graphics/Vulkan/VkUtilities.hpp"

namespace YTE
{
  class VkInternals
  {
  public:
    // returns the first window's surface back to VkRenderer
    // a surface is needed for the physical device selection
    std::shared_ptr<vkhlf::Surface> InitializeVulkan(Engine *aEngine);
    void DeinitializeVulkan();

    std::shared_ptr<vkhlf::Instance> GetInstance() const
    {
      return mInstance;
    }

    std::shared_ptr<vkhlf::PhysicalDevice> GetPhysicalDevice() const
    {
      return mMainDevice;
    }

    QueueFamilyIndices GetQueueFamilies() const
    {
      return mQueueFamilyIndices;
    }

  private:
    void SelectDevice(std::shared_ptr<vkhlf::Surface> aSurface);
    void FindDeviceOfType(vk::PhysicalDeviceType aType);
    void PrintDeviceProperties(std::shared_ptr<vkhlf::PhysicalDevice> &aDevice);
    
    
    std::shared_ptr<vkhlf::Instance> mInstance;
    std::shared_ptr<vkhlf::DebugReportCallback> mDebugReportCallback;
    std::shared_ptr<vkhlf::PhysicalDevice> mMainDevice;
    std::vector<std::shared_ptr<vkhlf::PhysicalDevice>> mPhysicalDevices;
    QueueFamilyIndices mQueueFamilyIndices;
  };
}



#endif