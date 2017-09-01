#pragma once

#define NOMINMAX
#define DIRECTINPUT_VERSION 0x0800
#define WIN32_LEAN_AND_MEAN 1
#define VC_EXTRALEAN

#include "vkel/vkel.h"

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
