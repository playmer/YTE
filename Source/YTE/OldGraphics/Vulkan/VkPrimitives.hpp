#pragma once

#include <limits>
#include <set>

#include "YTE/Core/Utilities.hpp"

#include "YTE/Graphics/ForwardDeclarations.hpp"

#include "YTE/StandardLibrary/Delegate.hpp"

#include "YTE/Graphics/ForwardDeclarations.hpp"
#include "YTE/Graphics/Vulkan/VkFunctionLoader.hpp"
#include "vulkan/vulkan.hpp"


namespace YTE
{
  class QueueFamilyIndices
  {
  public:
    QueueFamilyIndices()
      : mGraphicsFamily(std::numeric_limits<u32>::max())
    {

    }

    static QueueFamilyIndices FindQueueFamilies(vk::PhysicalDevice aDevice);
    static void AddRequiredExtension(const char *aExtension);
    static void ClearRequiredExtensions();

    bool IsComplete();
    bool IsDeviceSuitable(vk::PhysicalDevice aDevice, vk::SurfaceKHR aSurface);
    bool CheckDeviceExtensionSupport(vk::PhysicalDevice aDevice);

    u32 GetGraphicsFamily() { return mGraphicsFamily; }
  private:
    static std::vector<const char*> sDeviceExtensions;

    u32 mGraphicsFamily;
  };


  class SwapChainSupportDetails
  {
  public:
    static SwapChainSupportDetails QuerySwapChainSupport(vk::PhysicalDevice aDevice,
                                                         vk::SurfaceKHR aSurface)
    {
      SwapChainSupportDetails details;

      details.mCapabilities = aDevice.getSurfaceCapabilitiesKHR(aSurface);
      details.mFormats = aDevice.getSurfaceFormatsKHR(aSurface);
      details.mPresentModes = aDevice.getSurfacePresentModesKHR(aSurface);

      return details;
    }

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

  template <typename T>
  class VulkanDeleter
  {
    VulkanDeleter()
    {

    }

    Delegate<void(*)(T*)> mDeleter;
    T mData;
  };


  inline void vulkan_assert(u64 aFlag, const char *aMessage = "")
  {
    bool check = aFlag ? true : false;
    runtime_assert(check, aMessage);
  }

  inline void vulkan_assert(void *aFlag, const char *aMessage = "")
  {
    bool check = aFlag ? false : true;
    runtime_assert(check, aMessage);
  }

  template<typename Type>
  inline void vulkan_assert(Type aFlag, const char *aMessage = "")
  {
    bool check = static_cast<bool>(aFlag);
    runtime_assert(check, aMessage);
  }

  inline void checkVulkanResult(vk::Result &aResult, const char *aMessage)
  {
    vulkan_assert(aResult == vk::Result::eSuccess, aMessage);
  }


  inline void checkVulkanResult(VkResult &aResult, const char *aMessage)
  {
    vulkan_assert(aResult == VK_SUCCESS, aMessage);
  }


  inline glm::mat4 Rotate(glm::mat4 &aMatrix, glm::vec3 aRotation)
  {
    float xCos = cos(aRotation.x);
    float xSin = sin(aRotation.x);
    float yCos = cos(aRotation.y);
    float ySin = sin(aRotation.y);
    float zCos = cos(aRotation.z);
    float zSin = sin(aRotation.z);

    float ySin_zCos = ySin * zCos;

    glm::mat4 rotation(glm::uninitialize);
    rotation[0][0] = yCos * zCos;
    rotation[0][1] = -yCos * zSin;
    rotation[0][2] = ySin;

    rotation[1][0] = xCos * zSin + xSin * ySin_zCos;
    rotation[1][1] = xCos * zCos - xSin * ySin * zSin;
    rotation[1][2] = -xSin * yCos;

    rotation[2][0] = xSin * zSin - xCos * ySin_zCos;
    rotation[2][1] = xCos * ySin * zSin + xSin * zCos;
    rotation[2][2] = xCos * yCos;


    glm::mat4 result(glm::uninitialize);
    result[0] = aMatrix[0] * rotation[0][0] +
                aMatrix[1] * rotation[0][1] +
                aMatrix[2] * rotation[0][2];

    result[1] = aMatrix[0] * rotation[1][0] +
                aMatrix[1] * rotation[1][1] +
                aMatrix[2] * rotation[1][2];

    result[2] = aMatrix[0] * rotation[2][0] +
                aMatrix[1] * rotation[2][1] +
                aMatrix[2] * rotation[2][2];

    result[3] = aMatrix[3];

    return result;
  }

  class VkRenderer;

}