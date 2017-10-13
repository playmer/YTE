#include <set>

#include "YTE/Core/Utilities.hpp"

#include "YTE/Graphics/ForwardDeclarations.hpp"

#include "YTE/StandardLibrary/Delegate.hpp"

#include "YTE/Graphics/ForwardDeclarations.hpp"
#include "YTE/Graphics/Vulkan/VkFunctionLoader.hpp"
#include "vulkan/vulkan.hpp"

namespace YTE
{
  namespace AllocatorTypes
  {
    extern std::string Mesh{ "Mesh" };
    extern std::string Texture{ "Texture" };
    extern std::string UniformBufferObject{ "UniformBufferObject" };
  }

  static VKAPI_ATTR
    VkBool32 VKAPI_CALL debugReportCallback(VkDebugReportFlagsEXT flags,
      VkDebugReportObjectTypeEXT objectType,
      uint64_t object,
      size_t location,
      int32_t messageCode,
      const char* pLayerPrefix,
      const char* pMessage,
      void* pUserData)
  {
    switch (flags)
    {
    case VK_DEBUG_REPORT_INFORMATION_BIT_EXT:
    {
      printf("INFORMATION: %s\n", pMessage);
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
      printf("ERROR: ");
      break;
    }
    case VK_DEBUG_REPORT_DEBUG_BIT_EXT:
    {
      printf("DEBUG: %s\n", pMessage);
      return VK_FALSE;
    }
    default:
    {
      printf("Unknown Flag (%u): ", flags);
      break;
    }
    }
    printf("%s\n", pMessage);
    assert(!pMessage);
    return VK_TRUE;
  }
}