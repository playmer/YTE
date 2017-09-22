/******************************************************************************/
/*!
 * \author Joshua T. Fisher
 * \date   2015-09-19
 *
 * \copyright All content 2016 DigiPen (USA) Corporation, all rights reserved.
 */
/******************************************************************************/
#include <stdlib.h>
#include "YTE/Platform/Windows/WindowsInclude_Windows.hpp"

#include "AK/SoundEngine/Common/AkMemoryMgr.h"    // Memory Manager

// Custom alloc/free functions. These are declared as "extern" in AkMemoryMgr.h
// and MUST be defined by the game developer.
namespace AK
{
#ifdef WIN32
  void * AllocHook(size_t in_size)
  {
    return malloc(in_size);
  }
  void FreeHook(void * in_ptr)
  {
    free(in_ptr);
  }

  // Note: VirtualAllocHook() may be used by I/O pools of the default implementation
  // of the Stream Manager, to allow "true" unbuffered I/O (using FILE_FLAG_NO_BUFFERING
  // - refer to the Windows SDK documentation for more details). This is NOT mandatory;
  // you may implement it with a simple malloc().
  void * VirtualAllocHook(void * in_pMemAddress, size_t in_size, DWORD in_dwAllocationType,
    DWORD in_dwProtect)
  {
    return VirtualAlloc(in_pMemAddress, in_size, in_dwAllocationType, in_dwProtect);
  }

  void VirtualFreeHook(void * in_pMemAddress, size_t in_size, DWORD in_dwFreeType)
  {
    #pragma warning( push )
    #pragma warning( disable : 28160 )
    VirtualFree(in_pMemAddress, in_size, in_dwFreeType);
    #pragma warning( pop ) 
  }
#endif
}