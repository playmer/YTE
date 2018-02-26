/******************************************************************************/
/*!
\author Joshua T. Fisher
\par    email: j.fisher\@digipen.edu
\date   2015-09-19
All content (c) 2016 DigiPen  (USA) Corporation, all rights reserved.
*/
/******************************************************************************/

#include <memory>
#include <filesystem>

#include "YTE/Utilities/Utilities.hpp"

#include "YTE/Core/AssetLoader.hpp"
#include "YTE/Core/ComponentSystem.hpp"
#include "YTE/Core/Engine.hpp"
#include "YTE/Core/Threading/JobHandle.hpp"
#include "YTE/Core/Threading/JobSystem.hpp"
#include "YTE/Core/ScriptBind.hpp"

#include "YTE/Graphics/GraphicsSystem.hpp"

#include "YTE/WWise//WWiseSystem.hpp"

namespace YTE
{
  YTEDefineEvent(LogicUpdate);
  YTEDefineEvent(FrameUpdate);
  YTEDefineEvent(BeginDebugDrawUpdate);
  YTEDefineEvent(DebugDrawUpdate);
  YTEDefineEvent(EndDebugDrawUpdate);
  YTEDefineEvent(DeletionUpdate);
  YTEDefineEvent(BoundTypeChanged);
  YTEDefineEvent(GraphicsDataUpdate);
  YTEDefineEvent(PresentFrame);
  YTEDefineEvent(AnimationUpdate);
  YTEDefineEvent(LogEvent);

  YTEDefineType(LogicUpdate)
  {
    YTERegisterType(LogicUpdate);
    YTEBindField(&LogicUpdate::Dt, "Dt", PropertyBinding::GetSet);
  }

  YTEDefineType(BoundTypeChanged)
  {
    YTERegisterType(BoundTypeChanged);
    YTEBindField(&BoundTypeChanged::aOldType, "OldType", PropertyBinding::GetSet);
    YTEBindField(&BoundTypeChanged::aNewType, "NewType", PropertyBinding::GetSet);
  }

  YTEDefineType(Engine)
  {
    YTERegisterType(Engine);
    YTEBindFunction(&Engine::EndExecution, YTENoOverload, "EndExecution", YTENoNames).Description()
      = "End the execution of the program before the beginning of the next frame.";
    YTEBindProperty(&Engine::GetGamepadSystem, YTENoSetter, "GamepadSystem");
  }

  static String cEngineName{ "Engine" };

  Engine::Engine(const char *aFile, bool aEditorMode)
    : Composition(this, cEngineName, nullptr)
    , mShouldRun(true)
    , mEditorMode(aEditorMode)
    , mFrame(0)
    , mCompositionsByGUID()
    , mComponentsByGUID()
  {
    if constexpr (YTE_CAN_PROFILE)
    {
      profiler::startListen();
    }

    YTEProfileFunction(profiler::colors::Magenta);

    namespace fs = std::experimental::filesystem;
    
    auto enginePath = Path::SetEnginePath(fs::current_path().string());

    fs::path configFilePath{ aFile };
    configFilePath = configFilePath.parent_path();

    Path::SetEnginePath(fs::current_path().parent_path().string());
    Path::SetGamePath(fs::canonical(configFilePath).parent_path().string());

    mBegin = std::chrono::high_resolution_clock::now();
    mLastFrame = mBegin;
    mComponents.Emplace(ComponentSystem::GetStaticType(), std::make_unique<ComponentSystem>(this, nullptr));
    mComponents.Emplace(WWiseSystem::GetStaticType(), std::make_unique<WWiseSystem>(this, nullptr));
    mComponents.Emplace(GraphicsSystem::GetStaticType(), std::make_unique<GraphicsSystem>(this, nullptr));
    mComponents.Emplace(JobSystem::GetStaticType(), std::make_unique<JobSystem>(this, nullptr));

    namespace fs = std::experimental::filesystem;

    fs::path archetypesPath = Path::GetGamePath().String();
    archetypesPath = archetypesPath.parent_path();
    archetypesPath /= L"Archetypes";

    for (auto& file : fs::directory_iterator(archetypesPath))
    {
      String archetypeName = file.path().stem().string();

      mArchetypes.emplace(std::make_pair(archetypeName, std::unique_ptr<RSDocument>(nullptr)));
    }

    fs::path levelsPath = Path::GetGamePath().String();;
    levelsPath = levelsPath.parent_path();
    levelsPath /= L"Levels";

    for (auto& file : fs::directory_iterator(levelsPath))
    { 
      String levelName = file.path().stem().string();

      mLevels.emplace(std::make_pair(levelName, std::unique_ptr<RSDocument>(nullptr)));
    }

    if (nullptr != aFile)
    {
      auto path = GetConfigPath(aFile);
      std::string fileText;
      auto success = ReadFileToString(path, fileText);

      RSDocument document;
      
      if (success && document.Parse(fileText.c_str()).HasParseError())
      {
        std::cout << "Error in Config: " << aFile << std::endl;
      }

      Deserialize(&document);
    }
  }

  void Engine::Deserialize(RSValue *aValue)
  {
    DebugObjection(false == aValue->IsObject(), "We're trying to serialize something that isn't an Engine.");
    DebugObjection(false == aValue->HasMember("Windows") || 
                false == (*aValue)["Windows"].IsObject(), 
                "We're trying to serialize something without Windows.");
    DebugObjection(false == aValue->HasMember("Spaces") ||
                false == (*aValue)["Spaces"].IsObject(), 
                "We're trying to serialize something without Spaces.");

    auto &windows = (*aValue)["Windows"];

    for (auto windowsIt = windows.MemberBegin(); windowsIt  < windows.MemberEnd(); ++windowsIt)
    {
      std::string windowName = windowsIt->name.GetString();

      if (false == mEditorMode)
      {
        mWindows.emplace(windowName, std::make_unique<Window>(this, &windowsIt->value));
      }
      else
      {
        mWindows.emplace(windowName, std::make_unique<Window>(this));
      }
    }

    if (!mEditorMode)
    {
      auto &spaces = (*aValue)["Spaces"];

      for (auto spacesIt = spaces.MemberBegin(); spacesIt < spaces.MemberEnd(); ++spacesIt)
      {
        std::string spaceName = spacesIt->name.GetString();

        mCompositions.Emplace(spaceName, std::make_unique<Space>(this, &spacesIt->value));
      }
    }
  }


  Window* Engine::AddWindow(const char *aName)
  {
    if (false == mEditorMode)
    {
      auto toReturn = mWindows.emplace(aName, std::make_unique<Window>(this, nullptr));

      return toReturn.first->second.get();
    }
    else
    {
      auto toReturn = mWindows.emplace(aName, std::make_unique<Window>(this));

      return toReturn.first->second.get();
    }
  }

  void Engine::RemoveWindow(Window * aWindow)
  {
    for (auto it = mWindows.begin(); it != mWindows.end(); ++it) 
    {
      if (it->second.get() == aWindow)
      {
        mWindows.erase(it);
        return;
      }
    }
  }

  void Engine::Initialize()
  {
    if (mShouldIntialize == false)
    {
      return;
    }

    for (auto &component : mComponents)
    {
      component.second->Initialize();
    }

    for (auto &composition : mCompositions)
    {
      auto space = static_cast<Space*>(composition.second.get());
      space->Load();
      space->Initialize();
    }

    mShouldIntialize = false;
    mIsInitialized = true;

    mDt = 0.016;
  }

  
  // Updates the Space to the current frame.
  void Engine::Update()
  {
    YTEProfileFunction(profiler::colors::Blue);
    using namespace std::chrono;
    duration<double> time_span = duration_cast<duration<double>>(high_resolution_clock::now() - mLastFrame);
    mLastFrame = high_resolution_clock::now();
    mDt = time_span.count();
  
    // TODO (Josh): Should this be like it is?
    if (mDt > 0.5)
    {
      mDt = 0.016f;
    }

    for (auto &window : mWindows)
    {
      SetFrameRate(*window.second, mDt);
      window.second->Update();
    }
    
    LogicUpdate updateEvent;
    updateEvent.Dt = mDt;

    SendEvent(Events::DeletionUpdate, &updateEvent);

    GetComponent<WWiseSystem>()->Update(mDt);
  
    mGamepadSystem.Update(mDt);
  
    for (auto &space : mCompositions)
    {
      space.second->Update(mDt);
    }

    // If we're told to shut down then our windows might be invalidated
    // so we shouldn't try to run the Graphics updates.
    if (false == mShouldRun)
    {
      return;
    }
    
    SendEvent(Events::FrameUpdate, &updateEvent);
    SendEvent(Events::LogicUpdate, &updateEvent);

    // We may also have been told to shut down here.
    if (false == mShouldRun)
    {
      return;
    }

    SendEvent(Events::AnimationUpdate, &updateEvent);
    SendEvent(Events::GraphicsDataUpdate, &updateEvent);
    SendEvent(Events::PresentFrame, &updateEvent);

    ++mFrame;
  }

  void Engine::SetFrameRate(Window &aWindow, double aDt)
  {
    static double totalTime = 0.0;
    totalTime += aDt;

    static u16 counter = 0;
    ++counter;

    if (totalTime >= 0.5)
    {
      std::array<char, 64> buffer;
      sprintf_s(buffer.data(), buffer.size(), "FPS: %f", (counter / 0.5));

      aWindow.SetWindowTitle(buffer.data());

      totalTime = 0.0f;
      counter = 0;
    }
  }

  void Engine::EndExecution()
  {
    mShouldRun = false;
  };

  // Cleans up anything in the Space.
  Engine::~Engine()
  {
    mCompositions.Clear();

    if constexpr (YTE_CAN_PROFILE)
    {
      profiler::stopListen();
    }
  }

  void Engine::Recompile()
  {
  }
  
  RSDocument* Engine::GetArchetype(String &aArchetype)
  {
    if (false == mEditorMode)
    {
      if (auto iter = mArchetypes.find(aArchetype); 
          iter != mArchetypes.end() && iter->second.get() != nullptr)
      {
        return iter->second.get();
      }
    }

    auto path = Path::GetArchetypePath(Path::GetGamePath(), aArchetype.c_str());
    
    std::string fileText;
    auto success = ReadFileToString(path, fileText);

    auto document = std::make_unique<RSDocument>();
    auto toReturn = document.get();

    if (false == success)
    {
      path = Path::GetArchetypePath(Path::GetEnginePath(), aArchetype.c_str());
      fileText.clear();
      success = ReadFileToString(path, fileText);
    }

    if (success && document->Parse(fileText.c_str()).HasParseError())
    {
      std::cout << "Error in Archetype: " << aArchetype << std::endl;
    }
    
    mArchetypes[aArchetype] = std::move(document);
    
    return toReturn;
  }

  std::unordered_map<String, UniquePointer<RSDocument>>* Engine::GetArchetypes()
  {
    return &mArchetypes;
  }
  
  RSDocument* Engine::GetLevel(String &aLevel)
  {
    if (false == mEditorMode)
    {
      if (auto iter = mLevels.find(aLevel);
          iter != mLevels.end() && iter->second.get() != nullptr)
      {
        return iter->second.get();
      }
    }

    auto path = Path::GetLevelPath(Path::GetGamePath(), aLevel.c_str());

    std::string fileText;
    auto success = ReadFileToString(path, fileText);

    auto document = std::make_unique<RSDocument>();
    auto toReturn = document.get();

    if (false == success)
    {
      path = Path::GetLevelPath(Path::GetEnginePath(), aLevel.c_str());

      fileText.clear();
      success = ReadFileToString(path, fileText);
    }

    if (success)
    {
      auto error = document->Parse(fileText.c_str()).GetParseError();

      if (error)
      {
        std::cout << "Error in Level: " << aLevel << ", " << error <<std::endl;
        return nullptr;
      }
    }
    else
    {
      std::cout << "Could not find level " << aLevel << std::endl;
      return nullptr;
    }

    mLevels[aLevel] = std::move(document);

    return toReturn;
  }

  std::unordered_map<String, UniquePointer<RSDocument>>* Engine::GetLevels()
  {
    return &mLevels;
  }

  Composition* Engine::StoreCompositionGUID(Composition *aComposition)
  {
    GlobalUniqueIdentifier &guid = aComposition->GetGUID();
    
    Composition *collision = CheckForCompositionGUIDCollision(guid);

    // GUID has NOT already been used for a composition
    if (!collision)
    {
      mCompositionsByGUID.emplace(std::make_pair(aComposition->GetGUID().ToString(), aComposition));
    }

    return collision;
  }

  Composition* Engine::CheckForCompositionGUIDCollision(GlobalUniqueIdentifier& aGUID)
  {
    auto it = mCompositionsByGUID.find(aGUID.ToString());

    if (it == mCompositionsByGUID.end())
    {
      return nullptr;
    }

    return it->second;
  }

  Composition* Engine::GetCompositionByGUID(GlobalUniqueIdentifier const& aGUID)
  {
    std::string guid = aGUID.ToString();
    auto it = mCompositionsByGUID.find(guid);

    if (it == mCompositionsByGUID.end())
    {
      return nullptr;
    }

    return it->second;
  }

  bool Engine::RemoveCompositionGUID(GlobalUniqueIdentifier const& aGUID)
  {
    if (mCompositionsByGUID.size() == 0)
    {
      return false;
    }

    auto it = mCompositionsByGUID.find(aGUID.ToString());

    if (it == mCompositionsByGUID.end())
    {
      return false;
    }

    mCompositionsByGUID.erase(aGUID.ToString());
    return true;
  }

  Component* Engine::StoreComponentGUID(Component *aComponent)
  {
    GlobalUniqueIdentifier &guid = aComponent->GetGUID();

    Component* collision = CheckForComponentGUIDCollision(guid);

    // GUID has NOT already been used for a composition
    if (!collision)
    {
      mComponentsByGUID.emplace(std::make_pair(aComponent->GetGUID().ToString(), aComponent));
    }

    return collision;
  }

  Component* Engine::CheckForComponentGUIDCollision(GlobalUniqueIdentifier& aGUID)
  {
    auto it = mComponentsByGUID.find(aGUID.ToString());

    if (it == mComponentsByGUID.end())
    {
      return nullptr;
    }

    return it->second;
  }

  Component* Engine::GetComponentByGUID(GlobalUniqueIdentifier const& aGUID)
  {
    std::string guid = aGUID.ToString();
    auto it = mComponentsByGUID.find(guid);

    if (it == mComponentsByGUID.end())
    {
      return nullptr;
    }

    return it->second;
  }

  bool Engine::RemoveComponentGUID(GlobalUniqueIdentifier const& aGUID)
  {
    if (mComponentsByGUID.size() == 0)
    {
      return false;
    }

    auto it = mComponentsByGUID.find(aGUID.ToString());

    if (it == mComponentsByGUID.end())
    {
      return false;
    }

    mComponentsByGUID.erase(aGUID.ToString());
    return true;
  }

  void Engine::Log(LogType aType, std::string_view aLog)
  {
    printf("%*s", static_cast<int>(aLog.size()), &(*aLog.begin()));
    
    LogEvent event;
    event.Log = aLog;
    event.Type = aType;
    SendEvent(Events::LogEvent, &event);
  }
}
