#include "YTE/StandardLibrary/Utilities.hpp"

#include "YTE/Graphics/Vulkan/VkFunctionLoader.hpp"
#include "YTE/Graphics/Vulkan/VkDeviceInfo.hpp"

namespace YTE
{
  std::vector<const char*> QueueFamilyIndices::sDeviceExtensions;



  QueueFamilyIndices QueueFamilyIndices::FindQueueFamilies(std::shared_ptr<vkhlf::PhysicalDevice>& aDevice, vk::QueueFlags aQueueFlag)
  {
    QueueFamilyIndices indices;

    auto queueFamilyProperties = aDevice->getQueueFamilyProperties();

    // Dedicated queue for compute
    // Try to find a queue family index that supports compute but not graphics
    if (aQueueFlag & vk::QueueFlagBits::eCompute)
    {
      for (uint32_t i = 0; i < static_cast<uint32_t>(queueFamilyProperties.size()); i++)
      {
        if ((queueFamilyProperties[i].queueFlags & aQueueFlag) && 
            ((queueFamilyProperties[i].queueFlags & vk::QueueFlagBits::eGraphics) == vk::QueueFlags{0}))
        {
          indices.mGraphicsFamily = i;
          return indices;
        }
      }
    }

    // Dedicated queue for transfer
    // Try to find a queue family index that supports transfer but not graphics and compute
    if (aQueueFlag & vk::QueueFlagBits::eTransfer)
    {
      for (uint32_t i = 0; i < static_cast<uint32_t>(queueFamilyProperties.size()); i++)
      {
        if ((queueFamilyProperties[i].queueFlags & aQueueFlag) && 
            ((queueFamilyProperties[i].queueFlags & vk::QueueFlagBits::eGraphics) == vk::QueueFlags{0}) &&
            ((queueFamilyProperties[i].queueFlags & vk::QueueFlagBits::eCompute) == vk::QueueFlags{0}))
        {
          indices.mGraphicsFamily = i;
          return indices;
        }
      }
    }

    // For other queue types or if no separate compute queue is present, return the first one to support the requested flags
    for (uint32_t i = 0; i < static_cast<uint32_t>(queueFamilyProperties.size()); i++)
    {
      if (queueFamilyProperties[i].queueFlags & aQueueFlag)
      {
        indices.mGraphicsFamily = i;
        return indices;
      }
    }

    return indices;
  }



  bool QueueFamilyIndices::IsDeviceSuitable(std::shared_ptr<vkhlf::PhysicalDevice>& aDevice, std::shared_ptr<vkhlf::Surface>& aSurface)
  {
    auto indices = FindQueueFamilies(aDevice, vk::QueueFlagBits::eGraphics);

    bool extensionsSupported = CheckDeviceExtensionSupport(aDevice);

    bool swapChainAdequate = false;

    if (extensionsSupported)
    {
      SwapChainSupportDetails swapChainSupport = SwapChainSupportDetails::QuerySwapChainSupport(
                                                   aDevice, aSurface);
      swapChainAdequate = !swapChainSupport.Formats().empty() && 
                          !swapChainSupport.PresentModes().empty();
    }

    auto supportsSurface = aDevice->getSurfaceSupport(mGraphicsFamily, aSurface);

    return indices.IsComplete() && 
           extensionsSupported && 
           swapChainAdequate &&
           supportsSurface;
  }



  bool QueueFamilyIndices::CheckDeviceExtensionSupport(std::shared_ptr<vkhlf::PhysicalDevice>& aDevice)
  {
    vk::PhysicalDevice device = static_cast<vk::PhysicalDevice>(*aDevice);

    auto availableExtensions = device.enumerateDeviceExtensionProperties();

    std::set<std::string> requiredExtensions(sDeviceExtensions.begin(),
                                             sDeviceExtensions.end());

    for (const auto& extension : availableExtensions)
    {
      requiredExtensions.erase(extension.extensionName);
    }

    return requiredExtensions.empty();
  }

  void QueueFamilyIndices::AddRequiredExtension(char const* aExtension)
  {
    for (auto extension : sDeviceExtensions)
    {
      // Already enabled this extension.
      if (StringComparison::Equal == StringCompare(extension, aExtension))
      {
        return;
      }
    }

    sDeviceExtensions.emplace_back(aExtension);
  }



  void QueueFamilyIndices::ClearRequiredExtensions()
  {
    sDeviceExtensions.clear();
  }



  bool QueueFamilyIndices::IsComplete()
  {
    return mGraphicsFamily >= 0;
  }
}
