#pragma once

#ifndef YTE_ComponentFactory_h
#define YTE_ComponentFactory_h

#include <memory>

#include "YTE/Core/EventHandler.hpp"

#include "YTE/Core/ForwardDeclarations.hpp"
#include "YTE/Core/Utilities.hpp"

#include "YTE/StandardLibrary/PrivateImplementation.hpp"

namespace YTE
{
  namespace Detail 
  {
    class ComponentDeleter
    {
    public:
      template <typename ComponentType>
      static ComponentDeleter MakeComponentDeleter(BlockAllocator<ComponentType>& aAllocator)
      {
        return ComponentDeleter{ &aAllocator, DestructionHandler<ComponentType> };
      }


      void operator()(Component *aToDelete)
      {
        mDestructor(mAllocator, aToDelete);
      }

    private:
      using Destructor = void(*)(void*, Component*);

      ComponentDeleter(void* aAllocator, Destructor aDestructor)
        : mDestructor{aDestructor}
        , mAllocator{aAllocator}
      {

      }

      Destructor mDestructor;
      void* mAllocator;
      //PrivateImplementationLocal<128> mData;

      //template <typename ComponentType>
      //struct ComponentDeleterData
      //{
      //  BlockAllocator<ComponentType>* mAllocator;
      //};

      template <typename ComponentType>
      static void DestructionHandler(void* aAllocator, Component *aToDelete)
      {
        auto allocator = static_cast<BlockAllocator<ComponentType>*>(aAllocator);

        auto component = static_cast<ComponentType*>(aToDelete);

        component->~ComponentType();

        allocator->deallocate(component);
      }
    };
  }

  using ComponentOwner = std::unique_ptr<Component, Detail::ComponentDeleter>;

  class StringComponentFactory : public EventHandler
  {
  public:
    virtual ComponentOwner MakeComponent(Composition *aOwner,
                                                   Space *aSpace) = 0;

    StringComponentFactory(Engine *aEngine) : mEngine(aEngine) {};

    virtual void Initialize() { };
    virtual ~StringComponentFactory() {};
  protected:
    Engine *mEngine;
  };

  template <typename T>
  class ComponentFactory : public StringComponentFactory
  {
  public:
    ComponentOwner MakeComponent(Composition *aOwner, Space *aSpace) override
    {
      auto ptr = mAllocator.allocate();

      new(ptr) T(aOwner, aSpace);

      return ComponentOwner(ptr, Detail::ComponentDeleter::MakeComponentDeleter(mAllocator));
    }
  
    ComponentFactory(Engine *aEngine)
      : StringComponentFactory(aEngine) 
    {

    };

    virtual ~ComponentFactory() { };

  private:
    BlockAllocator<T> mAllocator;
  };
  
  using ComponentMap = OrderedMap<Type*, ComponentOwner>;
}

#endif
