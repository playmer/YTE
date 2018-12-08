///////////////////
// Author: Andrew Griffin
// YTE - Graphics - Vulkan
///////////////////

#pragma once

#ifndef YTE_Graphics_Vulkan_VkUtils_hpp
#define YTE_Graphics_Vulkan_VkUtils_hpp

#include "YTE/Graphics/Vulkan/VkFunctionLoader.hpp"

#include "YTE/StandardLibrary/Utilities.hpp"

namespace YTE
{
  class QueueFamilyIndices
  {
  public:
    // statics
    static QueueFamilyIndices FindQueueFamilies(vk::PhysicalDevice aDevice);
    static void AddRequiredExtension(const char *aExtension);
    static void ClearRequiredExtensions();

    QueueFamilyIndices()
      : mGraphicsFamily(std::numeric_limits<u32>::max())
    {

    }
    
    bool IsComplete();
    bool IsDeviceSuitable(vk::PhysicalDevice aDevice, vk::SurfaceKHR aSurface);
    bool CheckDeviceExtensionSupport(vk::PhysicalDevice aDevice);

    // Getter
    u32 GetGraphicsFamily()
    {
      return mGraphicsFamily;
    }

  private:
    static std::vector<const char*> sDeviceExtensions;
    u32 mGraphicsFamily;
  };



  class SwapChainSupportDetails
  {
  public:
    // statics
    static SwapChainSupportDetails QuerySwapChainSupport(vk::PhysicalDevice aDevice,
                                                         vk::SurfaceKHR aSurface)
    {
      SwapChainSupportDetails details;

      details.mCapabilities = aDevice.getSurfaceCapabilitiesKHR(aSurface);
      details.mFormats = aDevice.getSurfaceFormatsKHR(aSurface);
      details.mPresentModes = aDevice.getSurfacePresentModesKHR(aSurface);

      return details;
    }


    // Gettors
    vk::SurfaceCapabilitiesKHR& Capabilities()
    {
      return mCapabilities;
    }

    std::vector<vk::SurfaceFormatKHR>& Formats()
    {
      return mFormats;
    }

    std::vector<vk::PresentModeKHR>& PresentModes()
    {
      return mPresentModes;
    }



  private:
    vk::SurfaceCapabilitiesKHR mCapabilities;
    std::vector<vk::SurfaceFormatKHR> mFormats;
    std::vector<vk::PresentModeKHR> mPresentModes;
  };
}

#endif
