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

#include "YTE/Core/EventHandler.hpp"
#include "YTE/Core/ForwardDeclarations.hpp"

#include "YTE/Utilities/String/String.h"

namespace YTE
{
  class Composition : public EventHandler
  {
  public:
    YTEDeclareType(Composition);

    Composition(Engine *aEngine, Space *aSpace, String &aName);
    Composition(Engine *aEngine, Space *aSpace);

    ~Composition();

    virtual void Update(float dt);

    virtual void NativeInitialize();
    void PhysicsInitialize();
    virtual void Initialize();
    void DeletionUpdate(LogicUpdate *aUpdate);

    void ToggleSerialize() { mShouldSerialize = !mShouldSerialize; };
    bool ShouldSerialize() const { return mShouldSerialize; };
    void Deserialize(RSValue *aValue);
    RSValue Serialize(RSAllocator &aAllocator) override;
    
    Space* GetSpace() const { return mSpace; }
    Engine* GetEngine() const { return mEngine; }


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


    std::string CheckDependencies(BoundType *aComponent);

    // Gets all Components of the given type that are part of or childed to this composition.
    template <typename ComponentType> std::vector<ComponentType*> GetComponents();

    template <typename ComponentType> ComponentType* GetComponent();
    Component* GetComponent(BoundType *aType);

    Component* AddComponent(BoundType *aType);
    Component* AddComponent(BoundType *aType, RSValue *aProperties);

    Composition* FindFirstCompositionByName(String &aName);
    Composition* FindLastCompositionByName(String &aName);
    CompositionMap::range FindAllCompositionsByName(String &aName);

    Composition* GetOwner() { return mOwner; };
    void SetOwner(Composition *aOwner);
    Composition* GetParent();
    Composition* GetUniverseOrSpaceOrEngine();

    String& GetName() { return mName; };
    void SetName(String &aName);

    ComponentMap* GetComponents() { return &mComponents; };

    void SetArchetypeName(String &aArchName);
    String& GetArchetypeName();
    bool SameAsArchetype();

  protected:
    void RemoveCompositionInternal(CompositionMap::iterator &aComposition);
    void RemoveComponentInternal(ComponentMap::iterator &aComponent);
    Composition* AddCompositionInternal(String aArchetype, String aObjectName);
    Composition* AddCompositionInternal(RSValue *aSerialization, String aObjectName);
    bool ParentBeingDeleted();

    CompositionMap mCompositions;
    ComponentMap mComponents;

    Engine *mEngine;
    Space *mSpace;
    String mName;

    bool mShouldSerialize;
    bool mShouldIntialize;
    bool mIsInitialized;
    bool mBeingDeleted;

    String mArchetypeName;

  private:
    Composition *mOwner;
    Composition(const Composition &) = delete;
    Composition& operator=(const Composition& rhs) = delete;
  };

  template <typename ComponentType>
  ComponentType* Composition::GetComponent()
  {
    auto iterator = mComponents.Find(ComponentType::GetStaticType());

    if (iterator == mComponents.end())
    {
      return nullptr;
    }

    return static_cast<ComponentType*>(iterator->second.get());
  }

  // This function traverses all compositions and retrieves
  // all the components of the templated type.
  template <typename ComponentType>
  std::vector<ComponentType*> Composition::GetComponents()
  {
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
}

YTEDeclareExternalType(CompositionMap::range);

#endif
