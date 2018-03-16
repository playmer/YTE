/******************************************************************************/
/*!
\author Joshua T. Fisher
\par    email: j.fisher\@digipen.edu
\date   2015-10-26
All content (c) 2016 DigiPen  (USA) Corporation, all rights reserved.
*/
/******************************************************************************/
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

#include "YTE/WWise/WWiseView.hpp"

namespace YTE
{
  YTEDefineType(Space)
  {
    YTERegisterType(Space);
    YTEBindFunction(&Space::LoadLevel, YTENoOverload, "LoadLevel", YTEParameterNames("aLevel", "aCheckRunInEditor")).Description()
      = "Loads a level within the current space on the next frame. Current level will be torn down.";

    YTEBindFunction(&Space::SaveLevel, YTENoOverload, "SaveLevel", YTEParameterNames("aLevelName")).Description()
      = "Saves a level.";

    YTEBindFunction(&Space::Remove, YTENoOverload, "Remove", YTENoNames).Description()
      = "Removes the Space from the Engine at the start of the next frame.";
      
    YTEBindProperty(&Space::IsPaused, &Space::SetPaused, "Paused").Description() = "Sets if the space is paused or not.";

    YTEBindProperty(&Space::GetEngine, YTENoSetter, "Engine");

    YTEBindField(&Space::mStartingLevel, "StartingLevel", PropertyBinding::GetSet)
      .AddAttribute<EditorProperty>()
      .AddAttribute<Serializable>();
  }


  Space::Space(Engine *aEngine, RSValue *aProperties)
    : Composition(aEngine, this)
    , mLevelToLoad(nullptr)
  {
    if (false == mEngine->IsEditor())
    {
      mEngine->GetWindow()->YTERegister(Events::WindowFocusLostOrGained, 
                                        this, 
                                        &Space::WindowLostOrGainedFocusHandler);
      mEngine->GetWindow()->YTERegister(Events::WindowMinimizedOrRestored, 
                                        this, 
                                        &Space::WindowMinimizedOrRestoredHandler);
    }


    mEngine->YTERegister(Events::LogicUpdate, this, &Space::Update);

    if (nullptr != aProperties)
    {
      DeserializeByType(aProperties, this, TypeId<Space>());
    }
  }

  void Space::Initialize()
  {
    InitializeEvent event;
    event.CheckRunInEditor = mEngine->IsEditor();
    Initialize(&event);
  }

  // Loads a level into the current Space. If already loaded, destroys 
  // the current Space and loads level in place.
  void Space::Load()
  {
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
  void Space::Load(RSValue *aLevel, bool aInitialize)
  {
    mCompositions.Clear();
    mComponents.Clear();
      
    if (nullptr != aLevel)
    {
      Deserialize(aLevel);
    }
    else
    {
      printf("We could not deserialize the level provided.\n");
    }
      
    if (aInitialize)
    {
      Initialize();
    }
      
    mLoading = false;
  }


  void Space::Initialize(InitializeEvent *aEvent)
  {
    Composition::NativeInitialize(aEvent);
    Composition::PhysicsInitialize(aEvent);
    Composition::Initialize(aEvent);

    mShouldIntialize = true;
  }

  // Updates the Space to the current frame.
  void Space::Update(LogicUpdate *aEvent)
  {
    YTEProfileFunction(profiler::colors::Amber);
    if (mLoading)
    {
      mLevelName = mLoadingName;
      SetName(mLoadingName);
      Load(mLevelToLoad);
    }

    SendEvent(Events::DeletionUpdate, aEvent);
      
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
    mCompositions.Clear();
    mComponents.Clear();
  }

  void Space::CreateBlankLevel(const String& aLevelName)
{
    mCompositions.Clear();
    mComponents.Clear();

    mLevelName = aLevelName;

    AddComponent<WWiseView>();
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

  void Space::LoadLevel(String &level, bool aCheckRunInEditor)
  {
    mCheckRunInEditor = aCheckRunInEditor;
    mLoading = true;
    mLevelToLoad = mEngine->GetLevel(level);
    mLoadingName = level;
  }

  void Space::SaveLevel(String &aLevelName)
  {
    // TODO (Josh): Fix
    RSAllocator allocator;
    auto value = Serialize(allocator);

    std::string levelNameTemp(aLevelName.c_str());
    std::wstring level{levelNameTemp.begin(), levelNameTemp.end()};

    std::string path = YTE::Path::GetGamePath().String();
    std::wstring pathWStr{ path.begin(), path.end() };
    
    level = pathWStr + L"Levels/" + level + L".json";
    
    level = std::experimental::filesystem::canonical(level, cWorkingDirectory);
    
    RSStringBuffer sb;
    RSPrettyWriter writer(sb);
    value.Accept(writer);    // Accept() traverses the DOM and generates Handler events.
    std::string levelInJson = sb.GetString();
    
    std::ofstream levelToSave;
    levelToSave.open(level);
    levelToSave << levelInJson;
    levelToSave.close();
  }


  Space* Space::AddChildSpace(String aLevelName)
  {
    auto newSpace = AddComposition<Space>(aLevelName, mEngine, nullptr);
    newSpace->Load(mEngine->GetLevel(aLevelName));
    auto ourView = GetComponent<GraphicsView>();
    auto newView = newSpace->GetComponent<GraphicsView>();

    if (ourView && newView)
    {
      newView->ChangeWindow(ourView->GetWindow());
    }

    return newSpace;
  }


  // TODO (Josh): Abstract or move to another handler.
  void Space::WindowLostOrGainedFocusHandler(const WindowFocusLostOrGained *aEvent)
  {
    if ((mFocusHandled == false) && (aEvent->Focused == false) && !mEngine->GetWindow()->IsMinimized())
    {
      //std::cout << "Set mPriorToMinimize" << std::endl;
      mPriorToMinimize = mPaused;
      mPaused = true;
      mFocusHandled = true;
    }
    else if ((aEvent->Focused == true) && !mEngine->GetWindow()->IsMinimized())
    {
      mPaused = mPriorToMinimize;
      mFocusHandled = false;
    }
  }

  void Space::WindowMinimizedOrRestoredHandler(const WindowMinimizedOrRestored *aEvent)
  {
    if (aEvent->Minimized && !mEngine->GetWindow()->IsNotFocused())
    {
      //std::cout << "Set mPriorToMinimize" << std::endl;
      mPriorToMinimize = mPaused;
      mPaused = true;
    }
    else if (!aEvent->Minimized && !mEngine->GetWindow()->IsNotFocused())
    {
      mPaused = mPriorToMinimize;
      mFocusHandled = false;
    }
  }
} // End yte namespace
