/******************************************************************************/
// Joshua T. Fisher
// All content (c) 2016 DigiPen  (USA) Corporation, all rights reserved.
/******************************************************************************/
#pragma once

#ifndef YTE_Core_Space_h
#define YTE_Core_Space_h

#include "YTE/Core/EventHandler.hpp"

#include "YTE/Platform/DeviceEnums.hpp"
#include "YTE/Platform/Window.hpp"

#include "YTE/Core/Composition.hpp"
#include "YTE/Core/ForwardDeclarations.hpp"

namespace YTE
{
  class Space : public Composition
  {
  public:
    YTEDeclareType(Space);
  
    Space(Engine *aEngine, RSValue *aProperties);
    void Load();
    void Load(RSValue *aLevel, bool aInitialize = true);
    void Update(LogicUpdate *aEvent);
    ~Space();

    void Initialize(InitializeEvent *aEvent) override;
    void Initialize();
        
    void CreateBlankLevel(const String& aLevelName);
    void LoadLevel(String &level, bool aCheckRunInEditor = false);
    void SaveLevel(String &aLevelName);

    Space* AddChildSpace(String aLevelName);
  
    bool IsPaused() const { return mPaused; };
    void SetPaused(bool aPause) { mPaused = aPause; };

    String& GetLevelName() { return mLevelName; };

    bool GetIsEditorSpace() { return mIsEditorSpace; }
    void SetIsEditorSpace(bool aIsEditorSpace) { mIsEditorSpace = aIsEditorSpace; }
  
    RSValue *mLevelToLoad;

    bool GetFinishedLoading()
    {
      return mFinishedLoading;
    }

  private:
    void WindowLostOrGainedFocusHandler(const WindowFocusLostOrGained *aEvent);
    void WindowMinimizedOrRestoredHandler(const WindowMinimizedOrRestored *aEvent);
      
    bool mPaused = false;
    bool mPriorToMinimize = false;
    bool mFocusHandled = false;
    bool mIsEditorSpace = false;

    bool mFinishedLoading = true;

    bool mLoading = true;
    bool mCheckRunInEditor = false;
    String mLoadingName;
    String mLevelName;
    String mStartingLevel;
  };
}

#endif
