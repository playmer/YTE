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

#include "YTE/Core/EventHandler.hpp"

#include "YTE/Core/ForwardDeclarations.hpp"

#include "YTE/Meta/ForwardDeclarations.hpp"

#include "YTE/Utilities/Utilities.h"

namespace YTE
{
  class Component : public EventHandler
  {
  public:
    YTEDeclareType(Component);

    Component(Composition *aOwner, Space *aSpace);

    virtual ~Component() {};

    virtual void NativeInitialize() { };
    virtual void Initialize() { };

    virtual void PhysicsInitialize() { };

    Space* GetSpace() { return mSpace; }
    Composition* GetOwner() { return mOwner; }

    virtual void Deserialize(RSValue*) {};
    RSValue Serialize(RSAllocator &aAllocator) override;

    virtual void Remove();
    virtual RSValue RemoveSerialized();

    void DebugBreak();

    GlobalUniqueIdentifier& GetGUID();
    bool SetGUID(GlobalUniqueIdentifier aGUID);

  protected:
    Composition *mOwner;
    Space *mSpace;

    GlobalUniqueIdentifier mGUID;
  };

  class ComponentDependencies : public Attribute
  {
  public:
    YTEDeclareType(ComponentDependencies);

    ComponentDependencies(DocumentedObject *aObject,
                          std::vector<std::vector<Type*>> aTypes = std::vector<std::vector<Type*>>());

    std::vector<std::vector<Type*>> mTypes;
  };
}
#endif
