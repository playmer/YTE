#pragma once

#ifndef YTE_Core_Engine_hpp
#define YTE_Core_Engine_hpp

#include <unordered_map>
#include <chrono>


#include "YTE/Core/Composition.hpp"
#include "YTE/Core/ForwardDeclarations.hpp"
#include "YTE/Core/Plugin.hpp"
#include "YTE/Core/Space.hpp"

#include "YTE/Platform/PlatformManager.hpp"

#include "YTE/Utilities/Utilities.hpp"


namespace YTE
{
  YTEDeclareEvent(LogicUpdate);
  YTEDeclareEvent(PhysicsUpdate);
  YTEDeclareEvent(PreLogicUpdate);
  YTEDeclareEvent(PreFrameUpdate);
  YTEDeclareEvent(FrameUpdate);
  YTEDeclareEvent(BeginDebugDrawUpdate);
  YTEDeclareEvent(DebugDrawUpdate);
  YTEDeclareEvent(EndDebugDrawUpdate);
  YTEDeclareEvent(DeletionUpdate);
  YTEDeclareEvent(BoundTypeChanged);
  YTEDeclareEvent(GraphicsDataUpdate);
  YTEDeclareEvent(PresentFrame);
  YTEDeclareEvent(AnimationUpdate);

  class LogicUpdate : public Event
  {
  public:
    YTEDeclareType(LogicUpdate);

    double Dt;
  };

  class BoundTypeChanged : public Event
  {
  public:
    YTEDeclareType(BoundTypeChanged);

    Type *aOldType;
    Type *aNewType;
  };

  enum class LogType
  {
    Success,
    Information,
	  Warning,
	  PerformanceWarning,
	  Error
  };

  class LogEvent : public Event
  {
  public:
    std::string_view Log;
    LogType Type;
  };

  YTEDeclareEvent(LogEvent);

  class Engine : public Composition
  {
  public:
    YTEDeclareType(Engine);
    
    YTE_Shared Engine(std::vector<const char *> aConfigFilePath, bool aEditorMode = false);
    YTE_Shared void Update();
    YTE_Shared ~Engine();

    YTE_Shared void Initialize(InitializeEvent*) override;
    void Initialize()
    {
      YTEProfileFunction();
      InitializeEvent event;
      event.CheckRunInEditor = true;
      Initialize(&event);
    }


    // Plugins
    YTE_Shared void LoadPlugins();



    YTE_Shared void Deserialize(RSValue *aValue);

    YTE_Shared Window* AddWindow(const char *aName);
    YTE_Shared void RemoveWindow(Window *aWindow);

    bool KeepRunning() const { return mShouldRun; };
    
    YTE_Shared void EndExecution();
    YTE_Shared void Recompile();


    bool IsInitialized()
    {
      return mInitialized;
    }

    YTE_Shared void SetFrameRate(Window &aWindow, double aDt);

    Window* GetWindow()
    {
      auto window = mPlatform.GetPrimaryWindow();
      
      if (window.has_value())
      {
        return *window;
      }
      
      return nullptr;
    }


    std::unordered_map<std::string, std::unique_ptr<Window>>& GetWindows()
    {
      return mPlatform.GetWindows();
    }


    inline PlatformManager* GetPlatformManager() { return &mPlatform; }
    YTE_Shared RSDocument* GetArchetype(String &aArchetype);
    YTE_Shared std::unordered_map<String, UniquePointer<RSDocument>>* GetArchetypes(void);
    YTE_Shared RSDocument* GetLevel(String &aLevel);
    YTE_Shared std::unordered_map<String, UniquePointer<RSDocument>>* GetLevels(void);

    bool IsEditor()
    {
      return mEditorMode;
    }

    double GetDt()
    {
      return mDt;
    }

    size_t GetFrame() { return mFrame; }

    YTE_Shared Composition* StoreCompositionGUID(Composition *aComposition);
    YTE_Shared Composition* CheckForCompositionGUIDCollision(GlobalUniqueIdentifier const& aGUID);
    YTE_Shared Composition* GetCompositionByGUID(GlobalUniqueIdentifier const& aGUID);
    YTE_Shared bool RemoveCompositionGUID(GlobalUniqueIdentifier const& aGUID);

    YTE_Shared Component* StoreComponentGUID(Component *aComponent);
    YTE_Shared Component* CheckForComponentGUIDCollision(GlobalUniqueIdentifier const& aGUID);
    YTE_Shared Component* GetComponentByGUID(GlobalUniqueIdentifier const& aGUID);
    YTE_Shared bool RemoveComponentGUID(GlobalUniqueIdentifier const& aGUID);

    YTE_Shared void Log(LogType aType, std::string_view aLog);

    YTE_Shared ComponentSystem* GetComponentSystem()
    {
      return &mComponentSystem;
    }

    OrderedMultiMap<Composition*, std::unique_ptr<Composition>> mCompositionsToRemove;
    OrderedMultiMap<Composition*, ComponentMap::iterator> mComponentsToRemove;

  private:
    // all compositions and components mapped to GUIDs (represented as strings)
    std::unordered_map<std::string, Composition*> mCompositionsByGUID;
    std::unordered_map<std::string, Component*> mComponentsByGUID;

    PlatformManager mPlatform;
    ComponentSystem mComponentSystem;

    std::unordered_map<String, UniquePointer<RSDocument>> mArchetypes;
    std::unordered_map<String, UniquePointer<RSDocument>> mLevels;

    std::chrono::time_point<std::chrono::high_resolution_clock> mBegin;
    std::chrono::time_point<std::chrono::high_resolution_clock> mLastFrame;

    std::unordered_map<std::string, std::unique_ptr<PluginWrapper>> mPlugins;

    double mDt;
    size_t mFrame;
    bool mShouldRun;
    bool mEditorMode;
    bool mInitialized;
  };
}

#endif
