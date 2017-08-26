/******************************************************************************/
// Joshua T. Fisher
// All content (c) 2016 DigiPen  (USA) Corporation, all rights reserved.
/******************************************************************************/
#pragma once

#ifndef YTE_Core_Space_h
#define YTE_Core_Space_h

#include "YTE/Event/Events.h"
#include "YTE/Event/StandardEvents.h"

#include "YTE/Platform/DeviceEnums.h"

#include "YTE/Core/Composition.hpp"
#include "YTE/Core/ForwardDeclarations.hpp"

namespace YTE
{
  class Space : public Composition
  {
  public:
    DeclareType(Space);
  
    Space(Engine *aEngine, RSValue *aProperties);
    void Load();
    void Load(RSValue *aLevel);
    void Update(float dt);
    ~Space();

    void Initialize() override;
        
    void LoadLevel(String &level);
    void SaveLevel(String &aLevelName);
  
    bool IsPaused() const { return mPaused; };
    void SetPaused(bool aPause) { mPaused = aPause; };

    String& GetLevelName() { return mLevelName; };
  
    RSValue *mLevelToLoad;
  private:
    void WindowLostOrGainedFocusHandler(const WindowFocusLostOrGained *aEvent);
    void WindowMinimizedOrRestoredHandler(const WindowMinimizedOrRestored *aEvent);
      
    bool mPaused = false;
    bool mPriorToMinimize = false;
    bool mFocusHandled = false;

    bool mLoading = true;
    String mLoadingName;
    String mLevelName;
    String mStartingLevel;
  };
}

#endif
