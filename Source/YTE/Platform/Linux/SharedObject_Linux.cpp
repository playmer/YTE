#include "YTE/Platform/SharedObject.hpp"

namespace YTE
{
  namespace PlatformData
  {
    struct SharedObject_Data
    {
    };
  }


  void SharedObject::Platform_Move(SharedObject&& aToMove)
  {
  }

  void SharedObject::Platform_LoadLibrary(std::string const& aFile)
  {
  }

  void SharedObject::Platform_UnLoadLibrary()
  {
  }

  void* SharedObject::Platform_GetVoidFunction(std::string const& aName)
  {
    return nullptr;
  }

  bool SharedObject::Platform_GetLoaded()
  {
    return false;
  }
}