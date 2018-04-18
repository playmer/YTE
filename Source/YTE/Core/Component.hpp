/******************************************************************************/
/*!
\author Joshua T. Fisher
All content (c) 2016 DigiPen  (USA) Corporation, all rights reserved.
*/
/******************************************************************************/
#pragma once

#ifndef YTE_Core_Component_hpp
#define YTE_Core_Component_hpp

#include <memory>

#include "YTE/Core/EventHandler.hpp"

#include "YTE/Core/ForwardDeclarations.hpp"

#include "YTE/Meta/ForwardDeclarations.hpp"

#include "YTE/Utilities/Utilities.hpp"

namespace YTE
{
  class Component : public EventHandler
  {
  public:
    YTEDeclareType(Component);

    Component(Composition *aOwner, Space *aSpace);

    virtual ~Component();

    virtual void AssetInitialize() { };
    virtual void NativeInitialize() { };
    virtual void PhysicsInitialize() { };
    virtual void Initialize() { };
    virtual void Start() { };
    virtual void Deinitialize() { };

    Space* GetSpace() { return mSpace; }
    Composition* GetOwner() { return mOwner; }

    void Deserialize(RSValue* aProperties) override
    {
      DeserializeByType(aProperties, this, GetStaticType());
    };

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
