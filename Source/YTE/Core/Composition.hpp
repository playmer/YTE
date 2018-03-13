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

#ifndef YTE_Composition_hpp
#define YTE_Composition_hpp

#include <memory>
#include <set>

#include "YTE/Core/ComponentSystem.hpp"
#include "YTE/Core/EventHandler.hpp"
#include "YTE/Core/ForwardDeclarations.hpp"

#include "YTE/Utilities/String/String.hpp"

#include "YTE/Utilities/Utilities.hpp"

namespace YTE
{
  YTEDeclareEvent(NativeInitialize);
  YTEDeclareEvent(PhysicsInitialize);
  YTEDeclareEvent(Initialize);

  class InitializeEvent : public Event
  {
  public:
    YTEDeclareType(InitializeEvent);

    bool CheckRunInEditor = false;
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

    Composition(Engine *aEngine, const String &aName, Space *aSpace, Composition *aOwner = nullptr);
    Composition(Engine *aEngine, Space *aSpace, Composition *aOwner = nullptr);

    ~Composition();

    virtual void Update(double dt);

    virtual void NativeInitialize(InitializeEvent *aEvent);
    void PhysicsInitialize(InitializeEvent *aEvent);
    virtual void Initialize(InitializeEvent *aEvent);
    void DeletionUpdate(LogicUpdate *aUpdate);

    void ToggleSerialize() { mShouldSerialize = !mShouldSerialize; };
    bool ShouldSerialize() const { return mShouldSerialize; };
    void Deserialize(RSValue *aValue);
    RSValue Serialize(RSAllocator &aAllocator) override;

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

    Composition* AddComposition(String aArchetype, String aObjectName);
    Composition* AddComposition(RSValue *aArchetype, String aObjectName);
    Composition* AddCompositionAtPosition(String archetype, String aObjectName, glm::vec3 aPosition);
    inline CompositionMap* GetCompositions() { return &mCompositions; };

    void Remove();
    virtual RSValue RemoveSerialized(RSAllocator &aAllocator);

    void RemoveComponent(Component *aComponent);
    void RemoveComponent(BoundType *aComponent);
    void RemoveComposition(Composition *aComposition);

    void BoundTypeChangedHandler(BoundTypeChanged *aEvent);

    std::string IsDependecy(BoundType *aComponent);
    std::string HasDependencies(BoundType *aComponent);

    // Gets all Components of the given type that are part of or childed to this composition.
    template <typename ComponentType>
    std::vector<ComponentType*> GetComponents()
    {
      static_assert(std::is_base_of<Component, ComponentType>()
                    && !std::is_same<Component, ComponentType>());
      // This function traverses all compositions and retrieves
      // all the components of the templated type.
      std::vector<ComponentType*> components;

      for (auto &composition : mCompositions)
      {
        auto moreComponents = composition.second->GetComponents<ComponentType>();

        components.insert(components.end(), moreComponents.begin(), moreComponents.end());
      }

      auto component = GetComponent<ComponentType>();

      if (component != nullptr)
      {
        components.emplace_back(component);
      }

      return components;
    }

    template <typename tComponentType>
    tComponentType* GetComponent()
    {
      static_assert(std::is_base_of<Component, tComponentType>()
                    && !std::is_same<Component, tComponentType>(),
                    "Type must be derived from YTE::Component");
      auto iterator = mComponents.Find(TypeId<tComponentType>());

      if (iterator == mComponents.end())
      {
        return nullptr;
      }

      return static_cast<tComponentType*>(iterator->second.get());
    }

    Component* GetDerivedComponent(BoundType *aType);

    template <typename tComponentType>
    tComponentType* GetDerivedComponent()
    {
      static_assert(std::is_base_of<Component, tComponentType>()
                    && !std::is_same<Component, tComponentType>(),
                    "Type must be derived from YTE::Component");

      return static_cast<tComponentType*>(GetDerivedComponent(TypeId<tComponentType>()));
    }

    Component* GetComponent(BoundType *aType);

    template <typename ComponentType> 
    ComponentType* AddComponent(RSValue *aProperties = nullptr)
    {
      static_assert(std::is_base_of<Component, ComponentType>()
                    && !std::is_same<Component, ComponentType>());
      ComponentType *toReturn = nullptr;

      auto type = ComponentType::GetStaticType();
      auto iterator = mComponents.Find(type);

      if (iterator == mComponents.end())
      {
        auto addFactory = mEngine->GetComponent<ComponentSystem>()->GetComponentFactory(type);

        auto component = addFactory->MakeComponent(this, mSpace, aProperties);
        toReturn = static_cast<ComponentType*>(component.get());

        mComponents.Emplace(type, std::move(component));
      }
      else
      {
        toReturn = static_cast<ComponentType*>(iterator->second.get());
        toReturn->Deserialize(aProperties);
      }

      return toReturn;
    }

    Component* AddComponent(BoundType *aType, bool aCheckRunInEditor = false);
    Component* AddComponent(BoundType *aType, RSValue *aProperties);

    Composition* FindFirstCompositionByName(String const &aName);
    Composition* FindLastCompositionByName(String const &aName);
    CompositionMap::range FindAllCompositionsByName(String const &aName);

    Composition* GetOwner() { return mOwner; };
    void SetOwner(Composition *aOwner);
    Composition* GetParent();
    void ReParent(Composition *aNewParent = nullptr);
    Composition* GetSpaceOrEngine();

    String const& GetName() const { return mName; };
    void SetName(String &aName);

    ComponentMap* GetComponents() { return &mComponents; };

    void SetArchetypeName(String &aArchName);
    String& GetArchetypeName();
    bool SameAsArchetype();

    GlobalUniqueIdentifier& GetGUID();
    bool SetGUID(GlobalUniqueIdentifier aGUID);

    bool GetIsBeingDeleted() const { return mBeingDeleted; }

  protected:
    std::string CheckDependencies(std::set<BoundType*> aTypesAvailible, 
                                  BoundType *aTypeToCheck);

    void RemoveCompositionInternal(CompositionMap::iterator &aComposition);
    void RemoveComponentInternal(ComponentMap::iterator &aComponent);
    Composition* AddCompositionInternal(String aArchetype, String aObjectName);
    Composition* AddCompositionInternal(std::unique_ptr<Composition> mComposition, 
                                        RSValue *aSerialization, 
                                        String aObjectName);
    bool ParentBeingDeleted();

    CompositionMap mCompositions;
    ComponentMap mComponents;

    Engine *mEngine;
    Space *mSpace;

    GlobalUniqueIdentifier mGUID;
    String mArchetypeName;
    String mName;

    bool mShouldSerialize;
    bool mShouldIntialize;
    bool mIsInitialized;
    bool mBeingDeleted;

    Composition *mOwner;
    Composition(const Composition &) = delete;
    Composition& operator=(const Composition& rhs) = delete;
  };
}

YTEDeclareExternalType(CompositionMap::range);

#endif