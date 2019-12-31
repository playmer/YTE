#include <stack>

#include "fmt/format.h"

#include "YTE/Core/Composition.hpp"
#include "YTE/Core/ComponentFactory.hpp"
#include "YTE/Core/Engine.hpp"
#include "YTE/Core/Space.hpp"

#include "YTE/Physics/CollisionBody.hpp"
#include "YTE/Physics/Collider.hpp"
#include "YTE/Physics/GhostBody.hpp"
#include "YTE/Physics/RigidBody.hpp"
#include "YTE/Physics/Transform.hpp"

#include "YTE/Utilities/Utilities.hpp"

#include "rapidjson/document.h"


namespace YTE
{
  std::vector<Type*> GetDependencyOrder(Composition *aComposition);

  YTEDefineEvent(AssetInitialize);
  YTEDefineEvent(NativeInitialize);
  YTEDefineEvent(PhysicsInitialize);
  YTEDefineEvent(Initialize);
  YTEDefineEvent(Start);
  YTEDefineEvent(Deinitialize);

  YTEDefineType(InitializeEvent)
  {
    RegisterType<InitializeEvent>();
    TypeBuilder<InitializeEvent> builder;

    builder.Field<&InitializeEvent::CheckRunInEditor>("CheckRunInEditor", PropertyBinding::Get);
  }

  YTEDefineEvent(CompositionAdded);

  YTEDefineType(CompositionAdded)
  {
    RegisterType<CompositionAdded>();
    TypeBuilder<CompositionAdded> builder;

    builder.Field<&CompositionAdded::mComposition>("Composition", PropertyBinding::Get);
  }

  YTEDefineEvent(CompositionRemoved);

  YTEDefineType(CompositionRemoved)
  {
    RegisterType<CompositionRemoved>();
    TypeBuilder<CompositionRemoved> builder;

    builder.Field<&CompositionRemoved::mComposition>("Composition", PropertyBinding::Get);
  }

  YTEDefineEvent(ParentChanged);

  YTEDefineType(ParentChanged)
  {
    RegisterType<ParentChanged>();
    TypeBuilder<ParentChanged> builder;

    builder.Field<&ParentChanged::mOldParent>("OldParent", PropertyBinding::Get);
    builder.Field<&ParentChanged::mNewParent>("NewParent", PropertyBinding::Get);
  }

  YTEDefineType(Composition)
  {
    RegisterType<Composition>();
    TypeBuilder<Composition> builder;

    builder.Function<&Composition::Remove>("Remove")
      .SetDocumentation("Removes the composition from it's owner. This is delayed until the next frame.");

    //auto getComponentFunction = SelectOverload<Component* (Composition::*)(Type*),&Composition::GetComponent>();
    //builder.Function<getComponentFunction>("GetComponent")
    //  .SetParameterNames("aType")
    //  .SetDocumentation("Gets a component via the typeid of the component you want. Should use this.Owner.ComponentType instead.");

    builder.Function<SelectOverload<Component* (Composition::*)(Type*)>(&Composition::GetComponent)>("GetComponent")
      .SetParameterNames("aType")
      .SetDocumentation("Gets a component via the typeid of the component you want. Should use this.Owner.ComponentType instead.");
    builder.Function<SelectOverload<Component*(Composition::*)(Type *, bool)>(&Composition::AddComponent)>("AddComponent")
      .SetParameterNames("aType", "aCheckRunInEditor")
      .SetDocumentation("Adds a component via the typeid of the component you want.");

    builder.Function<&Composition::FindFirstCompositionByName>("FindFirstCompositionByName")
      .SetParameterNames("aName")
      .SetDocumentation("Finds the first Composition with the given name. Does not search recursively.");
    builder.Function<&Composition::FindLastCompositionByName>("FindLastCompositionByName")
      .SetParameterNames("aName")
      .SetDocumentation("Finds the last Composition with the given name. Does not search recursively.");
    builder.Function<&Composition::FindAllCompositionsByName>("FindAllCompositionsByName")
      .SetParameterNames("aName")
      .SetDocumentation("Finds a Composition with the given name. Does not search recursively.");

    builder.Function<&Composition::IsDependecy>("IsDependecy")
      .SetParameterNames("aComponent")
      .SetDocumentation("Checks to see if the given component is a dependency of other components on the composition. "
                        "Returns info string if it is, empty otherwise.");

    builder.Function<&Composition::HasDependencies>("HasDependencies")
      .SetParameterNames("aComponent")
      .SetDocumentation("Checks to see if a component's dependencies are met. (Eg. so it can be added to the composition. "
                        "Returns info string if it is, empty otherwise.");

    builder.Function<SelectOverload<Composition*(Composition::*)(String, String)>(&Composition::AddComposition)>("AddObject")
      .SetParameterNames("aArchetype", "aName")
      .SetDocumentation("Adds an archetype to this Composition via the name of the Archetype. It takes the name of the object to name it.");
    builder.Function<&Composition::AddCompositionAtPosition>("AddObjectAtPosition")
      .SetParameterNames("aArchetype", "aName", "aPosition")
      .SetDocumentation("Adds an archetype to this Space via the name of the Archetype. It takes the name of the object to name it and the position to place it.");

    builder.Property<&Composition::GetName, NoSetter>("Name");

    builder.Property<&Composition::GetSpace, NoSetter>("Space");
  }

  Composition::Composition(Engine *aEngine, const String &aName, Space *aSpace, Composition *aOwner)
    : mEngine{ aEngine }
    , mSpace{ aSpace }
    , mOwner{ aOwner }
    , mName{ aName }
    , mInitializationHook{this}
    , mShouldSerialize{ true }
    , mBeingDeleted{ false }
    , mGUID{}
  {
    OPTICK_EVENT();

    Create();
  };

  Composition::Composition(Engine *aEngine, Space *aSpace, Composition *aOwner)
    : mEngine{ aEngine }
    , mSpace{ aSpace }
    , mOwner{ aOwner }
    , mName{}
    , mInitializationHook{ this }
    , mShouldSerialize{ true }
    , mBeingDeleted{ false }
    , mGUID{}
  {
    OPTICK_EVENT();

    Create();
  };

  void Composition::Create()
  {
    OPTICK_EVENT();

    mEngine->RegisterEvent<&Composition::BoundTypeChangedHandler>(Events::BoundTypeChanged, this);

    auto parent = GetParent();

    if (nullptr != parent && this != parent)
    {
      parent->RegisterEvent<&Composition::AssetInitialize>(Events::AssetInitialize, this);
      parent->RegisterEvent<&Composition::NativeInitialize>(Events::NativeInitialize, this);
      parent->RegisterEvent<&Composition::PhysicsInitialize>(Events::PhysicsInitialize, this);
      parent->RegisterEvent<&Composition::Initialize>(Events::Initialize, this);
      parent->RegisterEvent<&Composition::Start>(Events::Start, this);
      parent->RegisterEvent<&Composition::Deinitialize>(Events::Deinitialize, this);
    }
  }

  Composition::~Composition()
  {
    OPTICK_EVENT();

    mEngine->RemoveCompositionGUID(mGUID);

    if (nullptr != mSpace)
    {
      CompositionRemoved event;
      event.mComposition = this;
      mBeingDeleted = true;

      mSpace->SendEvent(Events::CompositionRemoved, &event);
    }

    mCompositions.Clear();
    ComponentClear();
  };

  void Composition::ComponentClear()
  {
    OPTICK_EVENT();

    for (auto typeIt = mDependencyOrder.rbegin();
         typeIt < mDependencyOrder.rend();
         ++typeIt)
    {
      auto component = mComponents.Find(*typeIt);
      if (component != mComponents.end())
      {
        mComponents.Erase(component);
      }
    }
  }

  void Composition::BoundTypeChangedHandler(BoundTypeChanged *aEvent)
  {
    OPTICK_EVENT();

    auto iterator = mComponents.Find(aEvent->aOldType);

    if (iterator != mComponents.end())
    {
      mComponents.ChangeKey(iterator, aEvent->aNewType);
    }
  }

  template <auto tMemberFunction>
  void HandleInitialization(std::string const& aEventName,
                            InitializeEvent* aEvent,
                            Composition* aComposition)
  {
    OPTICK_EVENT();

    for (auto &type : aComposition->GetDependencyOrder())
    {
      auto component = aComposition->GetComponent(type);

      if (aEvent->CheckRunInEditor &&
          nullptr == type->GetAttribute<RunInEditor>())
      {
        continue;
      }

      (component->*tMemberFunction)();
    }

    if (aEvent->ShouldRecurse)
    {
      aComposition->SendEvent(aEventName, aEvent);
    }
  }

  void Composition::AssetInitialize(InitializeEvent *aEvent)
  {
    OPTICK_EVENT();

    Composition *collision = mEngine->StoreCompositionGUID(this);

    while (collision)
    {
      if (collision == this)
      {
        break;
      }
      else
      {
        mGUID = GlobalUniqueIdentifier();
        collision = mEngine->StoreCompositionGUID(this);
      }
    }

    HandleInitialization<&Component::AssetInitialize>(Events::AssetInitialize,
                                                      aEvent,
                                                      this);
  }

  void Composition::NativeInitialize(InitializeEvent *aEvent)
  {
    OPTICK_EVENT();
    
    HandleInitialization<&Component::NativeInitialize>(Events::NativeInitialize,
                                                       aEvent,
                                                       this);
  }

  void Composition::PhysicsInitialize(InitializeEvent *aEvent)
  {
    OPTICK_EVENT();

    //if (auto collider = GetColliderFromObject(this);
    //    collider != nullptr && collider->GetType()->GetAttribute<RunInEditor>())
    //{
    //  if (aEvent->CheckRunInEditor &&
    //      collider->GetType()->GetAttribute<RunInEditor>())
    //  {
    //    collider->PhysicsInitialize();
    //  }
    //}
    //
    //if (auto ghostBody = GetComponent<GhostBody>();
    //    ghostBody != nullptr)
    //{
    //  if (aEvent->CheckRunInEditor &&
    //      TypeId<GhostBody>()->GetAttribute<RunInEditor>())
    //  {
    //    ghostBody->PhysicsInitialize();
    //  }
    //}
    //
    //if (auto collisionBody = GetComponent<CollisionBody>();
    //    collisionBody != nullptr)
    //{
    //  if (aEvent->CheckRunInEditor &&
    //      TypeId<CollisionBody>()->GetAttribute<RunInEditor>())
    //  {
    //    collisionBody->PhysicsInitialize();
    //  }
    //}
    //
    //if (auto rigidBody = GetComponent<GhostBody>();
    //    rigidBody != nullptr)
    //{
    //  if (aEvent->CheckRunInEditor &&
    //      TypeId<RigidBody>()->GetAttribute<RunInEditor>())
    //  {
    //    rigidBody->PhysicsInitialize();
    //  }
    //}

    //auto transform = GetComponent<Transform>();
    //if (transform != nullptr) transform->PhysicsInitialize();

    ++physInit;

    //SendEvent(Events::PhysicsInitialize, aEvent);
    
    HandleInitialization<&Component::PhysicsInitialize>(Events::PhysicsInitialize,
                                                        aEvent,
                                                        this);
  }

  void Composition::Initialize(InitializeEvent *aEvent)
  {
    OPTICK_EVENT();

    HandleInitialization<&Component::Initialize>(Events::Initialize,
                                                 aEvent,
                                                 this);

    CompositionAdded event;
    event.mComposition = this;

    if (GetType() != Space::GetStaticType())
    {
      mSpace->SendEvent(Events::CompositionAdded, &event);
    }
  }

  void Composition::Deinitialize(InitializeEvent* aEvent)
  {
    OPTICK_EVENT();

    for (auto it = mDependencyOrder.rbegin(); it != mDependencyOrder.rend(); ++it)
    {
      auto& type = *it;
      auto component = GetComponent(type);

      if (aEvent->CheckRunInEditor &&
          nullptr == type->GetAttribute<RunInEditor>())
      {
        continue;
      }

      {
        OPTICK_EVENT();
        OPTICK_TAG("Deinitialize Composition:", type->GetName().c_str());
        component->Deinitialize();
      }
    }

    SendEvent(Events::Deinitialize, aEvent);
  }

  void Composition::Start(InitializeEvent* aEvent)
  {
    OPTICK_EVENT();

    HandleInitialization<&Component::Start>(Events::Start,
                                            aEvent,
                                            this);
  }


  void Composition::Update(double dt)
  {
    OPTICK_EVENT();

    (void)dt;
  }

  void Composition::DeletionUpdate(LogicUpdate *aUpdate)
  {
    OPTICK_EVENT();

    // Delete Attached Components
    auto componentRange = mEngine->mComponentsToRemove.FindAll(this);

    if (componentRange.IsRange())
    {
      for (auto end = componentRange.end() - 1; end >= componentRange.begin(); --end)
      {
        end->second->second->Deinitialize();
        RemoveComponentInternal(end->second);
      }
    }

    mEngine->mComponentsToRemove.Erase(componentRange);

    // Delete Attached Compositions
    auto compositionRange = mEngine->mCompositionsToRemove.FindAll(this);

    mEngine->mCompositionsToRemove.Erase(compositionRange);

    // Stop handling deletions, as we've completed all of them thus far.
    GetSpaceOrEngine()->DeregisterEvent<&Composition::BoundTypeChangedHandler>(Events::DeletionUpdate,  this);
    SendEvent(Events::DeletionUpdate, aUpdate);
  }

  Composition* Composition::AddCompositionInternal(String aArchetype, String aObjectName)
  {
    OPTICK_EVENT();

    // If a Composition is just below the Space, we currently guarantee their mOwner is
    // nullptr.
    auto owner = this;

    if (GetType()->IsA<Space>())
    {
      owner = nullptr;
    }

    Composition *comp = AddCompositionInternal(std::make_unique<Composition>(mEngine,
                                                                             aObjectName,
                                                                             mSpace,
                                                                             owner),
                                               mEngine->GetArchetype(aArchetype),
                                               aObjectName);

    comp->SetArchetypeName(aArchetype);

    return comp;
  }

  Composition* Composition::AddCompositionInternal(std::unique_ptr<Composition> mComposition, RSValue *aSerialization, String aObjectName)
  {
    OPTICK_EVENT();

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


  Composition* Composition::AddComposition(RSValue* aSerialization, 
                                           String aObjectName)
  {
    OPTICK_EVENT();

    // If a Composition is just below the Space, we currently guarantee their mOwner is
    // nullptr.
    auto owner = this;

    if (GetType()->IsA<Space>())
    {
      owner = nullptr;
    }

    auto composition = AddCompositionInternal(std::make_unique<Composition>(mEngine,
                                                                            aObjectName,
                                                                            mSpace,
                                                                            owner),
                                              aSerialization, 
                                              aObjectName);

    if (composition != nullptr)
    {
      InitializeEvent event;

      composition->AssetInitialize(&event);
      composition->NativeInitialize(&event);
      composition->PhysicsInitialize(&event);
      composition->Initialize(&event);
      composition->Start(&event);
    }

    return composition;
  }


  Composition* Composition::AddComposition(String aArchetype, 
                                           String aObjectName)
  {
    OPTICK_EVENT();

    auto composition = AddCompositionInternal(aArchetype, aObjectName);

    if (composition != nullptr)
    {
      InitializeEvent event;

      composition->AssetInitialize(&event);
      composition->NativeInitialize(&event);
      composition->PhysicsInitialize(&event);
      composition->Initialize(&event);
      composition->Start(&event);
    }

    composition->SetArchetypeName(aArchetype);

    return composition;
  }

  Composition* Composition::AddCompositionAtPosition(String aArchetype, 
                                                     String aObjectName, 
                                                     glm::vec3 aPosition)
  {
    OPTICK_EVENT();

    Composition *composition = AddCompositionInternal(aArchetype, aObjectName);

    if (composition != nullptr)
    {
      InitializeEvent event;

      // Might need to be after the change of translation.
      composition->AssetInitialize(&event);
      composition->NativeInitialize(&event);
      composition->PhysicsInitialize(&event);

      auto transform = composition->GetComponent<Transform>();

      if (transform != nullptr)
      {
        transform->SetTranslation(aPosition);
      }

      composition->Initialize(&event);
      composition->Start(&event);
    }

    return composition;
  }

  void Composition::Deserialize(RSValue *aValue)
  {
    OPTICK_EVENT();

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

    for (auto componentIt = components.MemberBegin();
         componentIt < components.MemberEnd();
         ++componentIt)
    {
      std::string componentTypeName = componentIt->name.GetString();
      Type *componentType = Type::GetGlobalType(componentTypeName);

      AddComponent(componentType, &componentIt->value);
    }

    mDependencyOrder = YTE::GetDependencyOrder(this);

    if (mComponents.size() != mDependencyOrder.size())
    {
      debugbreak();
    }

    // If a Composition is just below the Space, we currently guarantee their mOwner is
    // nullptr.
    auto owner = this;

    if (GetType()->IsA<Space>())
    {
      owner = nullptr;
    }

    auto &compositions = (*aValue)["Compositions"];

    for (auto compositionIt = compositions.MemberBegin();
         compositionIt < compositions.MemberEnd();
         ++compositionIt)
    {
      String compositionName = compositionIt->name.GetString();

      auto uniqueComposition = std::make_unique<Composition>(mEngine,
                                                             compositionName,
                                                             mSpace,
                                                             owner);
      Composition *composition{ uniqueComposition.get() };

      mCompositions.Emplace(compositionName, std::move(uniqueComposition));

      composition->Deserialize(&compositionIt->value);
    }

    if (aValue->HasMember("Archetype"))
    {
        mArchetypeName = (*aValue)["Archetype"].GetString();
    }
  }

  RSValue Composition::Serialize(RSAllocator &aAllocator)
  {
    OPTICK_EVENT();

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

    for (auto const& [type, component] : mComponents)
    {
      auto componentSerialized = component->Serialize(aAllocator);
      
      RSValue componentName;
      componentName.SetString(type->GetName().c_str(),
                              static_cast<RSSizeType>(type->GetName().size()),
                              aAllocator);

      components.AddMember(componentName, componentSerialized, aAllocator);
    }
    
    toReturn.AddMember("Components", components, aAllocator);

    return toReturn;
  }

  Component* Composition::GetComponent(Type *aType)
  {
    OPTICK_EVENT();

    auto iterator = mComponents.Find(aType);

    if (iterator == mComponents.end())
    {
      return nullptr;
    }

    return iterator->second.get();
  }


  Composition* Composition::FindFirstCompositionByName(String const &aName)
  {
    OPTICK_EVENT();

    auto iterator = mCompositions.FindFirst(aName);

    if (iterator != mCompositions.end())
    {
      return (*iterator).second.get();
    }

    return nullptr;
  }

  Composition* Composition::FindLastCompositionByName(String const &aName)
  {
    OPTICK_EVENT();

    auto iterator = mCompositions.FindLast(aName);

    if (iterator != mCompositions.end())
    {
      return (*iterator).second.get();
    }

    return nullptr;
  }

  CompositionMap::range Composition::FindAllCompositionsByName(String const &aName)
  {
    OPTICK_EVENT();

    auto compositions = mCompositions.FindAll(aName);
    return compositions;
  }

  // Based on https://www.geeksforgeeks.org/topological-sorting/
  // as well as https://www.geeksforgeeks.org/detect-cycle-in-a-graph/
  class Graph
  {
    public:
    void AddEdge(Type* aValue, Type* aDependency)
    {
      // Add the dependency to the value's list.
      mAdjacencyLists[aValue].mDependencies.push_back(aDependency);
    }

    void AddVertex(Type* aValue)
    {
      // The only requirement for a vertex is having an AdjacencyList.
      mAdjacencyLists[aValue];
    }

    std::vector<Type*> TopologicalSort()
    {
      // Just in case the user has added since the last sorting.
      ResetVisited();

      std::vector<Type*> stack;
      stack.reserve(mAdjacencyLists.size());

      // Call the recursive helper function to store Topological
      // Sort starting from all vertices one by one
      for (auto[key, value] : mAdjacencyLists)
      {
        if (false == value.mVisited)
        {
          TopologicalSortRecusive(key, stack);
        }
      }

      return std::move(stack);
    }

    private:
    struct Node
    {
      std::vector<Type*> mDependencies;
      bool mVisited = false;
    };

    std::unordered_map<Type*, Node> mAdjacencyLists;

    void ResetVisited()
    {
      for (auto [key, value] : mAdjacencyLists)
      {
        value.mVisited = false;
      }
    }

    void TopologicalSortRecusive(Type* aValue,
                                 std::vector<Type*> &aStack)
    {
      // Mark the current node as visited.
      mAdjacencyLists[aValue].mVisited = true;

      // Recurse for all the vertices adjacent to this vertex

      for (auto value : mAdjacencyLists[aValue].mDependencies)
      {
        if (false == mAdjacencyLists[value].mVisited)
        {
          TopologicalSortRecusive(value, aStack);
        }
      }

      // Push current vertex to stack which stores result
      aStack.emplace_back(aValue);
    }
  };

  std::vector<Type*> GetDependencyOrder(Composition *aComposition)
  {
    OPTICK_EVENT();

    Graph dependencyGraph;

    for (auto const& [type, component] : aComposition->GetComponents())
    {
      // Might not have dependencies, so we add the vertex first just in case.
      dependencyGraph.AddVertex(type);

      auto dependencies = type->GetAttribute<ComponentDependencies>();
    
      if (dependencies)
      {
        // Each element of mTypes here represents a vector in which at least
        // one type must be satisfied.
        for (auto const& dependencyOrs : dependencies->mTypes)
        {
          bool foundOne = false;

          for (auto dependencyOr : dependencyOrs)
          {
            if (aComposition->GetComponent(dependencyOr))
            {
              // We've found a match, we no longer need to search
              // this OR list.
              dependencyGraph.AddEdge(type, dependencyOr);
              foundOne = true;
              break;
            }
          }

          // We didn't find a satisfactory dependency, early out.
          if (false == foundOne)
          {
            // Prefer to return an empty vector for failure;
            return {};
          }
        }
      }
    }

    return std::move(dependencyGraph.TopologicalSort());
  }


  std::string Composition::CheckDependencies(std::set<Type*> aTypesAvailible, 
                                             Type *aTypeToCheck)
  {
    OPTICK_EVENT();

    std::string toReturn;

    auto componentDeps = aTypeToCheck->GetAttribute<ComponentDependencies>();

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
          if (aTypesAvailible.count(type))
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
        toReturn += Format("Composition %s is attempting to add/remove Component of "
                           "type %s, but is missing the following Components:\n",
                           mName.c_str(),
                           aTypeToCheck->GetName().c_str());

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

  std::string Composition::IsDependecy(Type *aType)
  {
    OPTICK_EVENT();

    std::vector<Type*> typesWithAProblem;
    std::set<Type*> typesAvailible;

    for (auto &component : mComponents)
    {
      if (component.first == aType)
      {
        continue;
      }

      typesAvailible.emplace(component.first);
    }

    for (auto const&[type, component] : mComponents)
    {
      if (type == aType)
      {
        continue;
      }

      auto reason = CheckDependencies(typesAvailible, type);

      if (reason.size())
      {
        typesWithAProblem.emplace_back(type);
      }
    }

    std::string toReturn;

    if (typesWithAProblem.size())
    {
      toReturn = fmt::format("Removing the {} component from {} would violate "
                             "the dependencies of the following components:\n",
                             aType->GetName(),
                             mName.c_str());

      for (auto type : typesWithAProblem)
      {
        toReturn += fmt::format("{}\n",
                                type->GetName());
      }
    }

    return toReturn;
  }

  std::string Composition::HasDependencies(Type *aType)
  {
    OPTICK_EVENT();

    std::set<Type*> typesAvailible;

    for (auto const&[type, component] : mComponents)
    {
      typesAvailible.emplace(type);
    }

    return CheckDependencies(typesAvailible, aType);
  }

  Component* Composition::GetDerivedComponent(Type *aType)
  {
    OPTICK_EVENT();

    auto componentType = TypeId<Component>();

    for (auto const&[type, component] : mComponents)
    {
      if (type->IsA(aType, componentType))
      {
        return component.get();
      }
    }

    return nullptr;
  }

  Component* Composition::AddComponent(Type *aType, bool aCheckRunInEditor)
  {
    OPTICK_EVENT();

    // TODO: Output this to a debug logger. If this happens in game in the editor, 
    //       it won't be currently displayed.
    if (false == HasDependencies(aType).empty())
    {
      return nullptr;
    }

    Component *component = AddComponent(aType, nullptr);

    mDependencyOrder.emplace_back(aType);

    if (aCheckRunInEditor &&
        nullptr == aType->GetAttribute<RunInEditor>())
    {
      return component;
    }

    component->AssetInitialize();
    component->NativeInitialize();
    component->PhysicsInitialize();
    component->Initialize();
    component->Start();

    return component;
  }

  Component* Composition::AddComponent(Type *aType, RSValue *aProperties)
  {
    OPTICK_EVENT();
    OPTICK_TAG("Adding Component:", aType->GetName().c_str());
    
    Component *toReturn = nullptr;

    auto iterator = mComponents.Find(aType);

    if (iterator == mComponents.end())
    {
      auto addFactory = mEngine->GetComponentSystem()->GetComponentFactory(aType);

      if (nullptr == addFactory)
      {
        if (aType)
        {
          mEngine->Log(LogType::Warning,
            fmt::format("A factory of the type named {} could not be found. \n"
              "Perhaps it needs to be added to CoreComponentFactoryInitialization",
              aType->GetName()));
        }

        return nullptr;
      }

      auto component = addFactory->MakeComponent(this, mSpace);
      toReturn = component.get();

      DeserializeByType(aProperties, toReturn, aType);

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
    OPTICK_EVENT();

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
    OPTICK_EVENT();

    auto parent = GetParent();
    // TODO (Evan): Figure out how we want to handle default re-parenting children of the engine
    // and default re-parenting of spaces
    if (aNewParent == nullptr && (parent == mEngine || this == mSpace))
    {
      return;
    }

    mOwner = aNewParent;

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
  Composition* Composition::GetSpaceOrEngine()
  {
    OPTICK_EVENT();

    Composition *parent = mSpace;

    if (this == parent || parent == nullptr)
    {
      parent = mEngine;
    }

    return parent;
  }

  bool Composition::ParentBeingDeleted()
  {
    OPTICK_EVENT();

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
    OPTICK_EVENT();

    auto compare = [](UniquePointer<Composition> &aLhs, Composition *aRhs)-> bool
                    {
                      return aLhs.get() == aRhs; 
                    };

    auto iter = mCompositions.FindIteratorByPointer(aComposition->mName,
                                                    aComposition, 
                                                    compare);

    if (iter != mCompositions.end())
    {
      InitializeEvent deinit;
      aComposition->Deinitialize(&deinit);
      mEngine->mCompositionsToRemove.Emplace(this, std::move(iter->second));
      mCompositions.Erase(iter);
    }

    GetSpaceOrEngine()->RegisterEvent<&Composition::DeletionUpdate>(Events::DeletionUpdate, this);
  }

  void  Composition::RemoveComponentInternal(ComponentMap::iterator &aComponent)
  {
    mComponents.Erase(aComponent);
  }

  void Composition::RemoveComponent(Type *aComponent)
  {
    OPTICK_EVENT();

    auto iter = mComponents.Find(aComponent);

    if (iter != mComponents.end())
    {
      mEngine->mComponentsToRemove.Emplace(this, iter);
    }

    mDependencyOrder.erase(std::remove_if(mDependencyOrder.begin(),
                                          mDependencyOrder.end(),
                                          [aComponent](Type* aType) { return aType == aComponent; }),
                           mDependencyOrder.end());

    GetSpaceOrEngine()->RegisterEvent<&Composition::DeletionUpdate>(Events::DeletionUpdate, this);
  }

  void Composition::RemoveComponent(Component *aComponent)
  {
    OPTICK_EVENT();

    RemoveComponent(aComponent->GetType());
  }

  void Composition::Remove()
  {
    OPTICK_EVENT();

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
    CompositionMap &compositionMap = GetParent()->GetCompositions();

    auto range = compositionMap.FindAll(mName);
    
    for (auto it = range.begin(); it < range.end(); ++it)
    {
      if (this == it->second.get())
      {
        compositionMap.ChangeKey(it, aName);
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
    OPTICK_EVENT();

    bool collision = mEngine->CheckForCompositionGUIDCollision(aGUID);

    if (collision)
    {
      mEngine->RemoveCompositionGUID(mGUID);
    }
    
    mGUID = aGUID;

    mEngine->StoreCompositionGUID(this);

    return collision;
  }

  StringComponentFactory* Composition::GetFactoryFromEngine(Type* aType)
  {
    return mEngine->GetComponentSystem()->GetComponentFactory(aType);
  }
}

YTEDefineExternalType(YTE::CompositionMap::range)
{
  RegisterType<CompositionMap::range>();
  TypeBuilder<CompositionMap::range> builder;
}
