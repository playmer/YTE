/******************************************************************************/
/*!
\file   ComponentFactory.h
\author Joshua T. Fisher \ Austin A. Morris
\par    email: j.fisher\@digipen.edu, a.morris\@digipen.edu
\par    Course: GAM 200
\date   10/23/2014
\brief
This file contains the declaration of various functions included in our
ComponentFactory library.

All content (c) 2016 DigiPen  (USA) Corporation, all rights reserved.
*/
/******************************************************************************/
#pragma once

#ifndef YTE_ComponentSystem_h
#define YTE_ComponentSystem_h

#include <unordered_map>

#include "YTE/Core/Component.hpp"
#include "YTE/Core/ComponentFactory.hpp"
#include "YTE/Core/ForwardDeclarations.hpp"

namespace YTE
{
  class ComponentSystem : public Component
  {
  public:
    YTEDeclareType(ComponentSystem);

    // Sets up the system, probably could just be the constructor.
    ComponentSystem(Composition *aOwner);

    void FactorySetup(FactorySetupCallback aFunctionPtr);
    void BoundTypeChangedHandler(BoundTypeChanged *aEvent);

    std::pair<StringComponentFactory *, UniquePointer<Component>> 
      MakeComponent(Type *aType, Composition *aOwner);

    template<typename T>
    ComponentFactory<T>* GetComponentFactory()
    {
      auto iterator = mComponentFactories.Find(T::GetStaticType());
        
      if (iterator != mComponentFactories.end())
      {
        return static_cast<ComponentFactory<T>*>((*iterator).second.get());
      }

      return nullptr;
    }
      
    StringComponentFactory* GetComponentFactory(Type *aType)
    {
      auto iterator = mComponentFactories.Find(aType);

      if (iterator != mComponentFactories.end())
      {
        return iterator->second.get();
      }

      return nullptr;
    }

    std::vector<Type*> GetComponentTypes()
    {
      std::vector<Type*> components;
 
      for (auto &factoryPair : mComponentFactories)
      {
        components.push_back(factoryPair.first);
      }

      return components;
    }

    FactoryMap* GetComponentFactories() { return &mComponentFactories; };

    ComponentSystem(const ComponentSystem&) : Component(nullptr, nullptr) 
      { abort(); }

  private:
    OrderedMap<Type*, std::unique_ptr<StringComponentFactory>> mComponentFactories;
  };
}

#endif
