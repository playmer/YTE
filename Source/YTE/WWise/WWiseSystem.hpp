/******************************************************************************/
/*!
 * \author Joshua T. Fisher
 * \date   2015-09-19
 *
 * \copyright All content 2016 DigiPen (USA) Corporation, all rights reserved.
 */
/******************************************************************************/
#pragma once

#ifndef YTE_WWise_WWiseSystem_h
#define YTE_WWise_WWiseSystem_h

#include <unordered_map>
#include <vector>

#include "AK/SoundEngine/Common/AkTypes.h"

#include "YTE/Core/Component.hpp"

#include "YTE/Meta/Type.hpp"

#include "YTE/Utilities/String/String.h"

namespace YTE
{
  struct AudioBank
  {
    std::vector<std::string> mEvents;
    std::vector<std::string> mSwitchGroup;

    //All the Game param in the bank file itself in strings
    std::vector<std::string> mGameParameters;

    AkBankID mBankID;
    //1st = Switch Group, 2nd = Switches
    std::unordered_map <std::string, std::vector<std::string>> mSwitches;
  };

  class WWiseSystem : public Component
  {
  public:
    YTEDeclareType(WWiseSystem);

    // Sets up the system, probably could just be the constructor.

    WWiseSystem(Composition *aOwner, RSValue *aProperties);

    virtual void Initialize() override;
    // Updates the system to the current frame.
    void Update(float);

    // Cleans up anything in the system.
    virtual ~WWiseSystem();

    void LoadAllBanks();
    bool LoadBank(const std::string &aFilename);
    bool LoadJson(const std::string &aFilename);

    void UnloadBank(const std::string &aBankName);
    void UnloadAllBanks();

    void SendEvent(const std::string &aEvent, AkGameObjectID aId);
    void SetRTPC(const std::string &aRTPC, float aValue);
    float GetRTPC(const std::string &aRTPC);

    bool GetMute();
    void SetMute(bool aMute);

  private:
    void WindowLostOrGainedFocusHandler(const WindowFocusLostOrGained *aEvent);
    void WindowMinimizedOrRestoredHandler(const WindowMinimizedOrRestored *aEvent);

    std::unordered_map<std::string, float> mRTPCs;
    std::unordered_map<std::string, AudioBank> mBanks;

    bool mPaused = false;
    bool mPriorToMinimize = false;
    bool mFocusHandled = false;

    bool mMuted;

    bool mMinimized = false;
    bool mFocused = false;
  };
}

#endif