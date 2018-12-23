
/******************************************************************************/
/*!
\file   Composition.hpp
\author Joshua T. Fisher
\par    email: j.fisher\@digipen.edu
\par    Course: GAM 200
\date   10/23/2014
\brief
This file contains the declaration of various functions included in our
Component library.

All content (c) 2016 DigiPen  (USA) Corporation, all rights reserved.
*/
/******************************************************************************/
#pragma once

#ifndef YTE_Core_Composition_hpp
#define YTE_Core_Composition_hpp

#include <memory>
#include <set>

#include "YTE/Core/ComponentSystem.hpp"
#include "YTE/Core/EventHandler.hpp"
#include "YTE/Core/ForwardDeclarations.hpp"

#include "YTE/Utilities/String/String.hpp"

#include "YTE/Utilities/Utilities.hpp"

namespace YTE
{
  YTEDeclareEvent(AssetInitialize);
  YTEDeclareEvent(NativeInitialize);
  YTEDeclareEvent(PhysicsInitialize);
  YTEDeclareEvent(Initialize);
  YTEDeclareEvent(Start);
  YTEDeclareEvent(Deinitialize);

  class InitializeEvent : public Event
  {
  public:
    YTEDeclareType(InitializeEvent);

    std::chrono::time_point<std::chrono::high_resolution_clock> mLoadingBegin;
    bool CheckRunInEditor = false;
    bool ShouldRecurse = true;
  };

  YTEDeclareEvent(CompositionAdded);

  class CompositionAdded : public Event
  {
  public:
    YTEDeclareType(CompositionAdded);

    Composition *mComposition;
  };

  YTEDeclareEvent(CompositionRemoved);

  class CompositionRemoved : public Event
  {
  public:
    YTEDeclareType(CompositionRemoved);

    Composition *mComposition;
  };

  YTEDeclareEvent(ParentChanged);

  class ParentChanged : public Event
  {
  public:
    YTEDeclareType(ParentChanged);

    Composition* mOldParent;
    Composition* mNewParent;
  };

  class Composition : public EventHandler
  {
  public:
    YTEDeclareType(Composition);

    YTE_Shared Composition(Engine* aEngine, String const& aName, Space* aSpace, Composition* aOwner = nullptr);
    YTE_Shared Composition(Engine* aEngine, Space* aSpace, Composition* aOwner = nullptr);

    YTE_Shared ~Composition();

    YTE_Shared virtual void Update(double dt);

    YTE_Shared virtual void AssetInitialize(InitializeEvent* aEvent);
    YTE_Shared virtual void NativeInitialize(InitializeEvent* aEvent);
    YTE_Shared virtual void PhysicsInitialize(InitializeEvent* aEvent);
    YTE_Shared virtual void Initialize(InitializeEvent* aEvent);
    YTE_Shared virtual void Deinitialize(InitializeEvent* aEvent);
    YTE_Shared virtual void Start(InitializeEvent* aEvent);
    YTE_Shared void DeletionUpdate(LogicUpdate* aUpdate);

    void ToggleSerialize() { mShouldSerialize = !mShouldSerialize; };
    bool ShouldSerialize() const { return mShouldSerialize; };
    YTE_Shared void Deserialize(RSValue *aValue);
    YTE_Shared RSValue Serialize(RSAllocator &aAllocator) override;

    Space* GetSpace() const { return mSpace; }
    Engine* GetEngine() const { return mEngine; }

    // aObjectName is the name that the Composition gets mapped to. The arguments
    // are passed to the tComposition constructor. We always pass engine.
    template<typename tComposition, typename... Arguments>
    tComposition* AddComposition(String aObjectName, Arguments &&...aArguments)
    {
      tComposition *result = static_cast<tComposition*>(AddCompositionInternal(std::make_unique<tComposition>(aArguments...),
                                                                               nullptr,
                                                                               aObjectName));

      return result;
    }

    template<typename tComposition, typename... Arguments>
    tComposition* AddComposition(Composition *aOwner, String aObjectName, Arguments &&...aArguments)
    {
      tComposition *result = static_cast<tComposition*>(AddCompositionInternal(std::make_unique<tComposition>(aArguments...),
                                                                               nullptr,
                                                                               aObjectName));
      result->mOwner = aOwner;

      return result;
    }

    YTE_Shared Composition* AddComposition(String aArchetype, String aObjectName);
    YTE_Shared Composition* AddComposition(RSValue* aArchetype, String aObjectName);
    YTE_Shared Composition* AddCompositionAtPosition(String archetype, String aObjectName, glm::vec3 aPosition);
    inline CompositionMap& GetCompositions() { return mCompositions; };

    YTE_Shared void Remove();
    YTE_Shared virtual RSValue RemoveSerialized(RSAllocator &aAllocator);

    template <typename tComponentType>
    tComponentType* GetComponent()
    {
      static_assert(std::is_base_of<Component, tComponentType>() &&
                    !std::is_same<Component, tComponentType>(),
                    "Type must be derived from YTE::Component");
      auto iterator = mComponents.Find(TypeId<tComponentType>());

      if (iterator == mComponents.end())
      {
        return nullptr;
      }

      return static_cast<tComponentType*>(iterator->second.get());
    }

    YTE_Shared void RemoveComponent(Component* aComponent);
    YTE_Shared void RemoveComponent(Type* aComponent);
    YTE_Shared void RemoveComposition(Composition* aComposition);

    YTE_Shared void BoundTypeChangedHandler(BoundTypeChanged* aEvent);

    YTE_Shared std::string IsDependecy(Type* aComponent);
    YTE_Shared std::string HasDependencies(Type* aComponent);

    // Gets all Components of the given type that are part of or childed to this composition.
    template <typename tComponentType>
    std::vector<tComponentType*> GetComponents()
    {
      static_assert(std::is_base_of<Component, tComponentType>() &&
                    !std::is_same<Component, tComponentType>());
      // This function traverses all compositions and retrieves
      // all the components of the templated type.
      std::vector<tComponentType*> components;

      for (auto const& [name, composition] : mCompositions)
      {
        auto moreComponents = composition->template GetComponents<tComponentType>();

        components.insert(components.end(), moreComponents.begin(), moreComponents.end());
      }

      auto component = GetComponent<tComponentType>();

      if (component != nullptr)
      {
        components.emplace_back(component);
      }

      return components;
    }

    YTE_Shared Component* GetDerivedComponent(Type* aType);

    template <typename tComponentType>
    tComponentType* GetDerivedComponent()
    {
      static_assert(std::is_base_of<Component, tComponentType>()
                    && !std::is_same<Component, tComponentType>(),
                    "Type must be derived from YTE::Component");

      return static_cast<tComponentType*>(GetDerivedComponent(TypeId<tComponentType>()));
    }

    YTE_Shared Component* GetComponent(Type *aType);

    template <typename tComponentType> 
    tComponentType* AddComponent(RSValue *aProperties = nullptr)
    {
      static_assert(std::is_base_of<Component, tComponentType>()
                    && !std::is_same<Component, tComponentType>());

      auto type = TypeId<tComponentType>();
      auto component = static_cast<tComponentType*>(AddComponent(type, aProperties));

      mDependencyOrder.emplace_back(type);

      return component;
    }

    YTE_Shared Component* AddComponent(Type* aType, bool aCheckRunInEditor = false);
    YTE_Shared Component* AddComponent(Type* aType, RSValue* aProperties);

    YTE_Shared Composition* FindFirstCompositionByName(String const& aName);
    YTE_Shared Composition* FindLastCompositionByName(String const& aName);
    YTE_Shared CompositionMap::range FindAllCompositionsByName(String const& aName);

    Composition* GetOwner() { return mOwner; };
    YTE_Shared void SetOwner(Composition *aOwner);
    YTE_Shared Composition* GetParent();
    YTE_Shared void ReParent(Composition *aNewParent = nullptr);
    YTE_Shared Composition* GetSpaceOrEngine();

    String const& GetName() const { return mName; };
    YTE_Shared void SetName(String &aName);

    ComponentMap& GetComponents() { return mComponents; };

    YTE_Shared void SetArchetypeName(String& aArchName);
    YTE_Shared String& GetArchetypeName();
    YTE_Shared bool SameAsArchetype();

    YTE_Shared GlobalUniqueIdentifier& GetGUID();
    YTE_Shared bool SetGUID(GlobalUniqueIdentifier aGUID);

    bool GetIsBeingDeleted() const { return mBeingDeleted; }


    std::vector<Type*> const& GetDependencyOrder()
    {
      return mDependencyOrder;
    }

    IntrusiveList<Composition>::Hook& GetInitializationHook()
    {
      return mInitializationHook;
    }

  protected:
    YTE_Shared void Create();

    YTE_Shared StringComponentFactory* GetFactoryFromEngine(Type* aType);

    YTE_Shared void ComponentClear();
    YTE_Shared std::string CheckDependencies(std::set<BoundType*> aTypesAvailible, 
                                             BoundType* aTypeToCheck);

    YTE_Shared void RemoveCompositionInternal(CompositionMap::iterator& aComposition);
    YTE_Shared void RemoveComponentInternal(ComponentMap::iterator& aComponent);
    YTE_Shared Composition* AddCompositionInternal(String aArchetype, String aObjectName);
    YTE_Shared Composition* AddCompositionInternal(std::unique_ptr<Composition> mComposition, 
                                                   RSValue* aSerialization, 
                                                   String aObjectName);
    YTE_Shared bool ParentBeingDeleted();

    CompositionMap mCompositions;
    ComponentMap mComponents;
    std::vector<Type*> mDependencyOrder;

    Engine *mEngine;
    Space *mSpace;

    GlobalUniqueIdentifier mGUID;
    String mArchetypeName;
    String mName;
    IntrusiveList<Composition>::Hook mInitializationHook;

    bool mShouldSerialize;
    bool mBeingDeleted;

    Composition* mOwner;
    Composition(const Composition &) = delete;
    Composition& operator=(const Composition& rhs) = delete;

    int physInit = 0;
  };
}

YTEDeclareExternalType(CompositionMap::range);

#endif