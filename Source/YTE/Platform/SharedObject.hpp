#pragma once

#include <string>

#include "YTE/StandardLibrary/PrivateImplementation.hpp"

namespace YTE
{
  struct SharedObject
  {
    SharedObject(std::string const& aFile)
    {
      Platform_LoadLibrary(aFile);
      mFile = aFile;
    }

    SharedObject(SharedObject&& aToMove)
      : mFile{std::move(aToMove.mFile)}
    {
      Platform_Move(std::move(aToMove));
    }

    void operator=(SharedObject&& aToMove)
    {
      mFile = std::move(aToMove.mFile);
      Platform_Move(std::move(aToMove));
    }

    ~SharedObject()
    {
      Platform_UnLoadLibrary();
    }

    void ReloadLibrary()
    {
      if (GetLoaded())
      {
        Platform_UnLoadLibrary();
      }

      Platform_LoadLibrary(mFile);
    }

    void UnloadLibrary()
    {
      Platform_UnLoadLibrary();
    }

    void* GetVoidFunction(std::string const& aName)
    {
      return Platform_GetVoidFunction(aName);
    }

    template <typename tFunctionType>
    tFunctionType GetFunction(std::string const& aName)
    {
      return reinterpret_cast<tFunctionType>(GetVoidFunction(aName));
    }

    template <typename tFunctionType>
    void GetFunction(std::string const& aName, tFunctionType& aFunction)
    {
      aFunction = GetFunction<tFunctionType>(aName);
    }

    bool GetLoaded()
    {
      return Platform_GetLoaded();
    }

    private:
    void Platform_Move(SharedObject&& aToMove);
    void Platform_LoadLibrary(std::string const& aFile);
    void Platform_UnLoadLibrary();
    void* Platform_GetVoidFunction(std::string const& aName);
    bool Platform_GetLoaded();

    PrivateImplementationLocal<32> mData;
    std::string mFile;
  };
}
