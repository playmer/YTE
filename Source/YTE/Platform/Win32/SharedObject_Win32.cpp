#include <windows.h>

#include "YTE/Platform/SharedObject.hpp"

namespace YTE
{
  namespace PlatformData
  {
    struct SharedObject_Data
    {
      HMODULE mModule = nullptr;
      bool mLoaded = false;
    };
  }


  void SharedObject::Platform_Move(SharedObject&& aToMove)
  {
    auto self = mData.ConstructAndGet<PlatformData::SharedObject_Data>();
    auto toMoveSelf = aToMove.mData.Get<PlatformData::SharedObject_Data>();

    if (toMoveSelf)
    {
      self->mModule = toMoveSelf->mModule;
      self->mLoaded = toMoveSelf->mLoaded;

      toMoveSelf->mModule = nullptr;
      toMoveSelf->mLoaded = false;
    }
  }

  void SharedObject::Platform_LoadLibrary(std::string const& aFile)
  {
    auto self = mData.ConstructAndGet<PlatformData::SharedObject_Data>();
    auto module = LoadLibraryA(aFile.c_str());

    if (nullptr != module)
    {
      self->mModule = module;
      self->mLoaded = true;
    }
  }

  void SharedObject::Platform_UnLoadLibrary()
  {
    auto self = mData.Get<PlatformData::SharedObject_Data>();

    if (self->mLoaded)
    {
      FreeLibrary(self->mModule);
    }

    mData.Release();
  }

  void* SharedObject::Platform_GetVoidFunction(std::string const& aName)
  {
    auto self = mData.Get<PlatformData::SharedObject_Data>();

    if (false == self->mLoaded)
    {
      return nullptr;
    }

    return GetProcAddress(self->mModule, aName.c_str());
  }

  bool SharedObject::Platform_GetLoaded()
  {
    auto self = mData.Get<PlatformData::SharedObject_Data>();

    return self->mLoaded;
  }
}