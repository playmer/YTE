#pragma once

#ifndef YTE_Graphics_Vulkan_Dx12Internals_hpp
#define YTE_Graphics_Vulkan_Dx12Internals_hpp

#include "YTE/Graphics/DirectX12/DX12FunctionLoader.hpp"
#include "YTE/Graphics/DirectX12/DX12Renderer.hpp"
#include "YTE/Graphics/DirectX12/DX12DeviceInfo.hpp"

namespace YTE
{
  class Dx12Internals
  {
  public:
    // returns the first window's surface back to Dx12Renderer
    // a surface is needed for the physical device selection
    std::shared_ptr<vkhlf::Surface> InitializeVulkan(Engine *aEngine);
    void DeinitializeVulkan();
    std::shared_ptr<vkhlf::Surface> CreateSurface(Window *aWindow);

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
