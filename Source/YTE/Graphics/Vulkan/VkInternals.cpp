#include "YTE/Core/Engine.hpp"

#include "YTE/Graphics/Vulkan/VkFunctionLoader.hpp"
#include "YTE/Graphics/Vulkan/VkInternals.hpp"


namespace YTE
{
  constexpr bool cVulkanValidations = true;

  LogType ToYTE(vk::DebugUtilsMessageSeverityFlagBitsEXT aSeverity)
  {
    switch (aSeverity)
    {
      case vk::DebugUtilsMessageSeverityFlagBitsEXT::eVerbose: return LogType::Information;
      case vk::DebugUtilsMessageSeverityFlagBitsEXT::eInfo: return LogType::Information;
      case vk::DebugUtilsMessageSeverityFlagBitsEXT::eWarning: return LogType::Warning;
      case vk::DebugUtilsMessageSeverityFlagBitsEXT::eError : return LogType::Error;
    }
  
    return LogType::Information;
  }
  
  // Debug Utils callback for vulkan
  VkBool32 VKAPI_PTR debugUtilsCallback(
    VkDebugUtilsMessageSeverityFlagBitsEXT messageSeverity,
    VkDebugUtilsMessageTypeFlagsEXT messageTypes,
    const VkDebugUtilsMessengerCallbackDataEXT* aCallbackData,
    void* aUserData)
  {
    //UnusedArguments(aObjectType, aObject, aLocation, aMessageCode, aLayerPrefix, aUserData, aObjectType);
    auto engine = static_cast<Engine*>(aUserData);
  
    vk::DebugUtilsMessageSeverityFlagBitsEXT severityFlag = static_cast<vk::DebugUtilsMessageSeverityFlagBitsEXT>(messageSeverity);
    vk::DebugUtilsMessageTypeFlagsEXT typeFlag = static_cast<vk::DebugUtilsMessageTypeFlagsEXT>(messageTypes);
  
    auto text = fmt::format(
      "Vulkan Layer {} Report, Severity {}:\n\t: {}", 
      vk::to_string(typeFlag), 
      vk::to_string(severityFlag),
      aCallbackData->pMessage);
  
    engine->Log(ToYTE(severityFlag), text);
  
    if (vk::DebugUtilsMessageSeverityFlagBitsEXT::eError == severityFlag)
    {
      // If you're hitting this and it's not causing an issue, you should probably turn off
      // cVulkanValidations, apologies that it's on. If it is causing an issue and it's beyond
      // you, contact Joshua T. Fisher.
      __debugbreak();
      assert(false);
    }
  
    return VK_TRUE;
  }

  std::shared_ptr<vkhlf::Surface> VkInternals::InitializeVulkan(Engine *aEngine)
  {
    OPTICK_EVENT();

    std::vector<std::string>  enabledExtensions, enabledLayers;

    enabledExtensions.emplace_back("VK_KHR_surface");

    if constexpr(PlatformInformation::Windows())
    {
      enabledExtensions.emplace_back("VK_KHR_win32_surface");
    }

    if constexpr(CompilerOptions::Debug() && cVulkanValidations)
    {
      enabledExtensions.emplace_back("VK_EXT_debug_report");

      // Enable standard validation layer to find as many errors as possible!
      enabledLayers.push_back("VK_LAYER_KHRONOS_validation");
    }

    // Create a new vulkan instance using the required extensions
    {
      OPTICK_EVENT("Creating Vulkan Instance");
      mInstance = vkhlf::Instance::create("Yours Truly Engine",
                                          1,
                                          enabledLayers,
                                          enabledExtensions);
    }
    
    if (aEngine->GetConfig().ValidationLayers)
    {
      constexpr auto severityFlags =
        vk::DebugUtilsMessageSeverityFlagBitsEXT::eVerbose |
        vk::DebugUtilsMessageSeverityFlagBitsEXT::eInfo |
        vk::DebugUtilsMessageSeverityFlagBitsEXT::eWarning |
        vk::DebugUtilsMessageSeverityFlagBitsEXT::eError;
      
      constexpr auto messageTypes =
        vk::DebugUtilsMessageTypeFlagBitsEXT::eGeneral |
        vk::DebugUtilsMessageTypeFlagBitsEXT::eValidation |
        vk::DebugUtilsMessageTypeFlagBitsEXT::ePerformance;
      
      vk::DebugUtilsMessengerCreateInfoEXT debugUtilsCreate(
        vk::DebugUtilsMessengerCreateFlagsEXT(),
        severityFlags,
        messageTypes,
        debugUtilsCallback,
        static_cast<void*>(aEngine)
      );
      
      mDebugUtilsMessenger = mInstance->createDebugUtilsMessenger(debugUtilsCreate);
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

    SelectDevice(aEngine, surface);

    // initialize physical devices
    auto baseDevice = static_cast<vk::PhysicalDevice>(*mMainDevice);

    return surface;
  }



  void VkInternals::DeinitializeVulkan()
  {
  }



  std::shared_ptr<vkhlf::Surface> VkInternals::CreateSurface(Window *aWindow)
  {
    OPTICK_EVENT();

    auto surface = std::any_cast<std::shared_ptr<vkhlf::Surface>>(aWindow->SetUpVulkanWindow(static_cast<void*>(mInstance.get())));
    auto indices = QueueFamilyIndices::FindQueueFamilies(mMainDevice, vk::QueueFlagBits::eGraphics);

    if (indices.IsDeviceSuitable(mMainDevice, surface))
    {
      return surface;
    }

    return nullptr;
  }



  void VkInternals::SelectDevice(Engine* aEngine, std::shared_ptr<vkhlf::Surface> aSurface)
  {
    OPTICK_EVENT();

    // Find a physical device with presentation support
    DebugObjection(mInstance->getPhysicalDeviceCount() == 0,
      "We can't find any graphics devices!");

    QueueFamilyIndices::AddRequiredExtension("VK_KHR_swapchain");

    auto count = mInstance->getPhysicalDeviceCount();

    for (size_t i = 0; i < count; ++i)
    {
      auto device = mInstance->getPhysicalDevice(i);

      auto indices = QueueFamilyIndices::FindQueueFamilies(device, vk::QueueFlagBits::eGraphics);

      if (indices.IsDeviceSuitable(device, aSurface))
      {
        mPhysicalDevices.emplace_back(device);
      }

      PrintDeviceProperties(device);
    }

    FindDeviceOfType(vk::PhysicalDeviceType::eDiscreteGpu);

    // If we can't find a Discrete GPU, select one of the others by default.
    if (nullptr == mMainDevice)
    {
      FindDeviceOfType(vk::PhysicalDeviceType::eIntegratedGpu);
    }

    if (nullptr == mMainDevice)
    {
      FindDeviceOfType(vk::PhysicalDeviceType::eVirtualGpu);
    }

    if (nullptr == mMainDevice)
    {
      FindDeviceOfType(vk::PhysicalDeviceType::eCpu);
    }

    if (nullptr == mMainDevice)
    {
      FindDeviceOfType(vk::PhysicalDeviceType::eOther);
    }

    std::string const& deviceName = aEngine->GetConfig().PreferredGpu;

    auto it = std::find_if(mPhysicalDevices.begin(), mPhysicalDevices.end(), [&deviceName](std::shared_ptr<vkhlf::PhysicalDevice> aDevice)
    {
      auto props = aDevice->getProperties();
      if (deviceName == props.deviceName.data())
        return true;

      return false;
    });

    if (it != mPhysicalDevices.end())
      mMainDevice = *it;


    DebugObjection(nullptr == mMainDevice, "We can't find a suitible graphics device!");

    printf("Chosen Device: %s\n", mMainDevice->getProperties().deviceName.data());
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

    printf("  Device Name: %s\n", props.deviceName.data());
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
