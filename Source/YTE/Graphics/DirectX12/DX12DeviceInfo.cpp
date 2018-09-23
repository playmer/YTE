#include "YTE/StandardLibrary/Utilities.hpp"

#include "YTE/Graphics/DirectX12/DX12FunctionLoader.hpp"
#include "YTE/Graphics/DirectX12/DX12DeviceInfo.hpp"

namespace YTE
{
  //std::vector<const char*> DX12QueueFamilyIndices::sDeviceExtensions;
  //
  //DX12QueueFamilyIndices DX12QueueFamilyIndices::FindQueueFamilies(vk::PhysicalDevice aDevice)
  //{
  //  DX12QueueFamilyIndices indices;
  //
  //  auto queueFamilyProperties = aDevice.getQueueFamilyProperties();
  //
  //  u32 i = 0;
  //  for (const auto& queueFamily : queueFamilyProperties)
  //  {
  //    if (queueFamily.queueCount > 0 && queueFamily.queueFlags & vk::QueueFlagBits::eGraphics)
  //    {
  //      indices.mGraphicsFamily = i;
  //    }
  //
  //    if (indices.IsComplete())
  //    {
  //      break;
  //    }
  //
  //    i++;
  //  }
  //
  //  return indices;
  //}
  //
  //
  //
  //bool DX12QueueFamilyIndices::IsDeviceSuitable(vk::PhysicalDevice aDevice, vk::SurfaceKHR aSurface)
  //{
  //  auto indices = FindQueueFamilies(aDevice);
  //
  //  bool extensionsSupported = CheckDeviceExtensionSupport(aDevice);
  //
  //  bool swapChainAdequate = false;
  //
  //  if (extensionsSupported)
  //  {
  //    DX12SwapChainSupportDetails swapChainSupport = DX12SwapChainSupportDetails::QuerySwapChainSupport(
  //                                                 aDevice, aSurface);
  //    swapChainAdequate = !swapChainSupport.Formats().empty() && 
  //                        !swapChainSupport.PresentModes().empty();
  //  }
  //
  //  auto supportsSurface = aDevice.getSurfaceSupportKHR(mGraphicsFamily, aSurface);
  //
  //  return indices.IsComplete() && 
  //         extensionsSupported && 
  //         swapChainAdequate &&
  //         supportsSurface;
  //}
  //
  //
  //
  //bool DX12QueueFamilyIndices::CheckDeviceExtensionSupport(vk::PhysicalDevice aDevice)
  //{
  //  auto availableExtensions = aDevice.enumerateDeviceExtensionProperties();
  //
  //  std::set<std::string> requiredExtensions(sDeviceExtensions.begin(),
  //                                           sDeviceExtensions.end());
  //
  //  for (const auto& extension : availableExtensions)
  //  {
  //    requiredExtensions.erase(extension.extensionName);
  //  }
  //
  //  return requiredExtensions.empty();
  //}
  //
  //
  //
  //void DX12QueueFamilyIndices::AddRequiredExtension(const char * aExtension)
  //{
  //  for (auto extension : sDeviceExtensions)
  //  {
  //    // Already enabled this extension.
  //    if (StringComparison::Equal == StringCompare(extension, aExtension))
  //    {
  //      return;
  //    }
  //  }
  //
  //  sDeviceExtensions.emplace_back(aExtension);
  //}
  //
  //
  //
  //void DX12QueueFamilyIndices::ClearRequiredExtensions()
  //{
  //  sDeviceExtensions.clear();
  //}
  //
  //
  //
  //bool DX12QueueFamilyIndices::IsComplete()
  //{
  //  return mGraphicsFamily >= 0;
  //}
  //
  //
  //
  namespace DX12AllocatorTypes
  {
    const std::string Mesh{ "Mesh" };
    const std::string Texture{ "Texture" };
    const std::string UniformBufferObject{ "UniformBufferObject" };
    const std::string SpriteInstances{ "SpriteInstances" };
  }
}
