#include "YTE/StandardLibrary/Utilities.hpp"

#include "YTE/Graphics/Vulkan/VkFunctionLoader.hpp"
#include "YTE/Graphics/Vulkan/VkDeviceInfo.hpp"

namespace YTE
{
  std::vector<const char*> QueueFamilyIndices::sDeviceExtensions;



  QueueFamilyIndices QueueFamilyIndices::FindQueueFamilies(vk::PhysicalDevice aDevice)
  {
    QueueFamilyIndices indices;

    auto queueFamilyProperties = aDevice.getQueueFamilyProperties();

    u32 i = 0;
    for (const auto& queueFamily : queueFamilyProperties)
    {
      if (queueFamily.queueCount > 0 && queueFamily.queueFlags & vk::QueueFlagBits::eGraphics)
      {
        indices.mGraphicsFamily = i;
      }

      if (indices.IsComplete())
      {
        break;
      }

      i++;
    }

    return indices;
  }



  bool QueueFamilyIndices::IsDeviceSuitable(vk::PhysicalDevice aDevice, vk::SurfaceKHR aSurface)
  {
    auto indices = FindQueueFamilies(aDevice);

    bool extensionsSupported = CheckDeviceExtensionSupport(aDevice);

    bool swapChainAdequate = false;

    if (extensionsSupported)
    {
      SwapChainSupportDetails swapChainSupport = SwapChainSupportDetails::QuerySwapChainSupport(
                                                   aDevice, aSurface);
      swapChainAdequate = !swapChainSupport.Formats().empty() && 
                          !swapChainSupport.PresentModes().empty();
    }

    auto supportsSurface = aDevice.getSurfaceSupportKHR(mGraphicsFamily, aSurface);

    return indices.IsComplete() && 
           extensionsSupported && 
           swapChainAdequate &&
           supportsSurface;
  }



  bool QueueFamilyIndices::CheckDeviceExtensionSupport(vk::PhysicalDevice aDevice)
  {
    auto availableExtensions = aDevice.enumerateDeviceExtensionProperties();

    std::set<std::string> requiredExtensions(sDeviceExtensions.begin(),
                                             sDeviceExtensions.end());

    for (const auto& extension : availableExtensions)
    {
      requiredExtensions.erase(extension.extensionName);
    }

    return requiredExtensions.empty();
  }



  void QueueFamilyIndices::AddRequiredExtension(const char * aExtension)
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
