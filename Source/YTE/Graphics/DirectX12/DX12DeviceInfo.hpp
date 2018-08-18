#pragma once

#ifndef YTE_Graphics_Vulkan_VkUtils_hpp
#define YTE_Graphics_Vulkan_VkUtils_hpp

#include "YTE/Graphics/DirectX12/DX12FunctionLoader.hpp"

#include "YTE/StandardLibrary/Utilities.hpp"

namespace YTE
{
  //class DX12QueueFamilyIndices
  //{
  //public:
  //  // statics
  //  static DX12QueueFamilyIndices FindQueueFamilies(vk::PhysicalDevice aDevice);
  //  static void AddRequiredExtension(const char *aExtension);
  //  static void ClearRequiredExtensions();
  //
  //  DX12QueueFamilyIndices()
  //    : mGraphicsFamily(std::numeric_limits<u32>::max())
  //  {
  //
  //  }
  //  
  //  bool IsComplete();
  //  bool IsDeviceSuitable(vk::PhysicalDevice aDevice, vk::SurfaceKHR aSurface);
  //  bool CheckDeviceExtensionSupport(vk::PhysicalDevice aDevice);
  //
  //  // Getter
  //  u32 GetGraphicsFamily()
  //  {
  //    return mGraphicsFamily;
  //  }
  //
  //private:
  //  static std::vector<const char*> sDeviceExtensions;
  //  u32 mGraphicsFamily;
  //};
  //
  //class DX12SwapChainSupportDetails
  //{
  //public:
  //  // statics
  //  static DX12SwapChainSupportDetails QuerySwapChainSupport(vk::PhysicalDevice aDevice,
  //                                                       vk::SurfaceKHR aSurface)
  //  {
  //    DX12SwapChainSupportDetails details;
  //
  //    details.mCapabilities = aDevice.getSurfaceCapabilitiesKHR(aSurface);
  //    details.mFormats = aDevice.getSurfaceFormatsKHR(aSurface);
  //    details.mPresentModes = aDevice.getSurfacePresentModesKHR(aSurface);
  //
  //    return details;
  //  }
  //
  //
  //  // Gettors
  //  vk::SurfaceCapabilitiesKHR& Capabilities()
  //  {
  //    return mCapabilities;
  //  }
  //
  //  std::vector<vk::SurfaceFormatKHR>& Formats()
  //  {
  //    return mFormats;
  //  }
  //
  //  std::vector<vk::PresentModeKHR>& PresentModes()
  //  {
  //    return mPresentModes;
  //  }
  //
  //
  //
  //private:
  //  vk::SurfaceCapabilitiesKHR mCapabilities;
  //  std::vector<vk::SurfaceFormatKHR> mFormats;
  //  std::vector<vk::PresentModeKHR> mPresentModes;
  //};
  //
  //
  //
  namespace DX12AllocatorTypes
  {
    extern const std::string Mesh;
    extern const std::string Texture;
    extern const std::string UniformBufferObject;
    extern const std::string SpriteInstances;
  }
}

#endif
