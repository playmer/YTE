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

#include "YTE/Utilities/Utilities.h"

#include "YTE/Core/AssetLoader.hpp"
#include "YTE/Core/ComponentSystem.h"
#include "YTE/Core/Engine.hpp"
#include "YTE/Core/JobSystem.h"
#include "YTE/Core/ScriptBind.hpp"

#include "YTE/Graphics/GraphicsSystem.hpp"

#include "YTE/WWise//WWiseSystem.hpp"

namespace YTE
{
  float DeltaTime::dt = 0.016f; // static dt initialization

  DefineEvent(LogicUpdate);
  DefineEvent(FrameUpdate);
  DefineEvent(BeginDebugDrawUpdate);
  DefineEvent(DebugDrawUpdate);
  DefineEvent(EndDebugDrawUpdate);
  DefineEvent(DeletionUpdate);
  DefineEvent(BoundTypeChanged);

  DefineType(LogicUpdate)
  {
    YTERegisterType(LogicUpdate);
    YTEBindField(&LogicUpdate::Dt, "Dt", PropertyBinding::GetSet);
  }

  DefineType(BoundTypeChanged)
  {
    YTERegisterType(BoundTypeChanged);
    YTEBindField(&BoundTypeChanged::aOldType, "OldType", PropertyBinding::GetSet);
    YTEBindField(&BoundTypeChanged::aNewType, "NewType", PropertyBinding::GetSet);
  }

  DefineType(Engine)
  {
    YTERegisterType(Engine);
    YTEAddFunction( &Engine::EndExecution, YTENoOverload, "EndExecution", YTENoNames)->Description()
      = "End the execution of the program before the beginning of the next frame.";
    YTEBindProperty(&Engine::GetGamepadSystem, YTENoSetter, "GamepadSystem");
  }

  static String cEngineName{ "Engine" };

  Engine::Engine(const char *aFile, bool aEditorMode)
    : Composition(this, nullptr, cEngineName), mShouldRun(true), mEditorMode(aEditorMode)
  {
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

    namespace fs = std::experimental::filesystem;

    fs::path archetypesPath = cWorkingDirectory;
    archetypesPath = archetypesPath.parent_path();
    archetypesPath /= L"archetypes";

    for (auto& file : fs::directory_iterator(archetypesPath))
    {
      String archetypeName = file.path().stem().string();

      mArchetypes.emplace(std::make_pair(archetypeName, std::unique_ptr<RSDocument>(nullptr)));
    }

    fs::path levelsPath = cWorkingDirectory;
    levelsPath = levelsPath.parent_path();
    levelsPath /= L"levels";

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

    auto &spaces = (*aValue)["Spaces"];

    for (auto spacesIt = spaces.MemberBegin(); spacesIt < spaces.MemberEnd(); ++spacesIt)
    {
      std::string spaceName = spacesIt->name.GetString();

      mCompositions.Emplace(spaceName, std::make_unique<Space>(this, &spacesIt->value));
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

    dt.SetDt(0.016f);
  }

  
  // Updates the Space to the current frame.
  void Engine::Update()
  {
    using namespace std::chrono;
    duration<float> time_span = duration_cast<duration<float>>(high_resolution_clock::now() - mLastFrame);
    mLastFrame = high_resolution_clock::now();
    dt.SetDt(time_span.count());
  
    // TODO (Josh): Should this be like it is?
    if (dt.GetDt() > 0.5)
    {
      dt.SetDt(0.016f);
    }

    if (false == mEditorMode)
    {
      for (auto &window : mWindows)
      {
        SetFrameRate(*window.second, dt.GetDt());
        window.second->Update();
      }
    }

    LogicUpdate updateEvent;
    updateEvent.Dt = dt.GetDt();

    SendEvent(Events::DeletionUpdate, &updateEvent);

    GetComponent<WWiseSystem>()->Update(dt.GetDt());
  
    mGamepadSystem.Update(dt.GetDt());
  
    for (auto &space : mCompositions)
    {
      space.second->Update(dt.GetDt());
    }

    SendEvent(Events::LogicUpdate, &updateEvent);
    SendEvent(Events::FrameUpdate, &updateEvent);
  }

  void Engine::SetFrameRate(Window &aWindow, float aDt)
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
  }

  void Engine::Recompile()
  { 
  }
  
  RSDocument* Engine::GetArchetype(String &aArchetype)
  {
    auto iter = mArchetypes.find(aArchetype);
    if (iter != mArchetypes.end() && iter->second.get() != nullptr)
    {
      return iter->second.get();
    }

    auto path = Path::GetArchetypePath(Path::GetGamePath(), aArchetype.c_str());
    
    std::string fileText;
    auto success = ReadFileToString(path, fileText);

    auto document = std::make_unique<RSDocument>();
    auto toReturn = document.get();

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
    auto iter = mLevels.find(aLevel);
    if (iter != mLevels.end() && iter->second.get() != nullptr)
    {
      return iter->second.get();
    }

    auto path = Path::GetLevelPath(Path::GetGamePath(), aLevel.c_str());

    std::string fileText;
    auto success = ReadFileToString(path, fileText);

    auto document = std::make_unique<RSDocument>();
    auto toReturn = document.get();

    
    if (success)
    {
      auto error = document->Parse(fileText.c_str()).GetParseError();

      if (error)
      {
        std::cout << "Error in Level: " << aLevel << ", " << error <<std::endl;
      }
    }

    mLevels[aLevel] = std::move(document);

    return toReturn;
  }

  std::unordered_map<String, UniquePointer<RSDocument>>* Engine::GetLevels()
  {
    return &mLevels;
  }
}
