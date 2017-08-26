/******************************************************************************/
/*!
\author Joshua T. Fisher
All content (c) 2016 DigiPen  (USA) Corporation, all rights reserved.
*/
/******************************************************************************/
#pragma once

#ifndef YTE_Component_Component_h
#define YTE_Component_Component_h

#include <memory>

#include "YTE/Event/Events.h"

#include "YTE/Core/ForwardDeclarations.hpp"

#include "YTE/Meta/ForwardDeclarations.hpp"

namespace YTE
{
  class Component : public BaseEventHandler
  {
  public:
    DeclareType(Component);

    Component(Composition *aOwner, Space *aSpace)
      : mOwner(aOwner), mSpace(aSpace) { };

    virtual ~Component() {};

    virtual void NativeInitialize() { };
    virtual void Initialize() { };

    Space* GetSpace() { return mSpace; }
    Composition* GetOwner() { return mOwner; }

    virtual void Deserialize(RSValue*) {};
    RSValue Serialize(RSAllocator &aAllocator) override;

    virtual void Remove();
    virtual RSValue RemoveSerialized();

    void DebugBreak();

  protected:
    Composition *mOwner;
    Space *mSpace;
  };

  class ComponentDependencies : public Attribute
  {
  public:
    DeclareType(ComponentDependencies);

    ComponentDependencies(DocumentedObject *aObject,
                          std::vector<std::vector<Type*>> aTypes = std::vector<std::vector<Type*>>());

    std::vector<std::vector<Type*>> mTypes;
  };
}
#endif
