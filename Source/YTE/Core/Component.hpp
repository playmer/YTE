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

    YTE_Shared Component(Composition *aOwner, Space *aSpace);

    YTE_Shared virtual ~Component();

    virtual void AssetInitialize() { };
    virtual void NativeInitialize() { };
    virtual void PhysicsInitialize() { };
    virtual void Initialize() { };
    virtual void Start() { };
    virtual void Deinitialize() { };

    Space const* GetSpace() const { return mSpace; }
    Space* GetSpace() { return mSpace; }
    Composition const* GetOwner() const { return mOwner; }
    Composition* GetOwner() { return mOwner; }

    void Deserialize(RSValue* aProperties) override
    {
      DeserializeByType(aProperties, this, GetStaticType());
    };

    YTE_Shared RSValue Serialize(RSAllocator &aAllocator) override;

    YTE_Shared virtual void Remove();
    YTE_Shared virtual RSValue RemoveSerialized();

    YTE_Shared void DebugBreak();

    YTE_Shared GlobalUniqueIdentifier const& GetGUID();
    YTE_Shared bool SetGUID(GlobalUniqueIdentifier aGUID);

    YTE_Shared IntrusiveList<Component>::Hook& GetDeletionHook()
    {
      return mDeletionHook;
    };

  protected:
    Composition *mOwner;
    Space *mSpace;
    IntrusiveList<Component>::Hook mDeletionHook;

    GlobalUniqueIdentifier mGUID;
  };

  class ComponentDependencies : public Attribute
  {
  public:
    YTEDeclareType(ComponentDependencies);

    YTE_Shared ComponentDependencies(DocumentedObject* aObject,
                                     std::vector<std::vector<Type*>> aTypes = std::vector<std::vector<Type*>>());

    std::vector<std::vector<Type*>> mTypes;
  };

  namespace Attributes
  {
    // By default, a component and all properties/fields will be deserialized
    // after construction. But by using these attributes, the appropriate piece
    // will be deserialized just before the given initialization phase.
    enum class DeserializationTime
    {
      Asset,
      Native,
      Physics,
      Initialize,
      Start,
      Runtime
    };

    class ComponentInitialize : public Attribute
    {
      public:
      YTEDeclareType(ComponentInitialize);

      YTE_Shared ComponentInitialize(DocumentedObject* aObject, 
                                     DeserializationTime aTime);
      DeserializationTime mTime;
    };


    class PropertyInitialize : public Attribute
    {
      public:
      YTEDeclareType(PropertyInitialize);

      YTE_Shared PropertyInitialize(DocumentedObject* aObject, 
                                    DeserializationTime aTime);
      DeserializationTime mTime;
    };
  }
}

#endif
