#include <memory>
#include <iostream>
#include <fstream>

#include "YTE/Core/Actions/ActionManager.hpp"
#include "YTE/Core/Component.hpp"
#include "YTE/Core/Composition.hpp"
#include "YTE/Core/Engine.hpp"
#include "YTE/Core/Space.hpp"
#include "YTE/Core/AssetLoader.hpp"

#include "YTE/Graphics/Camera.hpp"
#include "YTE/Graphics/GraphicsView.hpp"

#include "YTE/Physics/Orientation.hpp"
#include "YTE/Physics/PhysicsSystem.hpp"
#include "YTE/Physics/RigidBody.hpp"
#include "YTE/Physics/Transform.hpp"

#include "YTE/Utilities/Utilities.hpp"

//#include "YTE/WWise/WWiseView.hpp"

namespace YTE
{
  using namespace std::chrono;

  YTEDefineEvent(SpaceUpdate);

  YTEDefineType(Space)
  {
    RegisterType<Space>();
    TypeBuilder<Space> builder;
    
    builder.Function<&Space::LoadLevel>("LoadLevel")
      .SetParameterNames("aLevel", "CheckRunInEditor")
      .SetDocumentation("Loads a level within the current space on the next frame. Current level will be torn down.");
    
    builder.Function<&Space::SaveLevel>("SaveLevel")
      .SetParameterNames("aLevelName")
      .SetDocumentation("Saves a level to the given file");
    
    builder.Function<&Space::Remove>("Remove")
      .SetDocumentation("Saves a level to the given file");
      
    builder.Property<&Space::IsPaused, &Space::SetPaused>("Paused")
      .SetDocumentation("Sets if the space is paused or not.");
    builder.Property<&Space::GetEngine, NoSetter>("Engine");
    
    builder.Field<&Space::mStartingLevel>("StartingLevel", PropertyBinding::GetSet)
      .AddAttribute<EditorProperty>()
      .AddAttribute<Serializable>();
  }


  Space::Space(Engine* aEngine, RSValue* aProperties)
    : Composition(aEngine, this, aEngine)
    , mLevelToLoad(nullptr)
  {
    if (false == mEngine->IsEditor())
    {
      mEngine->GetWindow()->RegisterEvent<&Space::WindowLostOrGainedFocusHandler>(Events::WindowFocusLostOrGained, this);
      mEngine->GetWindow()->RegisterEvent<&Space::WindowMinimizedOrRestoredHandler>(Events::WindowMinimizedOrRestored, this);
    }

    mEngine->RegisterEvent<&Space::Update>(Events::SpaceUpdate, this);
    mEngine->RegisterEvent<&Space::DeletionUpdate>(Events::DeletionUpdate, this);

    if (nullptr != aProperties)
    {
      DeserializeByType(aProperties, this, TypeId<Space>());
    }
  }

  void Space::Initialize(InitializeEvent* aEvent)
  {
    OPTICK_EVENT();

    aEvent->CheckRunInEditor = mIsEditorSpace;

    Composition::AssetInitialize(aEvent);
    Composition::NativeInitialize(aEvent);
    Composition::PhysicsInitialize(aEvent);
    Composition::Initialize(aEvent);
    Composition::Start(aEvent);

    // It's possible someone called Load with false for Initialize
    // but then called it manually. So we need to prevent double
    // initialization here.
    mAssetInitialize.UnlinkAll();
    mNativeInitialize.UnlinkAll();
    mPhysicsInitialize.UnlinkAll();
    mInitialize.UnlinkAll();
    mStart.UnlinkAll();

    mFinishedLoading = true;
  }

  void Space::Initialize()
  {
    OPTICK_EVENT();

    InitializeEvent event;
    event.CheckRunInEditor = mIsEditorSpace;
    Initialize(&event);
  }
  

  void Space::DeletionUpdate(LogicUpdate *aUpdate)
  {
    OPTICK_EVENT();
    
    // Delete Attached Components
    //auto componentRange = mEngine->mComponentsToRemove.FindAll(this);
    //
    //if (componentRange.IsRange())
    //{
    //  for (auto end = componentRange.end() - 1; end >= componentRange.begin(); --end)
    //  {
    //    end->second->second->Deinitialize();
    //    RemoveComponentInternal(end->second);
    //  }
    //}
    //
    //mEngine->mComponentsToRemove.Erase(componentRange);
    //
    //// Delete Attached Compositions
    //auto compositionRange = mEngine->mCompositionsToRemove.FindAll(this);
    //
    //mEngine->mCompositionsToRemove.Erase(compositionRange);
    //
    //// Stop handling deletions, as we've completed all of them thus far.
    //GetSpaceOrEngine()->DeregisterEvent<&Composition::BoundTypeChangedHandler>(Events::DeletionUpdate,  this);
    //SendEvent(Events::DeletionUpdate, aUpdate);

    if (mCompositionDeletionList.Empty() && mComponentDeletionList.Empty())
    {
      return;
    }
    
    // Delete Compositions
    {
      auto it = mCompositionDeletionList.begin();

      while (it.NextHook() != &mCompositionDeletionList.mHead)
      {
        // While technically the next hook, this hook pointer represents the delegate we're
        // about to invoke.
        auto current = it.NextHook();

        {
          auto &composition = *it;
          auto& compositionOwnerCompositions = composition.GetParent()->GetCompositions();
      
          auto compare = [](UniquePointer<Composition> &aLhs, Composition *aRhs)-> bool
                          {
                            return aLhs.get() == aRhs; 
                          };
          auto iter = compositionOwnerCompositions.FindIteratorByPointer(composition.GetName(),
                                                                         &composition, 
                                                                         compare);

          if (iter != compositionOwnerCompositions.end())
          {
            compositionOwnerCompositions.Erase(iter);
          }
        }

        // We need to check to see if we're reached the end due to some number of events
        // (including the current) removing itself from the list.
        if (it.NextHook() == &mCompositionDeletionList.mHead)
        {
          break;
        }

        // We check to see if our current next hook is the same. If it isn't it means an
        // event has removed itself, so we redo this loop without incrementing.
        if (false == it.IsNextSame(current))
        {
          continue;
        }

        // We don't need to iterate our iterator, we can just remove the current
        current->Unlink();
      }
    }

    // Delete Components
    {
      auto it = mComponentDeletionList.begin();

      while (it.NextHook() != &mComponentDeletionList.mHead)
      {
        // While technically the next hook, this hook pointer represents the delegate we're
        // about to invoke.
        auto current = it.NextHook();

        {
          auto &component = *it;
          auto& componentOwnerComponents = component.GetOwner()->GetComponents();
      
          auto iter = componentOwnerComponents.Find(component.GetType());
    
          if (iter != componentOwnerComponents.end())
          {
            componentOwnerComponents.Erase(iter);
          }
        }

        // We need to check to see if we're reached the end due to some number of events
        // (including the current) removing itself from the list.
        if (it.NextHook() == &mComponentDeletionList.mHead)
        {
          break;
        }

        // We check to see if our current next hook is the same. If it isn't it means an
        // event has removed itself, so we redo this loop without incrementing.
        if (false == it.IsNextSame(current))
        {
          continue;
        }

        // We don't need to iterate our iterator, we can just remove the current
        current->Unlink();
      }
    }
  }

  void Space::ConnectNodes(Space* aSpace, Composition* aComposition)
  {
    aSpace->mAssetInitialize.InsertBack(aComposition->GetInitializationHook());

    for (auto const& [name, composition] : aComposition->GetCompositions())
    {
      ConnectNodes(aSpace, composition.get());
    }
  }

  // Loads a level into the current Space. If already loaded, destroys 
  // the current Space and loads level in place.
  void Space::Load()
  {
    OPTICK_EVENT();
    if (mStartingLevel.Empty())
    {
      CreateBlankLevel("NewLevel");
    }
    else
    {
      Load(mEngine->GetLevel(mStartingLevel));
      mLevelName = mStartingLevel;
    }
  }

  // Loads a level into the current Space. If already loaded, destroys 
  // the current Space and loads level in place.
  void Space::Load(RSValue* aLevel, Space* aInitializeVia)
  {
    OPTICK_EVENT();

    bool mustSeriallyInitialize = false;

    if (nullptr == aInitializeVia)
    {
      aInitializeVia = this;
    }

    if (aInitializeVia->GetFinishedLoading())
    {
      mustSeriallyInitialize = true;
    }

    mCompositions.Clear();
    ComponentClear();
      
    if (nullptr != aLevel)
    {
      Deserialize(aLevel);
    }
    else
    {
      printf("We could not deserialize the level provided.\n");
    }
      
    if (mustSeriallyInitialize)
    {
      Initialize();
    }
    else
    {
      for (auto const&[name, composition] : mCompositions)
      {
        ConnectNodes(aInitializeVia, composition.get());
      }


      InitializeEvent event;
      event.mLoadingBegin = high_resolution_clock::now();
      event.ShouldRecurse = false;
      event.CheckRunInEditor = mIsEditorSpace;

      Composition::AssetInitialize(&event);
      Composition::NativeInitialize(&event);
      Composition::PhysicsInitialize(&event);
      Composition::Initialize(&event);
      Composition::Start(&event);
    }

    mFinishedLoading = false;
    mLoading = false;
  }


  template<auto tMemberFunction>
  void RunInitPhase(InitializeEvent* aEvent,
                    IntrusiveList<Composition>& aFrom,
                    IntrusiveList<Composition>* aTo)
  {
    OPTICK_EVENT();

    if (aFrom.Empty())
    {
      return;
    }

    auto it = aFrom.begin();

    while (it.NextHook() != &aFrom.mHead)
    {
      // Have we already gone over? Skip.
      auto delta = high_resolution_clock::now() - aEvent->mLoadingBegin;
      duration<double> time_span = duration_cast<duration<double>>(delta);
      if (0.032f < time_span.count())
      {
        return;
      }

      // While technically the next hook, this hook pointer represents the delegate we're
      // about to invoke.
      auto current = it.NextHook();

      auto &composition = *it;

      (composition.*tMemberFunction)(aEvent);

      // We need to check to see if we're reached the end due to some number of events
      // (including the current) removing itself from the list.
      if (it.NextHook() == &aFrom.mHead)
      {
        break;
      }

      // We check to see if our current next hook is the same. If it isn't it means an
      // event has removed itself, so we redo this loop without incrementing.
      if (false == it.IsNextSame(current))
      {
        continue;
      }

      // We don't need to iterate our iterator, we can just remove the current
      current->Unlink();

      if (aTo)
      {
        aTo->InsertBack(*current);
      }
    }
  }

  // Updates the Space to the current frame.
  void Space::Update(LogicUpdate* aEvent)
  {
    OPTICK_EVENT();

    SendEvent(Events::SpaceUpdate, aEvent);
    SendEvent(Events::DeletionUpdate, aEvent);

    if (mLoading)
    {
      mLevelName = mLoadingName;
      SetName(mLoadingName);
      Load(mLevelToLoad);
    }

    if (false == mFinishedLoading)
    {
      InitializeEvent event;
      event.mLoadingBegin = high_resolution_clock::now();
      event.ShouldRecurse = false;
      event.CheckRunInEditor = mIsEditorSpace;

      RunInitPhase<&Composition::AssetInitialize>(&event, mAssetInitialize, &mNativeInitialize);
      RunInitPhase<&Composition::NativeInitialize>(&event, mNativeInitialize, &mPhysicsInitialize);
      RunInitPhase<&Composition::PhysicsInitialize>(&event, mPhysicsInitialize, &mInitialize);
      RunInitPhase<&Composition::Initialize>(&event, mInitialize, &mStart);
      RunInitPhase<&Composition::Start>(&event, mStart, nullptr);

      if (mAssetInitialize.Empty() &&
          mNativeInitialize.Empty() &&
          mPhysicsInitialize.Empty() &&
          mInitialize.Empty() &&
          mStart.Empty())
      {
        mFinishedLoading = true;
      }
      else
      {
        return;
      }
    }
      
    if (mPaused == false)
    {
      SendEvent(Events::PhysicsUpdate, aEvent);
    }

    // TODO: Move the frame update calls to the graphics system
    SendEvent(Events::FrameUpdate, aEvent);

    // Don't send the LogicUpdate Event if the space is paused.
    if (mPaused == false)
    {
      SendEvent(Events::LogicUpdate, aEvent);
    }
  }
    
  // Cleans up anything in the Space.
  Space::~Space() 
  {
  }

  void Space::CreateBlankLevel(String const& aLevelName)
  {
    mCompositions.Clear();
    ComponentClear();

    mLevelName = aLevelName;

    //AddComponent<WWiseView>();
    auto graphicsView = AddComponent<GraphicsView>();
    graphicsView->ChangeWindow("Yours Truly Engine");
    graphicsView->NativeInitialize();
    
    AddComponent<PhysicsSystem>();
    AddComponent<ActionManager>();

    auto camera = AddComposition<Composition>("Camera", mEngine, this);
    camera->SetOwner(this);
    camera->AddComponent<Camera>();
    camera->AddComponent<Orientation>();
    camera->AddComponent<Transform>();

    Initialize();

    mLoading = false;
  }

  void Space::LoadLevel(String& level, bool aCheckRunInEditor)
  {
    mCheckRunInEditor = aCheckRunInEditor;
    mLoading = true;
    mLevelToLoad = mEngine->GetLevel(level);
    mLoadingName = level;
  }

  void Space::SaveLevel(String& aLevelName)
  {
    // TODO (Josh): Fix
    RSAllocator allocator;
    auto value = Serialize(allocator);

    std::string levelNameTemp(aLevelName.c_str());
    std::wstring level{levelNameTemp.begin(), levelNameTemp.end()};

    std::string path = YTE::Path::GetGamePath().String();
    std::wstring pathWStr{ path.begin(), path.end() };
    
    level = pathWStr + L"Levels/" + level + L".json";

    std::filesystem::path basePath{ cWorkingDirectory };
    std::filesystem::path levelPath{ level };
    std::filesystem::path baseAndLevel = basePath / levelPath;
    
    RSStringBuffer sb;
    RSPrettyWriter writer(sb);
    value.Accept(writer);    // Accept() traverses the DOM and generates Handler events.
    std::string levelInJson = sb.GetString();
    
    std::ofstream levelToSave;
    levelToSave.open(std::filesystem::canonical(baseAndLevel));
    levelToSave << levelInJson;
    levelToSave.close();
  }


  Space* Space::AddChildSpace(String aLevelName)
  {
    auto newSpace = AddComposition<Space>(aLevelName, mEngine, nullptr);
    newSpace->mOwner = this;

    newSpace->Load(mEngine->GetLevel(aLevelName), this);

    ConnectNodes(mSpace, this);

    auto ourView = GetComponent<GraphicsView>();
    auto newView = newSpace->GetComponent<GraphicsView>();

    if (ourView && newView)
    {
      newView->ChangeWindow(ourView->GetWindow());
    }

    return newSpace;
  }


  // TODO (Josh): Abstract or move to another handler.
  void Space::WindowLostOrGainedFocusHandler(WindowFocusLostOrGained const* aEvent)
  {
    UnusedArguments(aEvent);

    //if ((mFocusHandled == false) && (aEvent->Focused == false) && !mEngine->GetWindow()->IsMinimized())
    //{
    //  //std::cout << "Set mPriorToMinimize" << std::endl;
    //  mPriorToMinimize = mPaused;
    //  mPaused = true;
    //  mFocusHandled = true;
    //}
    //else if ((aEvent->Focused == true) && !mEngine->GetWindow()->IsMinimized())
    //{
    //  mPaused = mPriorToMinimize;
    //  mFocusHandled = false;
    //}
  }

  void Space::WindowMinimizedOrRestoredHandler(WindowMinimizedOrRestored const* aEvent)
  {
    UnusedArguments(aEvent);

    //if (aEvent->Minimized && !mEngine->GetWindow()->IsNotFocused())
    //{
    //  //std::cout << "Set mPriorToMinimize" << std::endl;
    //  mPriorToMinimize = mPaused;
    //  mPaused = true;
    //}
    //else if (!aEvent->Minimized && !mEngine->GetWindow()->IsNotFocused())
    //{
    //  mPaused = mPriorToMinimize;
    //  mFocusHandled = false;
    //}
  }
} // End yte namespace
