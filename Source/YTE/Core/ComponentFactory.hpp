#pragma once

#ifndef YTE_ComponentFactory_h
#define YTE_ComponentFactory_h

#include <memory>

#include "YTE/Core/EventHandler.hpp"

#include "YTE/Core/ForwardDeclarations.hpp"
#include "YTE/Core/Utilities.hpp"

namespace YTE
{
  class StringComponentFactory : public EventHandler
  {
  public:
    virtual UniquePointer<Component> MakeComponent(Composition *aOwner,
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
    UniquePointer<Component> MakeComponent(Composition *aOwner, Space *aSpace) override
    {
      return std::make_unique<T>(aOwner, aSpace);
    }
  
    ComponentFactory(Engine *aEngine) : StringComponentFactory(aEngine) {};
    virtual ~ComponentFactory() { };
  };
}

#endif
