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

#include "YTE/Core/ComponentSystem.h"
#include "YTE/Core/Engine.hpp"
#include "YTE/Core/JobSystem.h"
#include "YTE/Core/ScriptBind.hpp"

#include "YTE/Graphics/GraphicsSystem.hpp"

namespace YTE
{
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
    DebugAssert(false == aValue->IsObject(), "We're trying to serialize something that isn't an Engine.");
    DebugAssert(false == aValue->HasMember("Windows") || 
                false == (*aValue)["Windows"].IsObject(), 
                "We're trying to serialize something without Windows.");
    DebugAssert(false == aValue->HasMember("Spaces") ||
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
  }

  
  // Updates the Space to the current frame.
  void Engine::Update()
  {
    using namespace std::chrono;
    duration<float> time_span = duration_cast<duration<float>>(high_resolution_clock::now() - mLastFrame);
    mLastFrame = high_resolution_clock::now();
    auto dt = time_span.count();;
  
    // TODO (Josh): Should this be like it is?
    if (dt > 0.5)
    {
      dt = 0.016f;
    }

    if (false == mEditorMode)
    {
      for (auto &window : mWindows)
      {
        SetFrameRate(*window.second, dt);
        window.second->Update();
      }
    }

    LogicUpdate updateEvent;
    updateEvent.Dt = dt;

    Trigger(Events::DeletionUpdate, &updateEvent);

    GetComponent<WWiseSystem>()->Update(dt);
  
    mGamepadSystem.Update(dt);
  
    for (auto &space : mCompositions)
    {
      space.second->Update(dt);
    }

    Trigger(Events::LogicUpdate, &updateEvent);
    Trigger(Events::FrameUpdate, &updateEvent);
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

    auto path = GetArchetypePath(aArchetype);
    
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

    auto path = GetLevelPath(aLevel);

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
