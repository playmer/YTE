#pragma once

#ifndef PrivateImplementation_hpp
#define PrivateImplementation_hpp

#include <memory>
#include <new>

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
    using Destructor = decltype(GenericDestructByte<byte>)*;

    PrivateImplementationLocal() : mDestructor(GenericDestructByte<byte>) {}

    ~PrivateImplementationLocal()
    {
      // Destruct our data if it's already been constructed.
      Release();
    }

    void Release()
    {
      mDestructor(mMemory);
      mDestructor = GenericDestructByte<byte>;
    }

    template <typename tType, typename... tArguments>
    tType* ConstructAndGet(tArguments &&...aArguments)
    {
      static_assert(sizeof(tType) < SizeInBytes,
                    "Constructed Type must be smaller than our size.");

      // Destruct any undestructed object.
      mDestructor(mMemory);

      // Capture the destructor of the new type.
      mDestructor = GenericDestructByte<tType>;

      // Create a T in our local memory by forwarding any provided arguments.
      new (mMemory) tType(std::forward<tArguments &&>(aArguments)...);
      
      return Get<tType>();
    }

    template <typename tType>
    tType* Get()
    {
      if (&GenericDestructByte<tType> != mDestructor)
      {
        return nullptr;
      }

      return std::launder<tType>(reinterpret_cast<tType*>(mMemory));
    }

  private:

    byte mMemory[SizeInBytes];
    Destructor mDestructor;
  };

  // Allows the private implementation of an object allocated on the heap.
  class PrivateImplementationDynamic
  {
    public:
    using Destructor = decltype(GenericDestructByte<byte>)*;
    using Storage = std::unique_ptr<byte[], Destructor>;

    PrivateImplementationDynamic()
      : mMemory(nullptr, GenericDestructByte<byte>)
    {
    }

    PrivateImplementationDynamic(PrivateImplementationDynamic &&aRight)
      : mMemory(std::move(aRight.mMemory))
    {

    }

    ~PrivateImplementationDynamic()
    {
    }

    void Release()
    {
      //mMemory.release();
      mMemory = Storage(nullptr, GenericDestructByte<byte>);
    }

    template <typename tType, typename... tArguments>
    tType* ConstructAndGet(tArguments &&...aArguments)
    {
        // Destruct our data if it's already been constructed.
        mMemory.release();

        // Create a new T by forwarding any provided constructor arguments and
        // store the destructor.
        tType* tPtr = new tType(std::forward<tArguments&&>(aArguments)...);

        mMemory = Storage(reinterpret_cast<byte*>(tPtr), GenericDestructByte<tType>);

        return reinterpret_cast<tType*>(mMemory.get());
    }

    template <typename tType>
    tType* Get()
    {
        return reinterpret_cast<tType*>(mMemory.get());
    }

  private:
      Storage mMemory;
  };
}
#endif
