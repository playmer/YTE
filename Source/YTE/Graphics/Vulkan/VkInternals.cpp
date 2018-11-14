///////////////////
// Author: Andrew Griffin
// YTE - Graphics - Vulkan
///////////////////

#include "YTE/Core/Engine.hpp"

#include "YTE/Graphics/Vulkan/VkFunctionLoader.hpp"
#include "YTE/Graphics/Vulkan/VkInternals.hpp"


namespace YTE
{
  // debug report callback for vulkan
  static VKAPI_ATTR
  VkBool32 VKAPI_CALL debugReportCallback(VkDebugReportFlagsEXT aFlags,
                                          VkDebugReportObjectTypeEXT aObjectType,
                                          uint64_t aObject,
                                          size_t aLocation,
                                          int32_t aMessageCode,
                                          const char* aLayerPrefix,
                                          const char* aMessage,
                                          void *aUserData)
  {
    UnusedArguments(aObjectType, aObject, aLocation, aMessageCode, aLayerPrefix, aUserData, aObjectType);

    switch (aFlags)
    {
      case VK_DEBUG_REPORT_INFORMATION_BIT_EXT:
      {
        printf("INFORMATION: %s\n", aMessage);
        return VK_FALSE;
      }
      case VK_DEBUG_REPORT_WARNING_BIT_EXT:
      {
        printf("WARNING: ");
        break;
      }
      case VK_DEBUG_REPORT_PERFORMANCE_WARNING_BIT_EXT:
      {
        printf("PERFORMANCE WARNING: ");
        break;
      }
      case VK_DEBUG_REPORT_ERROR_BIT_EXT:
      {
        printf("\n\nERROR: ");
        break;
      }
      case VK_DEBUG_REPORT_DEBUG_BIT_EXT:
      {
        printf("DEBUG: %s\n", aMessage);
        return VK_FALSE;
      }
      default:
      {
        printf("Unknown Flag (%u): ", aFlags);
        break;
      }
    }

    printf("%s\n", aMessage);
    assert(false == (aFlags & VK_DEBUG_REPORT_ERROR_BIT_EXT));
    return VK_TRUE;
  }


  std::shared_ptr<vkhlf::Surface> VkInternals::InitializeVulkan(Engine *aEngine)
  {
    std::vector<std::string>  enabledExtensions, enabledLayers;

    enabledExtensions.emplace_back("VK_KHR_surface");

    if constexpr(PlatformInformation::Windows())
    {
      enabledExtensions.emplace_back("VK_KHR_win32_surface");
    }

    if constexpr(CompilerOptions::Debug())
    {
      enabledExtensions.emplace_back("VK_EXT_debug_report");

      // Enable standard validation layer to find as much errors as possible!
      enabledLayers.push_back("VK_LAYER_LUNARG_standard_validation");

      //enabledLayers.push_back("VK_LAYER_GOOGLE_threading");
      //enabledLayers.push_back("VK_LAYER_GOOGLE_unique_objects");
      //enabledLayers.push_back("VK_LAYER_LUNARG_api_dump");
      //enabledLayers.push_back("VK_LAYER_LUNARG_device_limits");
      //enabledLayers.push_back("VK_LAYER_LUNARG_draw_state");
      //enabledLayers.push_back("VK_LAYER_LUNARG_image");
      //enabledLayers.push_back("VK_LAYER_LUNARG_mem_tracker");
      //enabledLayers.push_back("VK_LAYER_LUNARG_object_tracker");
      //enabledLayers.push_back("VK_LAYER_LUNARG_param_checker");
      //enabledLayers.push_back("VK_LAYER_LUNARG_screenshot");
      //enabledLayers.push_back("VK_LAYER_LUNARG_swapchain");
    }

    // Create a new vulkan instance using the required extensions
    mInstance = vkhlf::Instance::create("Yours Truly Engine",
                                        1,
                                        enabledLayers,
                                        enabledExtensions);

    if constexpr(CompilerOptions::Debug())
    {
      vk::DebugReportFlagsEXT flags(//vk::DebugReportFlagBitsEXT::eInformation
                                    vk::DebugReportFlagBitsEXT::eWarning |
                                    vk::DebugReportFlagBitsEXT::ePerformanceWarning |
                                    vk::DebugReportFlagBitsEXT::eError |
                                    vk::DebugReportFlagBitsEXT::eDebug);

      mDebugReportCallback = mInstance->createDebugReportCallback(flags,
                                                                  &debugReportCallback);
  }

    auto &windows = aEngine->GetWindows();

    // get the surface to the first window
    std::shared_ptr<vkhlf::Surface> surface;
    for (auto &window : windows)
    {
      if (window.second->mShouldBeRenderedTo)
      {
        surface = std::any_cast<std::shared_ptr<vkhlf::Surface>>(window.second->SetUpVulkanWindow(static_cast<void*>(mInstance.get())));
        break;
      }
    }

    SelectDevice(surface);

    // initialize physical devices
    auto baseDevice = static_cast<vk::PhysicalDevice>(*mMainDevice);

    mQueueFamilyIndices = QueueFamilyIndices::FindQueueFamilies(baseDevice);

    auto family = mQueueFamilyIndices.GetGraphicsFamily();
    vkhlf::DeviceQueueCreateInfo deviceCreate{ family, 0.0f };

    return surface;
  }



  void VkInternals::DeinitializeVulkan()
  {
  }



  std::shared_ptr<vkhlf::Surface> VkInternals::CreateSurface(Window *aWindow)
  {
    auto surface = std::any_cast<std::shared_ptr<vkhlf::Surface>>(aWindow->SetUpVulkanWindow(static_cast<void*>(mInstance.get())));
    vk::SurfaceKHR baseSurfaceKhr = static_cast<vk::SurfaceKHR>(*surface);
    auto indices = QueueFamilyIndices::FindQueueFamilies(*mMainDevice);

    if (indices.IsDeviceSuitable(*mMainDevice, baseSurfaceKhr))
    {
      return surface;
    }

    return nullptr;
  }



  void VkInternals::SelectDevice(std::shared_ptr<vkhlf::Surface> aSurface)
  {
    // Find a physical device with presentation support
    DebugObjection(mInstance->getPhysicalDeviceCount() == 0,
      "We can't find any graphics devices!");

    QueueFamilyIndices::AddRequiredExtension("VK_KHR_swapchain");

    auto count = mInstance->getPhysicalDeviceCount();

    for (size_t i = 0; i < count; ++i)
    {
      auto device = mInstance->getPhysicalDevice(i);
      auto baseDevice = static_cast<vk::PhysicalDevice>(*device);
      vk::SurfaceKHR baseSurfaceKhr = static_cast<vk::SurfaceKHR>(*aSurface);

      auto indices = QueueFamilyIndices::FindQueueFamilies(baseDevice);

      if (indices.IsDeviceSuitable(baseDevice, baseSurfaceKhr))
      {
        mPhysicalDevices.emplace_back(device);
      }

      PrintDeviceProperties(device);
    }

    FindDeviceOfType(vk::PhysicalDeviceType::eDiscreteGpu);

    // If we can't find a Discrete GPU, select one of the others by default.
    if (false == mMainDevice)
    {
      FindDeviceOfType(vk::PhysicalDeviceType::eIntegratedGpu);
    }

    if (false == mMainDevice)
    {
      FindDeviceOfType(vk::PhysicalDeviceType::eVirtualGpu);
    }

    if (false == mMainDevice)
    {
      FindDeviceOfType(vk::PhysicalDeviceType::eCpu);
    }

    if (false == mMainDevice)
    {
      FindDeviceOfType(vk::PhysicalDeviceType::eOther);
    }

    DebugObjection(mMainDevice == false, "We can't find a suitible graphics device!");

    printf("Chosen Device: %s\n", mMainDevice->getProperties().deviceName);
  }



  void VkInternals::FindDeviceOfType(vk::PhysicalDeviceType aType)
  {
    for (auto &device : mPhysicalDevices)
    {
      if (device->getProperties().deviceType == aType)
      {
        mMainDevice = device;
      }
    }
  }



  void VkInternals::PrintDeviceProperties(std::shared_ptr<vkhlf::PhysicalDevice> &aDevice)
  {
    auto props = aDevice->getProperties();

    printf("\nDevice:");

    printf("  Device Name: %s\n", props.deviceName);
    printf("  Device ID: %u\n", props.deviceID);
    printf("  Driver Version: %u\n", props.driverVersion);
    printf("  Vendor ID: %u\n", props.vendorID);

    auto type = props.deviceType;

    switch (type)
    {
      case vk::PhysicalDeviceType::eOther:
      {
        printf("  Device Type: Other\n");
        break;
      }

      case vk::PhysicalDeviceType::eIntegratedGpu:
      {
        printf("  Device Type: Integrated GPU\n");
        break;
      }

      case vk::PhysicalDeviceType::eDiscreteGpu:
      {
        printf("  Device Type: Discrete GPU\n");
        break;
      }

      case vk::PhysicalDeviceType::eVirtualGpu:
      {
        printf("  Device Type: Virtual GPU\n");
        break;
      }

      case vk::PhysicalDeviceType::eCpu:
      {
        printf("  Device Type: CPU\n");
        break;
      }
    }

    // TODO (Josh): Print/Store device limits.
    //auto limits = props.limits;

    printf("  API Version: %u\n", props.apiVersion);
  }
}
