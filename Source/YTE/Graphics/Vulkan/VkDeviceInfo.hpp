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
    static QueueFamilyIndices FindQueueFamilies(std::shared_ptr<vkhlf::PhysicalDevice>& aDevice, vk::QueueFlags aQueueFlag);
    static void AddRequiredExtension(char const* aExtension);
    static void ClearRequiredExtensions();

    QueueFamilyIndices()
      : mGraphicsFamily(std::numeric_limits<u32>::max())
    {

    }
    
    bool IsComplete();
    bool IsDeviceSuitable(std::shared_ptr<vkhlf::PhysicalDevice>& aDevice, std::shared_ptr<vkhlf::Surface>& aSurface);
    bool CheckDeviceExtensionSupport(std::shared_ptr<vkhlf::PhysicalDevice>& aDevice);

    // Getter
    u32 GetFamily()
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
    static SwapChainSupportDetails QuerySwapChainSupport(std::shared_ptr<vkhlf::PhysicalDevice>& aDevice,
                                                         std::shared_ptr<vkhlf::Surface>& aSurface)
    {
      SwapChainSupportDetails details;

      details.mCapabilities = aDevice->getSurfaceCapabilities(aSurface);
      details.mFormats = aDevice->getSurfaceFormats(aSurface);
      details.mPresentModes = aDevice->getSurfacePresentModes(aSurface);

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
