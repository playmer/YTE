#include <iostream>
#include <fstream>
#include <memory>
#include "YTE/StandardLibrary/FileSystem.hpp"

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
  namespace fs = std::filesystem;

  YTEDefineEvent(LogicUpdate);
  YTEDefineEvent(PhysicsUpdate);
  YTEDefineEvent(PreLogicUpdate);
  YTEDefineEvent(PreFrameUpdate);
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
    RegisterType<LogicUpdate>();
    TypeBuilder<LogicUpdate> builder;
    builder.Field<&LogicUpdate::Dt>("Dt", PropertyBinding::GetSet);
  }

  YTEDefineType(BoundTypeChanged)
  {
    RegisterType<BoundTypeChanged>();
    TypeBuilder<BoundTypeChanged> builder;
    builder.Field<&BoundTypeChanged::aOldType>("OldType", PropertyBinding::GetSet);
    builder.Field<&BoundTypeChanged::aNewType>("NewType", PropertyBinding::GetSet);
  }

  YTEDefineType(Engine)
  {
    RegisterType<Engine>();
    TypeBuilder<Engine> builder;
    builder.Function<&Engine::EndExecution>("EndExecution")
      .SetDocumentation("End the execution of the program before the beginning of the next frame.");
    builder.Property<&Engine::GetGamepadSystem, NoSetter>("GamepadSystem");
  }

  static String cEngineName{ "Engine" };

  Engine::Engine(std::vector<const char *> aConfigFilePath, bool aEditorMode)
    : Composition{ this, cEngineName, nullptr }
    , mCompositionsByGUID{}
    , mComponentsByGUID{}
    , mPlatform{ this }
    , mComponentSystem{ this }
    , mFrame{ 0 }
    , mShouldRun{ true }
    , mEditorMode{ aEditorMode }
    , mInitialized{ false }
  {
    namespace fs = std::filesystem;

    if constexpr (YTE_CAN_PROFILE)
    {
      //EASY_PROFILER_ENABLE;
      profiler::startListen();
    }

    const char *pathToUse{ nullptr };

    for (auto path : aConfigFilePath)
    {
      auto file = GetConfigPath(path);

      if (fs::exists(file))
      {
        pathToUse = path;
        break;
      }
    }

    YTEProfileFunction();
    
    auto enginePath = Path::SetEnginePath(fs::current_path().string());

    fs::path configFilePath{ pathToUse };
    configFilePath = configFilePath.parent_path();

    Path::SetEnginePath(fs::current_path().parent_path().string());
    Path::SetGamePath(fs::canonical(configFilePath).parent_path().string());

    mBegin = std::chrono::high_resolution_clock::now();
    mLastFrame = mBegin;

    mComponents.Emplace(TypeId<JobSystem>(), std::make_unique<JobSystem>(this));
    mComponents.Emplace(TypeId<WWiseSystem>(), std::make_unique<WWiseSystem>(this));
    mComponents.Emplace(TypeId<GraphicsSystem>(), std::make_unique<GraphicsSystem>(this));

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

    LoadPlugins();

    if (nullptr != pathToUse)
    {
      auto path = GetConfigPath(pathToUse);
      std::string fileText;
      auto success = ReadFileToString(path, fileText);

      RSDocument document;
      
      if (success && document.Parse(fileText.c_str()).HasParseError())
      {
        std::cout << "Error in Config: " << pathToUse << std::endl;
      }

      Deserialize(&document);
    }
  }

  void Engine::Deserialize(RSValue *aValue)
  {
    YTEProfileFunction();

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

      auto& platformWindows = GetWindows();

      if (false == mEditorMode)
      {
        platformWindows.emplace(windowName, std::make_unique<Window>(this, &windowsIt->value));
      }
      else
      {
        platformWindows.emplace(windowName, std::make_unique<Window>(this));
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
    YTEProfileFunction();

    auto& windows = GetWindows();

    if (false == mEditorMode)
    {
      auto toReturn = windows.emplace(aName, std::make_unique<Window>(this, nullptr));
      toReturn.first->second->mName = aName;

      return toReturn.first->second.get();
    }
    else
    {
      auto toReturn = windows.emplace(aName, std::make_unique<Window>(this));
      toReturn.first->second->mName = aName;

      return toReturn.first->second.get();
    }
  }

  void Engine::RemoveWindow(Window * aWindow)
  {
    YTEProfileFunction();

    auto& windows = GetWindows();

    for (auto it = windows.begin(); it != windows.end(); ++it)
    {
      if (it->second.get() == aWindow)
      {
        windows.erase(it);
        return;
      }
    }
  }

  void Engine::Initialize(InitializeEvent*)
  {
    YTEProfileFunction();

    for (auto &component : mComponents)
    {
      component.second->Initialize();
    }

    for (auto &composition : mCompositions)
    {
      auto space = static_cast<Space*>(composition.second.get());
      space->Load();
    }

    mDt = 0.016;

    mInitialized = true;
  }

  
  // Updates the Space to the current frame.
  void Engine::Update()
  {
    YTEProfileFunction();
    using namespace std::chrono;
    duration<double> time_span = duration_cast<duration<double>>(high_resolution_clock::now() - mLastFrame);
    mLastFrame = high_resolution_clock::now();
    mDt = time_span.count();
  
    // TODO (Josh): Should this be like it is?
    if (mDt > 0.5)
    {
      mDt = 0.016;
    }

    for (auto &window : GetWindows())
    {
      SetFrameRate(*window.second, mDt);
    }

    mPlatform.Update();
    
    LogicUpdate updateEvent;
    updateEvent.Dt = mDt;

    SendEvent(Events::DeletionUpdate, &updateEvent);

    SendEvent(Events::AnimationUpdate, &updateEvent);

    GetComponent<WWiseSystem>()->Update(mDt);
  
    mGamepadSystem.Update(mDt);

    SendEvent(Events::PreLogicUpdate, &updateEvent);
    SendEvent(Events::LogicUpdate, &updateEvent);
    SendEvent(Events::SpaceUpdate, &updateEvent);

    // If we're told to shut down then our windows might be invalidated
    // so we shouldn't try to run the Graphics updates.
    if (false == mShouldRun)
    {
      return;
    }

    SendEvent(Events::PreFrameUpdate, &updateEvent);
    SendEvent(Events::GraphicsDataUpdate, &updateEvent);
    SendEvent(Events::FrameUpdate, &updateEvent);
    SendEvent(Events::PresentFrame, &updateEvent);

    // We may also have been told to shut down here.
    if (false == mShouldRun)
    {
      return;
    }

    ++mFrame;
  }

  // TODO: (Josh) Implement in some other more acceptable way. On by default imgui metrics window?
  void Engine::SetFrameRate(Window &aWindow, double aDt)
  {
    UnusedArguments(aWindow);
    static double totalTime = 0.0;
    totalTime += aDt;

    static u16 counter = 0;
    ++counter;

    if (totalTime >= 0.5)
    {
      // FPS counter to the window name 
      std::array<char, 256> buffer;
      sprintf(buffer.data(), "FPS: %f", (counter / 0.5));
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
    YTEProfileFunction();

    mCompositions.Clear();

    mComponents.Clear();

    mPlugins.clear();

    if constexpr (YTE_CAN_PROFILE)
    {
      profiler::stopListen();
    }
  }

  void Engine::LoadPlugins()
  {
    YTEProfileFunction();

    fs::path gamePath = Path::GetGamePath().String();
    gamePath /= "Plugins";

    if constexpr(CompilerConfiguration::Debug())
    {
      gamePath /= "Debug";
    }
    else if constexpr(CompilerConfiguration::MinSizeRel())
    {
      gamePath /= "MinSizeRel";
    }
    else if constexpr(CompilerConfiguration::RelWithDebInfo())
    {
      gamePath /= "RelWithDebInfo";
    }
    else if constexpr(CompilerConfiguration::Release())
    {
      gamePath /= "Release";
    }
    else if constexpr(CompilerConfiguration::Publish())
    {
      gamePath /= "Publish";
    }

    std::error_code error;
    fs::create_directories(gamePath, error);


    for (auto& itemIt : fs::directory_iterator(gamePath))
    {
      fs::path item{ itemIt };

      auto extension = item.extension();
      
      if (".dll" == extension)
      {
        auto path = item.u8string();
        mPlugins[path] = std::make_unique<PluginWrapper>(this, path);
      }
    }
  }

  void Engine::Recompile()
  {

  }
  
  RSDocument* Engine::GetArchetype(String &aArchetype)
  {
    YTEProfileFunction();

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
    YTEProfileFunction();

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
    YTEProfileFunction();

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
    YTEProfileFunction();

    auto it = mCompositionsByGUID.find(aGUID.ToString());

    if (it == mCompositionsByGUID.end())
    {
      return nullptr;
    }

    return it->second;
  }

  Composition* Engine::GetCompositionByGUID(GlobalUniqueIdentifier const& aGUID)
  {
    YTEProfileFunction();

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
    YTEProfileFunction();

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
    YTEProfileFunction();

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
    YTEProfileFunction();

    auto it = mComponentsByGUID.find(aGUID.ToString());

    if (it == mComponentsByGUID.end())
    {
      return nullptr;
    }

    return it->second;
  }

  Component* Engine::GetComponentByGUID(GlobalUniqueIdentifier const& aGUID)
  {
    YTEProfileFunction();

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
    YTEProfileFunction();

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
    printf("%*s\n", static_cast<int>(aLog.size()), &(*aLog.begin()));
    
    LogEvent event;
    event.Log = aLog;
    event.Type = aType;
    SendEvent(Events::LogEvent, &event);
  }
}
