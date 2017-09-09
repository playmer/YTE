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



#include "YTE/Physics/PhysicsSystem.h"

#include "YTE/Utilities/Utilities.h"

#include "YTE/Core/Component.hpp"
#include "YTE/Core/Composition.hpp"
#include "YTE/Core/Engine.hpp"
#include "YTE/Core/Space.hpp"

#include "YTE/Physics/RigidBody.h"

namespace YTE
{
  DefineType(Space)
  {
    YTERegisterType(Space);
    YTEAddFunction( &Space::LoadLevel, YTENoOverload, "LoadLevel", YTEParameterNames("aLevel"))->Description()
      = "Loads a level within the current space on the next frame. Current level will be torn down.";

    YTEAddFunction( &Space::SaveLevel, YTENoOverload, "SaveLevel", YTEParameterNames("aLevelName"))->Description()
      = "Saves a level.";

    YTEAddFunction( &Space::Remove, YTENoOverload, "Remove", YTENoNames)->Description()
      = "Removes the Space from the Engine at the start of the next frame.";
      
    YTEBindProperty(&Space::IsPaused, &Space::SetPaused, "Paused")->Description() = "Sets if the space is paused or not.";

    YTEBindProperty(&Space::GetEngine, YTENoSetter, "Engine");

    YTEBindField(&Space::mStartingLevel, "StartingLevel", PropertyBinding::GetSet)->AddAttribute<EditorProperty>();
  }

  // Sets up the Space, probably could just be the constructor.
  Space::Space(Engine *aEngine, RSValue *aProperties)
                : Composition(aEngine, this), mLevelToLoad(nullptr)
  {
    mEngine->GetWindow()->CONNECT(Events::WindowFocusLostOrGained, this, &Space::WindowLostOrGainedFocusHandler);
    mEngine->GetWindow()->CONNECT(Events::WindowMinimizedOrRestored, this, &Space::WindowMinimizedOrRestoredHandler);

    DeserializeByType(aProperties, this, TypeId<Space>());
  }


  // Loads a level into the current Space. If already loaded, destroys 
  // the current Space and loads level in place.
  void Space::Load()
  {
    mLevelName = mStartingLevel;
    Load(mEngine->GetLevel(mStartingLevel));
  }

  // Loads a level into the current Space. If already loaded, destroys 
  // the current Space and loads level in place.
  void Space::Load(RSValue *aLevel)
  {
    mCompositions.Clear();
    mComponents.Clear();
      
    Deserialize(aLevel);
      
    Initialize();
      
    mLoading = false;
  }


  void Space::Initialize()
  {
    Composition::NativeInitialize();
    Composition::PhysicsInitialize();
    Composition::Initialize();

    mShouldIntialize = true;
  }

  // Updates the Space to the current frame.
  void Space::Update(float aDt)
  {
    if (mLoading)
    {
      mLevelName = mLoadingName;
      SetName(mLoadingName);
      Load(mLevelToLoad);
    }

    LogicUpdate updateEvent;
    updateEvent.Dt = aDt;

    SendEvent(Events::DeletionUpdate, &updateEvent);

    SendEvent(Events::FrameUpdate, &updateEvent);

    // Don't send the LogicUpdate Event if the space is paused.
    if (mPaused == false)
    {
      SendEvent(Events::LogicUpdate, &updateEvent);
    }
  }
    
  // Cleans up anything in the Space.
  Space::~Space() {  mCompositions.Clear();  }


  void Space::LoadLevel(String &level)
  {
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
    
    level = L"../levels/" + level + L".json";
    
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
