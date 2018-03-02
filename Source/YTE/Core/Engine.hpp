/******************************************************************************/
/*!
\file   Engine.hpp
\author Joshua T. Fisher
\par    email: j.fisher\@digipen.edu
\date   2014-04-30
\brief
  This file contains Space, which is what a level will be loaded into.
  All content (c) 2016 DigiPen  (USA) Corporation, all rights reserved.
*/
/******************************************************************************/
#pragma once

#ifndef YTE_Core_Engine_hpp
#define YTE_Core_Engine_hpp

#include <unordered_map>
#include <chrono>


#include "YTE/Core/Composition.hpp"
#include "YTE/Core/ForwardDeclarations.hpp"
#include "YTE/Core/Space.hpp"

#include "YTE/Platform/GamepadSystem.hpp"
#include "YTE/Platform/Window.hpp"

#include "YTE/StandardLibrary/Vector.hpp"

#include "YTE/Utilities/Utilities.hpp"


namespace YTE
{
  YTEDeclareEvent(LogicUpdate);
  YTEDeclareEvent(PhysicsUpdate);
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

    BoundType *aOldType;
    BoundType *aNewType;
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
    
    Engine(const char *aFile = nullptr, bool aEditorMode = false);
    void Update();
    ~Engine();

    void Initialize(bool) override;
    void Initialize() { Initialize(true); }
    void Deserialize(RSValue *aValue);

    Window* AddWindow(const char *aName);
    void RemoveWindow(Window *aWindow);

    bool KeepRunning() const { return mShouldRun; };

    void EndExecution();

    void Recompile();

    void SetFrameRate(Window &aWindow, double aDt);

    Window* GetWindow()
    {
      if (mWindows.size())
      {
        return mWindows.begin()->second.get();
      }
      
      return nullptr;
    }


    std::unordered_map<std::string, std::unique_ptr<Window>>& GetWindows()
    {
      return mWindows;
    }


    inline GamepadSystem *GetGamepadSystem() { return &mGamepadSystem; }
    RSDocument* GetArchetype(String &aArchetype);
    std::unordered_map<String, UniquePointer<RSDocument>>* GetArchetypes(void);
    RSDocument* GetLevel(String &aLevel);
    std::unordered_map<String, UniquePointer<RSDocument>>* GetLevels(void);

    OrderedMultiMap<Composition*, UniquePointer<Composition>> mCompositionsToRemove;
    OrderedMultiMap<Composition*, ComponentMap::iterator> mComponentsToRemove;

    bool IsEditor()
    {
      return mEditorMode;
    }

    double GetDt()
    {
      return mDt;
    }

    size_t GetFrame() { return mFrame; }

    Composition* StoreCompositionGUID(Composition *aComposition);
    Composition* CheckForCompositionGUIDCollision(GlobalUniqueIdentifier &aGUID);
    Composition* GetCompositionByGUID(GlobalUniqueIdentifier const& aGUID);
    bool RemoveCompositionGUID(GlobalUniqueIdentifier const& aGUID);

    Component* StoreComponentGUID(Component *aComponent);
    Component* CheckForComponentGUIDCollision(GlobalUniqueIdentifier &aGUID);
    Component* GetComponentByGUID(GlobalUniqueIdentifier const& aGUID);
    bool RemoveComponentGUID(GlobalUniqueIdentifier const& aGUID);

    void Log(LogType aType, std::string_view aLog);


  private:
    GamepadSystem mGamepadSystem;

    std::unordered_map<std::string, std::unique_ptr<Window>> mWindows;

    std::unordered_map<String, UniquePointer<RSDocument>> mArchetypes;
    std::unordered_map<String, UniquePointer<RSDocument>> mLevels;

    std::chrono::time_point<std::chrono::high_resolution_clock> mBegin;
    std::chrono::time_point<std::chrono::high_resolution_clock> mLastFrame;

    bool mShouldRun;
    bool mEditorMode;

    // all compositions and components mapped to GUIDs (represented as strings)
    std::unordered_map<std::string, Composition*> mCompositionsByGUID;
    std::unordered_map<std::string, Component*> mComponentsByGUID;

    double mDt;
    size_t mFrame;
  };
}

#endif
