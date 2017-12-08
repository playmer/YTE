#include "YTE/Core/Composition.hpp"
#include "YTE/Core/ComponentFactory.hpp"
#include "YTE/Core/Engine.hpp"
#include "YTE/Core/Space.hpp"

#include "YTE/Physics/CollisionBody.hpp"
#include "YTE/Physics/Collider.hpp"
#include "YTE/Physics/GhostBody.hpp"
#include "YTE/Physics/RigidBody.hpp"
#include "YTE/Physics/Transform.hpp"

#include "YTE/Utilities/Utilities.h"

#include "rapidjson\document.h"


namespace YTE
{
  YTEDefineEvent(CompositionAdded);

  YTEDefineType(CompositionAdded)
  {
    YTERegisterType(CompositionAdded);

    YTEBindField(&CompositionAdded::mComposition, "Composition", PropertyBinding::Get);
  }

  YTEDefineEvent(CompositionRemoved);

  YTEDefineType(CompositionRemoved)
  {
    YTERegisterType(CompositionRemoved);

    YTEBindField(&CompositionRemoved::mComposition, "Composition", PropertyBinding::Get);
  }

  YTEDefineEvent(ParentChanged);

  YTEDefineType(ParentChanged)
  {
    YTERegisterType(ParentChanged);

    YTEBindField(&ParentChanged::mOldParent, "Old Parent", PropertyBinding::Get);
    YTEBindField(&ParentChanged::mNewParent, "New Parent", PropertyBinding::Get);
  }

  YTEDefineExternalType(CompositionMap::range)
  {
    YTERegisterType(CompositionMap::range);
  }

  YTEDefineType(Composition)
  {
    YTERegisterType(Composition);

    YTEBindFunction(&Composition::Remove, YTENoOverload, "Remove", YTENoNames).Description()
      = "Removes the composition from it's owner. This is delayed until the next frame.";

    YTEBindFunction(&Composition::GetComponent, (Component* (Composition::*)(BoundType*)), "GetComponent", { "aType" }).Description()
      = "Gets a component via the typeid of the component you want. Should use this.Owner.ComponentType instead.";
    YTEBindFunction(&Composition::AddComponent, (Component*(Composition::*)(BoundType *)), "AddComponent", YTEParameterNames("aType")).Description()
      = "Adds a component via the typeid of the component you want.";

    YTEBindFunction(&Composition::FindFirstCompositionByName, YTENoOverload, "FindFirstCompositionByName", YTEParameterNames("aName")).Description()
      = "Finds the first Composition with the given name. Does not search recursively.";
    YTEBindFunction(&Composition::FindLastCompositionByName, YTENoOverload, "FindLastCompositionByName", YTEParameterNames("aName")).Description()
      = "Finds the last Composition with the given name. Does not search recursively.";
    YTEBindFunction(&Composition::FindAllCompositionsByName, YTENoOverload, "FindAllCompositionsByName", YTEParameterNames("aName")).Description()
      = "Finds a Composition with the given name. Does not search recursively.";

    YTEBindFunction(&Composition::AddComposition, (Composition*(Composition::*)(String, String)), "AddObject", YTEParameterNames("aArchetype", "aName")).Description()
      = "Adds an archetype to this Composition via the name of the Archetype. It takes the name of the object to name it.";
    YTEBindFunction(&Composition::AddCompositionAtPosition, YTENoOverload, "AddObjectAtPosition", YTEParameterNames("aArchetype", "aName", "aPosition")).Description()
      = "Adds an archetype to this Space via the name of the Archetype. It takes the name of the object to name it and the position to place it.";

    YTEBindProperty(&Composition::GetName, YTENoSetter, "Name");

    YTEBindProperty(&Composition::GetSpace, YTENoSetter, "Space");
  }

  Composition::Composition(Engine *aEngine, const String &aName, Space *aSpace)
    : mEngine(aEngine)
    , mSpace(aSpace)
    , mOwner(nullptr)
    , mName(aName)
    , mShouldSerialize(true)
    , mShouldIntialize(true)
    , mIsInitialized(false)
    , mBeingDeleted(false)
    , mGUID()
  {
    mEngine->YTERegister(Events::BoundTypeChanged, this, &Composition::BoundTypeChangedHandler);
  };

  Composition::Composition(Engine *aEngine, Space *aSpace)
    : mEngine(aEngine)
    , mSpace(aSpace)
    , mOwner(nullptr)
    , mName()
    , mShouldSerialize(true)
    , mShouldIntialize(true)
    , mIsInitialized(false)
    , mBeingDeleted(false)
    , mGUID()
  {
    mEngine->YTERegister(Events::BoundTypeChanged, this, &Composition::BoundTypeChangedHandler);
  };

  Composition::~Composition()
  {
    mEngine->RemoveCompositionGUID(mGUID);

    if (nullptr != mSpace)
    {
      CompositionRemoved event;
      event.mComposition = this;
      mBeingDeleted = true;

      mSpace->SendEvent(Events::CompositionRemoved, &event);
    }
  };

  void Composition::BoundTypeChangedHandler(BoundTypeChanged *aEvent)
  {
    auto iterator = mComponents.Find(aEvent->aOldType);

    if (iterator != mComponents.end())
    {
      mComponents.ChangeKey(iterator, aEvent->aNewType);
    }
  }

  void Composition::NativeInitialize()
  {
    if (mShouldIntialize == false)
    {
      return;
    }

    for (auto &component : mComponents)
    {
      component.second->NativeInitialize();
    }

    for (auto &composition : mCompositions)
    {
      composition.second->NativeInitialize();
    }
  }

  void Composition::PhysicsInitialize()
  {
    if (mShouldIntialize == false)
    {
      return;
    }

    auto collider = GetColliderFromObject(this);
    if (collider != nullptr) collider->PhysicsInitialize();
    auto ghostBody = GetComponent<GhostBody>();
    if (ghostBody != nullptr) ghostBody->PhysicsInitialize();
    auto collisionBody = GetComponent<CollisionBody>();
    if (collisionBody != nullptr) collisionBody->PhysicsInitialize();
    auto rigidBody = GetComponent<RigidBody>();
    if (rigidBody != nullptr) rigidBody->PhysicsInitialize();
    //auto transform = GetComponent<Transform>();
    //if (transform != nullptr) transform->PhysicsInitialize();

    for (auto &composition : mCompositions)
    {
      composition.second->PhysicsInitialize();
    }
  }

  void Composition::Initialize()
  {
    if (mShouldIntialize == false)
    {
      return;
    }

    bool collision = mEngine->StoreCompositionGUID(this);

    while (collision)
    {
      mGUID = GlobalUniqueIdentifier();
      collision = mEngine->StoreCompositionGUID(this);
    }

    for (auto &component : mComponents)
    {
      component.second->Initialize();
    }

    for (auto &composition : mCompositions)
    {
      composition.second->Initialize();
    }

    mShouldIntialize = false;
    mIsInitialized = true;


    CompositionAdded event;
    event.mComposition = this;

    if (GetType() != Space::GetStaticType())
    {
      mSpace->SendEvent(Events::CompositionAdded, &event);
    }
  }


  void Composition::Update(float dt)
  {
    (void)dt;
  }

  void Composition::DeletionUpdate(LogicUpdate *aUpdate)
  {
    // Delete Attached Components
    auto componentRange = mEngine->mComponentsToRemove.FindAll(this);

    if (componentRange.IsRange())
    {
      for (auto end = componentRange.end() - 1; end >= componentRange.begin(); --end)
      {
        RemoveComponentInternal(end->second);
      }
    }

    mEngine->mComponentsToRemove.Erase(componentRange);

    // Delete Attached Compositions
    auto compositionRange = mEngine->mCompositionsToRemove.FindAll(this);

    mEngine->mCompositionsToRemove.Erase(compositionRange);

    // Stop handling deletions, as we've completed all of them thus far.
    GetUniverseOrSpaceOrEngine()->YTEDeregister(Events::DeletionUpdate, this, &Composition::BoundTypeChangedHandler);
    SendEvent(Events::DeletionUpdate, aUpdate);
  }

  Composition* Composition::AddCompositionInternal(String aArchetype, String aObjectName)
  {
    Composition *comp = AddCompositionInternal(std::make_unique<Composition>(mEngine,
                                                                             aObjectName,
                                                                             mSpace),
                                               mEngine->GetArchetype(aArchetype),
                                               aObjectName);

    comp->SetArchetypeName(aArchetype);

    return comp;
  }

  Composition* Composition::AddCompositionInternal(std::unique_ptr<Composition> mComposition, RSValue *aSerialization, String aObjectName)
  {
    mComposition->mName = aObjectName;
    auto &composition = mCompositions.Emplace(aObjectName, std::move(mComposition))->second;
    if (aSerialization)
    {
      RSValue *archetype = aSerialization;

      DebugObjection(false == archetype->IsObject(),
        "We're trying to serialize something that isn't a composition.");

      // On release just exit out on these errors.
      if (false == archetype->IsObject())
      {
        printf("We're trying to serialize something that isn't a composition.\n");
        return nullptr;
      }

      DebugObjection(nullptr == archetype,
        "Archetype given is not an object for the object of the name: %s.\n",
        aObjectName.c_str());

      if (nullptr == archetype)
      {
        printf("No archetype provided for the object of the name %s.\n",
          aObjectName.c_str());
        return nullptr;
      }

      composition->Deserialize(archetype);
    };

    return composition.get();
  }


  Composition* Composition::AddComposition(RSValue *aSerialization, String aObjectName)
  {
    auto composition = AddCompositionInternal(std::make_unique<Composition>(mEngine,
                                                                            aObjectName,
                                                                            mSpace),
                                              aSerialization, 
                                              aObjectName);

    if (composition != nullptr)
    {
      composition->NativeInitialize();
      composition->PhysicsInitialize();
      composition->Initialize();
    }

    return composition;
  }


  Composition* Composition::AddComposition(String aArchetype, String aObjectName)
  {
    auto composition = AddCompositionInternal(aArchetype, aObjectName);

    if (composition != nullptr)
    {
      composition->NativeInitialize(); 
      composition->PhysicsInitialize();
      composition->Initialize();
    }

    composition->SetArchetypeName(aArchetype);

    return composition;
  }

  Composition* Composition::AddCompositionAtPosition(String aArchetype, String aObjectName, glm::vec3 aPosition)
  {
    Composition *composition = AddCompositionInternal(aArchetype, aObjectName);

    if (composition != nullptr)
    {
      // Might need to be after the change of translation.
      composition->NativeInitialize();
      composition->PhysicsInitialize();

      auto transform = composition->GetComponent<Transform>();

      if (transform != nullptr)
      {
        transform->SetTranslation(aPosition);
      }

      composition->Initialize();
    }

    return composition;
  }

  void Composition::Deserialize(RSValue *aValue)
  {
    RSStringBuffer buffer;
    RSPrettyWriter writer(buffer);
    aValue->Accept(writer);
    std::string json = buffer.GetString();

    // Annoyingly warn on debug.
    DebugObjection(false == aValue->IsObject(), "We're trying to serialize something that isn't a composition.");
    DebugObjection(false == aValue->HasMember("Compositions") || 
                false == (*aValue)["Compositions"].IsObject(), 
                "We're trying to serialize something without Compositions: \n%s",
                json.c_str());
    DebugObjection(false == aValue->HasMember("Components") ||
                false == (*aValue)["Components"].IsObject(), 
                "We're trying to serialize something without Components: \n%s",
                json.c_str());

    // On release just exit out on these errors.
    if (false == aValue->IsObject())
    {
      printf("We're trying to serialize something that isn't a composition: \n%s",
             json.c_str());
      return;
    }

    if (false == aValue->HasMember("Compositions") ||
        false == (*aValue)["Compositions"].IsObject())
    {
      printf("We're trying to serialize something without Compositions: \n%s",
             json.c_str());
      return;
    }


    if (false == aValue->HasMember("Components") ||
        false == (*aValue)["Components"].IsObject())
    {
      printf("We're trying to serialize something without Components: \n%s",
             json.c_str());
      return;
    }

    auto &components = (*aValue)["Components"];

    for (auto componentIt = components.MemberBegin(); componentIt < components.MemberEnd(); ++componentIt)
    {
      std::string componentTypeName = componentIt->name.GetString();
      BoundType *componentType = Type::GetGlobalType(componentTypeName);
      AddComponent(componentType, &componentIt->value);
    }


    auto &compositions = (*aValue)["Compositions"];

    for (auto compositionIt = compositions.MemberBegin(); compositionIt < compositions.MemberEnd(); ++compositionIt)
    {
      String compositionName = compositionIt->name.GetString();

      auto &composition = mCompositions.Emplace(compositionName,
                                                std::make_unique<Composition>(mEngine, 
                                                                              compositionName, 
                                                                              mSpace))->second;

      composition->SetOwner(this);
      composition->Deserialize(&compositionIt->value);
    }

    if (aValue->HasMember("Archetype"))
    {
        mArchetypeName = (*aValue)["Archetype"].GetString();
    }

  }

  RSValue Composition::Serialize(RSAllocator &aAllocator)
  {
    RSValue toReturn;
    toReturn.SetObject();


    RSValue archetype;
    archetype.SetString(mArchetypeName.c_str(), aAllocator);
    toReturn.AddMember("Archetype", archetype, aAllocator);


    RSValue compositions;
    compositions.SetObject();
    for (auto &composition : mCompositions)
    {
      if (composition.second->ShouldSerialize() == false)
      {
        continue;
      }

      auto compositionSerialized = composition.second->Serialize(aAllocator);

      RSValue compositionName;
      compositionName.SetString(composition.first.c_str(), 
                                static_cast<RSSizeType>(composition.first.Size()), 
                                aAllocator);
    
      compositions.AddMember(compositionName, compositionSerialized, aAllocator);
    }

    toReturn.AddMember("Compositions", compositions, aAllocator);

    RSValue components;
    components.SetObject();
    for (auto &component : mComponents)
    {
      auto componentSerialized = component.second->Serialize(aAllocator);
      
      RSValue componentName;
      componentName.SetString(component.first->GetName().c_str(),
                              static_cast<RSSizeType>(component.first->GetName().size()),
                              aAllocator);

      components.AddMember(componentName, componentSerialized, aAllocator);
    }
    
    toReturn.AddMember("Components", components, aAllocator);

    return toReturn;
  }

  Component* Composition::GetComponent(BoundType *aType)
  {
    auto iterator = mComponents.Find(aType);

    if (iterator == mComponents.end())
    {
      return nullptr;
    }

    return iterator->second.get();
  }


  Composition* Composition::FindFirstCompositionByName(String const &aName)
  {
    auto iterator = mCompositions.FindFirst(aName);

    if (iterator != mCompositions.end())
    {
      return (*iterator).second.get();
    }

    return nullptr;
  }

  Composition* Composition::FindLastCompositionByName(String const &aName)
  {
    auto iterator = mCompositions.FindLast(aName);

    if (iterator != mCompositions.end())
    {
      return (*iterator).second.get();
    }

    return nullptr;
  }

  CompositionMap::range Composition::FindAllCompositionsByName(String const &aName)
  {
    auto compositions = mCompositions.FindAll(aName);
    return compositions;
  }

  std::string Composition::CheckDependencies(BoundType *aType)
  {
    std::string toReturn;

    auto componentDeps = aType->GetAttribute<ComponentDependencies>();

    if (nullptr != componentDeps)
    {
      bool haveAllANDs = true;
      std::vector<std::vector<Type*>> notAcceptedAnds;

      // Check all the AND components.
      for (auto typeOrs : componentDeps->mTypes)
      {
        bool haveAnOr = false;

        for (auto type : typeOrs)
        {
          if (nullptr != GetComponent(type))
          {
            haveAnOr = true;
            break;
          }
        }

        if (false == haveAnOr)
        {
          notAcceptedAnds.push_back(typeOrs);
          haveAllANDs = false;
        }
      }

      if (false == haveAllANDs)
      {
        toReturn += Format("Composition %s is attempting to add Component of "
                           "type %s, but is missing the following Components:\n",
                           mName.c_str(),
                           aType->GetName().c_str());

        for (auto notAcceptedOrs : notAcceptedAnds)
        {
          auto orType = notAcceptedOrs.begin();

          toReturn += Format("  %s", (*orType++)->GetName().c_str());

          for (; orType < notAcceptedOrs.end(); orType++)
          {
            toReturn += Format(" or %s", (*orType)->GetName().c_str());
          }

          toReturn += Format("  \n");
        }
      }
    }

    return toReturn;
  }

  Component* Composition::AddComponent(BoundType *aType)
  {
    // TODO: Output this to a debug logger. If this happens in game in the editor, 
    //       it won't be currently displayed.
    if (false == CheckDependencies(aType).empty())
    {
      return nullptr;
    }

    Component *component = AddComponent(aType, nullptr);
    component->NativeInitialize();
    component->PhysicsInitialize();
    component->Initialize();
    return component;
  }

  Component* Composition::AddComponent(BoundType *aType, RSValue *aProperties)
  {
    Component *toReturn = nullptr;

    auto iterator = mComponents.Find(aType);

    if (iterator == mComponents.end())
    {
      auto addFactory = mEngine->GetComponent<ComponentSystem>()->GetComponentFactory(aType);

      auto component = addFactory->MakeComponent(this, mSpace, aProperties);
      toReturn = component.get();

      mComponents.Emplace(aType, std::move(component));
    }
    else
    {
      toReturn = iterator->second.get();
      toReturn->Deserialize(aProperties);
    }

    return toReturn;
  }

  void Composition::SetOwner(Composition *aOwner)
  {
    mOwner = aOwner;
  }

  // Get the most direct parent, whether that be a Composition,
  // Space or Engine.
  Composition* Composition::GetParent()
  {
    Composition *parent = mOwner;

    if (parent == nullptr)
    {
      parent = mSpace;
    }

    if (this == parent || parent == nullptr)
    {
      parent = mEngine;
    }

    return parent;
  }

  void Composition::ReParent(Composition *aNewParent /* = nullptr */)
  {
    auto parent = GetParent();
    // TODO (Evan): Figure out how we want to handle default re-parenting children of the engine
    // and default re-parenting of spaces
    if (aNewParent == nullptr && (parent == mEngine || this == mSpace))
    {
      return;
    }

    if (aNewParent == nullptr)
    {
      aNewParent = mSpace;
    }

    auto range = parent->FindAllCompositionsByName(mName);
    auto iter = range.begin();
    while (iter != range.end() && iter->second.get() != this)
    {
      ++iter;
    }

    if (iter != range.end()) 
    {
      auto unique_this = std::move(iter->second);
      parent->RemoveCompositionInternal(iter);
      aNewParent->AddCompositionInternal(std::move(unique_this), nullptr, mName);

      ParentChanged event;
      event.mOldParent = parent;
      event.mNewParent = aNewParent;
      SendEvent(Events::ParentChanged, &event);
    }
  }

  // Get the parent Space or Engine.
  Composition* Composition::GetUniverseOrSpaceOrEngine()
  {
    Composition *parent = mSpace;


    if (this == parent || parent == nullptr)
    {
      parent = mEngine;
    }

    return parent;
  }

  bool Composition::ParentBeingDeleted()
  {
    for (Composition *parent = GetParent();
         nullptr != parent;
         parent = GetParent())
    {
      if (true == parent->mBeingDeleted)
      {
        return true;
      }
    }

    return false;
  }

  void  Composition::RemoveCompositionInternal(CompositionMap::iterator &aComposition)
  {
    mCompositions.Erase(aComposition);
  }
  
  void Composition::RemoveComposition(Composition *aComposition)
  { 
    auto compare = [](UniquePointer<Composition> &aLhs, Composition *aRhs)-> bool
                    {
                      return aLhs.get() == aRhs; 
                    };

    auto iter = mCompositions.FindIteratorByPointer(aComposition->mName,
                                                    aComposition, 
                                                    compare);

    if (iter != mCompositions.end())
    {
      mEngine->mCompositionsToRemove.Emplace(this, std::move(iter->second));
      mCompositions.Erase(iter);
    }

    GetUniverseOrSpaceOrEngine()->YTERegister(Events::DeletionUpdate, this, &Composition::DeletionUpdate);
  }

  void  Composition::RemoveComponentInternal(ComponentMap::iterator &aComponent)
  {
    mComponents.Erase(aComponent);
  }

  void Composition::RemoveComponent(BoundType *aComponent)
  {
    auto iter = mComponents.Find(aComponent);

    if (iter != mComponents.end())
    {
      mEngine->mComponentsToRemove.Emplace(this, iter);
    }

    GetUniverseOrSpaceOrEngine()->YTERegister(Events::DeletionUpdate, this, &Composition::DeletionUpdate);
  }

  void Composition::RemoveComponent(Component *aComponent)
  {
    RemoveComponent(aComponent->GetType());
  }

  void Composition::Remove()
  {
    if (false == ParentBeingDeleted())
    {
      mBeingDeleted = true;
      GetParent()->RemoveComposition(this);
    }
  }

  RSValue Composition::RemoveSerialized(RSAllocator &aAllocator)
  {
    auto data = Serialize(aAllocator);
    Remove();
    return data;
  }

  void Composition::SetName(String &aName)
  {
    CompositionMap *compositionMap = GetParent()->GetCompositions();

    auto range = compositionMap->FindAll(mName);
    
    for (auto it = range.begin(); it < range.end(); ++it)
    {
      if (this == it->second.get())
      {
        compositionMap->ChangeKey(it, aName);
        mName = aName;
        return;
      }
    }

    printf("Had an issue changing a composition named %s to be named %s\n", 
           mName.c_str(), 
           aName.c_str());
  }

  void Composition::SetArchetypeName(String &aArchName)
  {
      mArchetypeName = aArchName;
  }

  String& Composition::GetArchetypeName()
  {
      return mArchetypeName;
  }

  bool Composition::SameAsArchetype()
  {
      RSAllocator allocator;
      RSValue comp = Serialize(allocator);

      RSValue *arch = mEngine->GetArchetype(mArchetypeName);

      if (comp == *arch)
      {
          return true;
      }
      
      return false;
  }

  GlobalUniqueIdentifier& Composition::GetGUID()
  {
    return mGUID;
  }

  bool Composition::SetGUID(GlobalUniqueIdentifier aGUID)
  {
    bool collision = mEngine->CheckForCompositionGUIDCollision(aGUID);

    if (collision)
    {
      mEngine->RemoveCompositionGUID(mGUID);
    }
    
    mGUID = aGUID;

    mEngine->StoreCompositionGUID(this);

    return collision;
  }
}
