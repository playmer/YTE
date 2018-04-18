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
    YTERegisterType(InitializeEvent);

    YTEBindField(&InitializeEvent::CheckRunInEditor, "CheckRunInEditor", PropertyBinding::Get);
  }

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
    YTEBindFunction(&Composition::AddComponent, (Component*(Composition::*)(BoundType *, bool)), "AddComponent", YTEParameterNames("aType", "aCheckRunInEditor")).Description()
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

  Composition::Composition(Engine *aEngine, const String &aName, Space *aSpace, Composition *aOwner)
    : mEngine{ aEngine }
    , mSpace{ aSpace }
    , mOwner{ aOwner }
    , mName{ aName }
    , mShouldSerialize{ true }
    , mShouldIntialize{ true }
    , mIsInitialized{ false }
    , mBeingDeleted{ false }
    , mGUID{}
    , mFinishedComponentAssetInitialize{ false }
    , mFinishedComponentNativeInitialize{ false }
    , mFinishedComponentPhysicsInitialize{ false }
    , mFinishedComponentInitialize{ false }
    , mFinishedComponentStart{ false }
    , mFinishedAssetInitialize{ false }
    , mFinishedNativeInitialize{ false }
    , mFinishedPhysicsInitialize{ false }
    , mFinishedInitialize{ false }
    , mFinishedStart{ false }
  {
    mEngine->YTERegister(Events::BoundTypeChanged, this, &Composition::BoundTypeChangedHandler);

    auto parent = GetParent();

    if (nullptr != parent && this != parent)
    {
      parent->YTERegister(Events::AssetInitialize, this, &Composition::AssetInitialize);
      parent->YTERegister(Events::NativeInitialize, this, &Composition::NativeInitialize);
      parent->YTERegister(Events::PhysicsInitialize, this, &Composition::PhysicsInitialize);
      parent->YTERegister(Events::Initialize, this, &Composition::Initialize);
      parent->YTERegister(Events::Start, this, &Composition::Start);
      parent->YTERegister(Events::Deinitialize, this, &Composition::Deinitialize);
    }
  };

  Composition::Composition(Engine *aEngine, Space *aSpace, Composition *aOwner)
    : mEngine{ aEngine }
    , mSpace{ aSpace }
    , mOwner{ aOwner }
    , mName{}
    , mShouldSerialize{ true }
    , mShouldIntialize{ true }
    , mIsInitialized{ false }
    , mBeingDeleted{ false }
    , mGUID{}
    , mFinishedComponentAssetInitialize{ false }
    , mFinishedComponentNativeInitialize{ false }
    , mFinishedComponentPhysicsInitialize{ false }
    , mFinishedComponentInitialize{ false }
    , mFinishedComponentStart{ false }
    , mFinishedAssetInitialize{ false }
    , mFinishedNativeInitialize{ false }
    , mFinishedPhysicsInitialize{ false }
    , mFinishedInitialize{ false }
    , mFinishedStart{ false }
  {
    mEngine->YTERegister(Events::BoundTypeChanged, this, &Composition::BoundTypeChangedHandler);

    auto parent = GetParent();

    if (nullptr != parent && this != parent)
    {
      parent->YTERegister(Events::AssetInitialize, this, &Composition::AssetInitialize);
      parent->YTERegister(Events::NativeInitialize, this, &Composition::NativeInitialize);
      parent->YTERegister(Events::PhysicsInitialize, this, &Composition::PhysicsInitialize);
      parent->YTERegister(Events::Initialize, this, &Composition::Initialize);
      parent->YTERegister(Events::Start, this, &Composition::Start);
      parent->YTERegister(Events::Deinitialize, this, &Composition::Deinitialize);
    }
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

    mCompositions.Clear();
    ComponentClear();
  };


  void Composition::ComponentClear()
  {
    // Destructing the components in order
    auto order = GetDependencyOrder(this);

    // If our order is compromised, we just clear the vector in whatever 
    // order the container decides to clear in Ideally we fix the 
    // GetDependencyOrder algorithm so this never occurs.
    if (order.size() != mComponents.size())
    {
      mComponents.Clear();
    }
    else
    {
      for (auto typeIt = order.rbegin(); typeIt < order.rend(); ++typeIt)
      {
        auto component = mComponents.Find(*typeIt);
        if (component != mComponents.end())
        {
          mComponents.Erase(component);
        }
      }
    }
  }

  void Composition::BoundTypeChangedHandler(BoundTypeChanged *aEvent)
  {
    auto iterator = mComponents.Find(aEvent->aOldType);

    if (iterator != mComponents.end())
    {
      mComponents.ChangeKey(iterator, aEvent->aNewType);
    }
  }

  void Composition::AssetInitialize(InitializeEvent *aEvent)
  {
    YTEProfileFunction();

    if (aEvent->EarlyOut)
    {
      return;
    }

    using namespace std::chrono;
    duration<double> time_span = duration_cast<duration<double>>(high_resolution_clock::now() -
                                                                 aEvent->mLoadingBegin);
    auto dt = time_span.count();
    if (mFinishedAssetInitialize || (dt > 32.0f))
    {
      aEvent->EarlyOut = true;
      return;
    }

    if (mShouldIntialize == false)
    {
      return;
    }

    if (false == mFinishedComponentAssetInitialize)
    {
      auto order = GetDependencyOrder(this);

      // If our order is compromised, we just initialize in whatever
      // order the Type* are sorted. Ideally we fix the GetDependencyOrder
      // algorithm so this never occurs.
      if (order.size() != mComponents.size())
      {
        for (auto &component : mComponents)
        {
          if (aEvent->CheckRunInEditor &&
              nullptr == component.first->GetAttribute<RunInEditor>())
          {
            continue;
          }

          component.second->AssetInitialize();
        }
      }
      else
      {
        for (auto &type : order)
        {
          auto component = GetComponent(type);

          if (aEvent->CheckRunInEditor &&
              nullptr == type->GetAttribute<RunInEditor>())
          {
            continue;
          }

          component->AssetInitialize();
        }
      }

      mFinishedComponentAssetInitialize = true;
    }

    SendEvent(Events::AssetInitialize, aEvent);

    if (false == aEvent->EarlyOut)
    {
      mFinishedAssetInitialize = true;
    }
  }

  void Composition::NativeInitialize(InitializeEvent *aEvent)
  {
    YTEProfileFunction();

    if (aEvent->EarlyOut)
    {
      return;
    }

    using namespace std::chrono;
    duration<double> time_span = duration_cast<duration<double>>(high_resolution_clock::now() -
                                                                 aEvent->mLoadingBegin);
    auto dt = time_span.count();
    if (mFinishedNativeInitialize || (dt > 0.032f))
    {
      aEvent->EarlyOut = true;
      return;
    }

    if (mShouldIntialize == false)
    {
      return;
    }

    if (false == mFinishedComponentNativeInitialize)
    {
      auto order = GetDependencyOrder(this);

      // If our order is compromised, we just initialize in whatever
      // order the Type* are sorted. Ideally we fix the GetDependencyOrder
      // algorithm so this never occurs.
      if (order.size() != mComponents.size())
      {
        for (auto &component : mComponents)
        {
          if (aEvent->CheckRunInEditor &&
              nullptr == component.first->GetAttribute<RunInEditor>())
          {
            continue;
          }

          {
            YTEProfileBlock(component.first->GetName().c_str());
            component.second->NativeInitialize();
          }
        }
      }
      else
      {
        for (auto &type : order)
        {
          auto component = GetComponent(type);

          if (aEvent->CheckRunInEditor &&
              nullptr == type->GetAttribute<RunInEditor>())
          {
            continue;
          }

          {
            YTEProfileBlock(type->GetName().c_str());
            component->NativeInitialize();
          }
        }
      }

      mFinishedComponentNativeInitialize = true;
    }

    SendEvent(Events::NativeInitialize, aEvent);

    if (false == aEvent->EarlyOut)
    {
      mFinishedNativeInitialize = true;
    }
  }

  void Composition::PhysicsInitialize(InitializeEvent *aEvent)
  {
    YTEProfileFunction();

    if (aEvent->EarlyOut)
    {
      return;
    }

    using namespace std::chrono;
    duration<double> time_span = duration_cast<duration<double>>(high_resolution_clock::now() -
                                                                 aEvent->mLoadingBegin);
    auto dt = time_span.count();
    if (mFinishedPhysicsInitialize || (dt > 32.0f))
    {
      aEvent->EarlyOut = true;
      return;
    }

    if (mShouldIntialize == false)
    {
      return;
    }

    if (false == mFinishedComponentPhysicsInitialize)
    {
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

      auto order = GetDependencyOrder(this);

      // If our order is compromised, we just initialize in whatever
      // order the Type* are sorted. Ideally we fix the GetDependencyOrder
      // algorithm so this never occurs.
      //if (order.size() != mComponents.size())
      //{
      //  for (auto &component : mComponents)
      //  {
      //    if (aEvent->CheckRunInEditor &&
      //        nullptr == component.first->GetAttribute<RunInEditor>())
      //    {
      //      continue;
      //    }
      //
      //    component.second->PhysicsInitialize();
      //  }
      //}
      //else
      //{
      //  for (auto &type : order)
      //  {
      //    auto component = GetComponent(type);
      //    
      //    if (aEvent->CheckRunInEditor &&
      //        nullptr == type->GetAttribute<RunInEditor>())
      //    {
      //      continue;
      //    }
      //    
      //    component->PhysicsInitialize();
      //  }
      //}
      mFinishedComponentPhysicsInitialize = true;
    }

    SendEvent(Events::PhysicsInitialize, aEvent);

    if (false == aEvent->EarlyOut)
    {
      mFinishedPhysicsInitialize = true;
    }
  }

  void Composition::Initialize(InitializeEvent *aEvent)
  {
    YTEProfileFunction();

    if (aEvent->EarlyOut)
    {
      return;
    }

    using namespace std::chrono;
    duration<double> time_span = duration_cast<duration<double>>(high_resolution_clock::now() -
                                                                 aEvent->mLoadingBegin);
    auto dt = time_span.count();
    if (mFinishedInitialize || (dt > 32.0f))
    {
      aEvent->EarlyOut = true;
      return;
    }

    if (mShouldIntialize == false)
    {
      return;
    }

    if (false == mFinishedComponentInitialize)
    {
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

      auto order = GetDependencyOrder(this);

      // If our order is compromised, we just initialize in whatever
      // order the Type* are sorted. Ideally we fix the GetDependencyOrder
      // algorithm so this never occurs.
      if (order.size() != mComponents.size())
      {
        for (auto &component : mComponents)
        {
          if (aEvent->CheckRunInEditor &&
              nullptr == component.first->GetAttribute<RunInEditor>())
          {
            continue;
          }

          {
            YTEProfileBlock(component.first->GetName().c_str());
            component.second->Initialize();
          }
        }
      }
      else
      {
        for (auto &type : order)
        {
          auto component = GetComponent(type);

          if (aEvent->CheckRunInEditor &&
              nullptr == type->GetAttribute<RunInEditor>())
          {
            continue;
          }

          {
            YTEProfileBlock(type->GetName().c_str());
            component->Initialize();
          }
        }
      }

      mFinishedComponentInitialize = true;
    }

    SendEvent(Events::Initialize, aEvent);

    mShouldIntialize = false;
    mIsInitialized = true;

    if (false == aEvent->EarlyOut)
    {
      mFinishedInitialize = true;

      CompositionAdded event;
      event.mComposition = this;

      if (GetType() != Space::GetStaticType())
      {
        mSpace->SendEvent(Events::CompositionAdded, &event);
      }
    }
  }

  void Composition::Start(InitializeEvent *aEvent)
  {
    YTEProfileFunction();

    if (aEvent->EarlyOut)
    {
      return;
    }

    using namespace std::chrono;
    duration<double> time_span = duration_cast<duration<double>>(high_resolution_clock::now() - 
                                                                 aEvent->mLoadingBegin);
    auto dt = time_span.count();
    if (mFinishedStart || (dt > 32.0f))
    {
      aEvent->EarlyOut = true;
      return;
    }

    if (false == mFinishedComponentStart)
    {
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

      auto order = GetDependencyOrder(this);

      // If our order is compromised, we just Start in whatever
      // order the Type* are sorted. Ideally we fix the GetDependencyOrder
      // algorithm so this never occurs.
      if (order.size() != mComponents.size())
      {
        for (auto &component : mComponents)
        {
          if (aEvent->CheckRunInEditor &&
              nullptr == component.first->GetAttribute<RunInEditor>())
          {
            continue;
          }

          component.second->Start();
        }
      }
      else
      {
        for (auto &type : order)
        {
          auto component = GetComponent(type);

          if (aEvent->CheckRunInEditor &&
              nullptr == type->GetAttribute<RunInEditor>())
          {
            continue;
          }

          component->Start();
        }
      }

      mFinishedComponentStart = true;
    }

    SendEvent(Events::Start, aEvent);

    if (false == aEvent->EarlyOut)
    {
      mFinishedStart = true;
    }
  }


  void Composition::Deinitialize(InitializeEvent * aEvent)
  {
    if (!mIsInitialized)
    {
      return;
    }

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

    auto order = GetDependencyOrder(this);

    // If our order is compromised, we just Start in whatever
    // order the Type* are sorted. Ideally we fix the GetDependencyOrder
    // algorithm so this never occurs.
    if (order.size() != mComponents.size())
    {
      for (auto &component : mComponents)
      {
        if (aEvent->CheckRunInEditor &&
          nullptr == component.first->GetAttribute<RunInEditor>())
        {
          continue;
        }

        component.second->Deinitialize();
      }
    }
    else
    {
      for (auto it = order.rbegin(); it != order.rend(); ++it)
      {
        auto& type = *it;
        auto component = GetComponent(type);

        if (aEvent->CheckRunInEditor &&
          nullptr == type->GetAttribute<RunInEditor>())
        {
          continue;
        }

        component->Deinitialize();
      }
    }

    SendEvent(Events::Deinitialize, aEvent);
  }

  void Composition::Update(double dt)
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
    GetSpaceOrEngine()->YTEDeregister(Events::DeletionUpdate, this, &Composition::BoundTypeChangedHandler);
    SendEvent(Events::DeletionUpdate, aUpdate);
  }

  Composition* Composition::AddCompositionInternal(String aArchetype, String aObjectName)
  {
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


  Composition* Composition::AddComposition(RSValue *aSerialization, 
                                           String aObjectName)
  {
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
      BoundType *componentType = Type::GetGlobalType(componentTypeName);
      AddComponent(componentType, &componentIt->value);
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


  struct TypeNode
  {
    TypeNode(Type *aType)
      : mType(aType)
    {

    }

    Type *mType;

    std::map<Type*, std::weak_ptr<TypeNode>> mParents;
    std::map<Type*, std::weak_ptr<TypeNode>> mChildren;
    bool mVisitedParents{ false };
    bool mOrdered{ false };
  };


  void resolveType(std::map<Type*, std::shared_ptr<TypeNode>> &aTypesPlaced, 
                   std::vector<TypeNode*> &aOrdering, 
                   TypeNode *aType)
  {
    if (aType->mOrdered)
    {
      return;
    }

    if (false == aType->mVisitedParents)
    {
      aType->mVisitedParents = true;

      for (auto &parent : aType->mParents)
      {
        auto parentNode = parent.second.lock();
        resolveType(aTypesPlaced, aOrdering, parentNode.get());
      }

      aOrdering.emplace_back(aType);
    }

    for (auto &child : aType->mChildren)
    {
      auto childNode = child.second.lock();
      resolveType(aTypesPlaced, aOrdering, childNode.get());
    }
  };

  
  std::vector<Type*> GetDependencyOrder(Composition *aComposition)
  {
    std::map<Type*, std::shared_ptr<TypeNode>> typesPlaced;
    std::set<Type*> typesOnComposition;

    auto getType = [&typesPlaced](Type *aType)
    {
      auto it = typesPlaced.find(aType);

      if (it != typesPlaced.end())
      {
        return it->second;
      }

      auto it2 = typesPlaced.emplace(aType, std::make_shared<TypeNode>(aType));

      return it2.first->second;
    };

    auto components = aComposition->GetComponents();

    // Preemptively make a set with the components that exist.
    for (auto &component : *components)
    {
      typesOnComposition.emplace(component.first);
    }

    for (auto &component : *components)
    {
      auto typeNode = getType(component.first);

      auto dependencies = typeNode->mType->GetAttribute<ComponentDependencies>();

      if (nullptr != dependencies)
      {
        for (auto &orTypes : dependencies->mTypes)
        {
          for (auto orType : orTypes)
          {
            if (0 != typesOnComposition.count(orType))
            {
              auto dependentTypeNode = getType(orType);
              typeNode->mParents.emplace(dependentTypeNode->mType, dependentTypeNode);
              dependentTypeNode->mChildren.emplace(typeNode->mType, typeNode);
            }
          }
        }
      }
    }

    std::vector<TypeNode*> ordering;

    for (auto &type : typesPlaced)
    {
      if (0 == type.second->mParents.size())
      {
        ordering.emplace_back(type.second.get());
        type.second->mVisitedParents = true;
        type.second->mOrdered = true;
      }
    }

    if (0 == ordering.size())
    {
      std::cout << "Type Cycle detected!\n";
      return {};
    }

    std::vector<TypeNode*> nonDependentTypes{ ordering };

    for (auto &type : nonDependentTypes)
    {
      for (auto &child : type->mChildren)
      {
        auto childNode = child.second.lock();
        resolveType(typesPlaced, ordering, childNode.get());
      }
    }

    std::vector<Type*> toReturn;
    toReturn.reserve(ordering.size());

    for (auto &type : ordering)
    {
      toReturn.emplace_back(type->mType);
    }

    return toReturn;
  }


  std::string Composition::CheckDependencies(std::set<BoundType*> aTypesAvailible, 
                                             BoundType *aTypeToCheck)
  {
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

  std::string Composition::IsDependecy(BoundType *aType)
  {
    std::vector<BoundType*> typesWithAProblem;
    std::set<BoundType*> typesAvailible;

    for (auto &component : mComponents)
    {
      if (component.first == aType)
      {
        continue;
      }

      typesAvailible.emplace(component.first);
    }

    for (auto &component : mComponents)
    {
      if (component.first == aType)
      {
        continue;
      }

      auto reason = CheckDependencies(typesAvailible, component.first);

      if (reason.size())
      {
        typesWithAProblem.emplace_back(component.first);
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

  std::string Composition::HasDependencies(BoundType *aType)
  {
    std::set<BoundType*> typesAvailible;

    for (auto &component : mComponents)
    {
      typesAvailible.emplace(component.first);
    }

    return CheckDependencies(typesAvailible, aType);
  }

  Component* Composition::GetDerivedComponent(BoundType *aType)
  {
    auto componentType = TypeId<Component>();

    for (auto &componentIt : mComponents)
    {
      auto type = componentIt.first;

      if (type->IsA(aType, componentType))
      {
        return componentIt.second.get();
      }
    }

    return nullptr;
  }

  Component* Composition::AddComponent(BoundType *aType, bool aCheckRunInEditor)
  {
    // TODO: Output this to a debug logger. If this happens in game in the editor, 
    //       it won't be currently displayed.
    if (false == HasDependencies(aType).empty())
    {
      return nullptr;
    }

    Component *component = AddComponent(aType, nullptr);


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

  Component* Composition::AddComponent(BoundType *aType, RSValue *aProperties)
  {
    Component *toReturn = nullptr;

    auto iterator = mComponents.Find(aType);

    if (iterator == mComponents.end())
    {
      auto addFactory = mEngine->GetComponent<ComponentSystem>()->GetComponentFactory(aType);

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
      InitializeEvent deinit;
      aComposition->Deinitialize(&deinit);
      mEngine->mCompositionsToRemove.Emplace(this, std::move(iter->second));
      mCompositions.Erase(iter);
    }

    GetSpaceOrEngine()->YTERegister(Events::DeletionUpdate, this, &Composition::DeletionUpdate);
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

    GetSpaceOrEngine()->YTERegister(Events::DeletionUpdate, this, &Composition::DeletionUpdate);
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
