#pragma once

#ifndef YTE_Core_Space_hpp
#define YTE_Core_Space_hpp

#include "YTE/Core/EventHandler.hpp"

#include "YTE/Platform/DeviceEnums.hpp"
#include "YTE/Platform/Window.hpp"

#include "YTE/Core/Composition.hpp"
#include "YTE/Core/ForwardDeclarations.hpp"

namespace YTE
{
  YTEDeclareEvent(SpaceUpdate);

  class Space : public Composition
  {
  public:
    YTEDeclareType(Space);
  
    YTE_Shared Space(Engine *aEngine, RSValue *aProperties);
    YTE_Shared void Load();
    YTE_Shared void Load(RSValue *aLevel, bool aInitialize = true);
    YTE_Shared void Update(LogicUpdate *aEvent);
    YTE_Shared ~Space();

    YTE_Shared void Initialize(InitializeEvent *aEvent) override;
    YTE_Shared void Initialize();
        
    YTE_Shared void CreateBlankLevel(const String& aLevelName);
    YTE_Shared void LoadLevel(String &level, bool aCheckRunInEditor = false);
    YTE_Shared void SaveLevel(String &aLevelName);

    YTE_Shared Space* AddChildSpace(String aLevelName);
  
    YTE_Shared bool IsPaused() const { return mPaused; };
    YTE_Shared void SetPaused(bool aPause) { mPaused = aPause; };

    YTE_Shared String& GetLevelName() { return mLevelName; };

    YTE_Shared bool GetIsEditorSpace() { return mIsEditorSpace; }
    YTE_Shared void SetIsEditorSpace(bool aIsEditorSpace) { mIsEditorSpace = aIsEditorSpace; }
  
    RSValue *mLevelToLoad;

    bool GetFinishedLoading()
    {
      return mFinishedLoading;
    }

  private:
    void WindowLostOrGainedFocusHandler(const WindowFocusLostOrGained *aEvent);
    void WindowMinimizedOrRestoredHandler(const WindowMinimizedOrRestored *aEvent);

    static void ConnectNodes(Space* aSpace, Composition* aComposition);

    String mLoadingName;
    String mLevelName;
    String mStartingLevel;

    IntrusiveList<Composition> mAssetInitialize;
    IntrusiveList<Composition> mNativeInitialize;
    IntrusiveList<Composition> mPhysicsInitialize;
    IntrusiveList<Composition> mInitialize;
    IntrusiveList<Composition> mStart;
      
    bool mPaused = false;
    bool mPriorToMinimize = false;
    bool mFocusHandled = false;
    bool mIsEditorSpace = false;

    bool mFinishedLoading = true;

    bool mLoading = true;
    bool mCheckRunInEditor = false;
  };
}

#endif
