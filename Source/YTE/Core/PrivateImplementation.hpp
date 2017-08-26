//////////////////////////////////////////////
// Author: Joshua T. Fisher
//////////////////////////////////////////////
#pragma once

#ifndef PrivateImplementation_hpp
#define PrivateImplementation_hpp

#include <memory>

#include "YTE/Core/Utilities.hpp"

namespace YTE
{
  // Allows the private implementation of an object allocated within a size
  // given as a template parameter. This allows the storage of the object to
  // reside on the stack or within another struct/class.
  template <int SizeInBytes>
  class PrivateImplementationLocal
  {
  public:
    using Destructor = void(*)(byte*);

    PrivateImplementationLocal() : mDestructor(GenericDoNothing) {}

    ~PrivateImplementationLocal()
    {
      // Destruct our data if it's already been constructed.
      Release();
    }

    void Release()
    {
      mDestructor(mMemory);
      mDestructor = GenericDoNothing;
    }

    template <typename T, typename... Arguments>
    T* ConstructAndGet(Arguments &&...aArguments)
    {
      static_assert(sizeof(T) < SizeInBytes, 
                    "Constructed Type must be smaller than our size.");

      // Destruct any undestructed object.
      mDestructor(mMemory);

      // Capture the destructor of the new type.
      mDestructor = GenericDestruct<T>;

      // Create a T in our local memory by forwarding any provided arguments.
      new (mMemory) T(std::forward<Arguments &&>(aArguments)...);
      
      return reinterpret_cast<T*>(mMemory);
    }

    template <typename T>
    T* Get()
    {
      return reinterpret_cast<T*>(mMemory);
    }

  private:

    byte mMemory[SizeInBytes];
    Destructor mDestructor;
  };

  // Allows the private implementation of an object allocated on the heap.
  class PrivateImplementationDynamic
  {
  public:
    using Destructor = void(*)(byte*);
    using Storage = std::unique_ptr<byte[], Destructor>;

    PrivateImplementationDynamic() : mMemory(nullptr, GenericDoNothing) {}

    ~PrivateImplementationDynamic()
    {
    }

    PrivateImplementationDynamic(PrivateImplementationDynamic &&aRight)
    : mMemory(std::move(aRight.mMemory))
    {
      
    }

    void Release()
    {
      mMemory.release();
      mMemory = Storage(nullptr, GenericDoNothing);
    }

    template <typename T, typename... Arguments>
    T* ConstructAndGet(Arguments &&...aArguments)
    {
      // Destruct our data if it's already been constructed.
      mMemory.release();

      // Create a new T by forwarding any provided constructor arguments and
      // store the destructor.
      T *tPtr = new T(std::forward<Arguments &&>(aArguments)...);

      mMemory = Storage(reinterpret_cast<byte*>(tPtr), GenericDestructByte<T>);

      return reinterpret_cast<T*>(mMemory.get());
    }

    template <typename T>
    T* Get()
    {
      return reinterpret_cast<T*>(mMemory.get());
    }

  private:
    Storage mMemory;
  };
}
#endif
