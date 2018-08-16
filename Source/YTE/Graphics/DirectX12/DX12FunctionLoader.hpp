#pragma once

#define NOMINMAX
#define DIRECTINPUT_VERSION 0x0800
#define WIN32_LEAN_AND_MEAN 1
#define VC_EXTRALEAN

#ifdef WIN32
 #define VK_USE_PLATFORM_WIN32_KHR
#endif

#include "vulkan/vulkan.h"

#if defined(MemoryBarrier)
  #undef MemoryBarrier
#endif

#undef NOMINMAX
#undef DIRECTINPUT_VERSION
#undef WIN32_LEAN_AND_MEAN
#undef VC_EXTRALEAN

#include "vulkan/vulkan.hpp"

#if defined(MemoryBarrier)
  #undef MemoryBarrier
#endif

#undef NOMINMAX
#undef DIRECTINPUT_VERSION
#undef WIN32_LEAN_AND_MEAN
#undef VC_EXTRALEAN

#include "vkhlf/vkhlf.h"

#if defined(MemoryBarrier)
  #undef MemoryBarrier
#endif

#undef NOMINMAX
#undef DIRECTINPUT_VERSION
#undef WIN32_LEAN_AND_MEAN
#undef VC_EXTRALEAN
