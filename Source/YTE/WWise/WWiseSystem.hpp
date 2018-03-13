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

#include "YTE/Platform/ForwardDeclarations.hpp"

#include "YTE/Utilities/String/String.hpp"

namespace YTE
{
  struct AudioBank
  {
    struct AudioPair
    {
      AudioPair()
        : mId(0)
        , mName()
      {

      }

      AudioPair(u64 aId, std::string &aName)
        : mId(aId)
        , mName(aName)
      {

      }

      u64 mId;
      std::string mName;
    };

    std::string mName;
    std::vector<AudioPair> mEvents;
    std::unordered_map<std::string, std::pair<AudioPair, std::vector<AudioPair>>> mSwitchGroups;
    std::unordered_map<std::string, std::pair<AudioPair, std::vector<AudioPair>>> mStateGroups;
    std::vector<AudioPair> mRTPCs;

    AkBankID mBankID;
  };

  class WWiseSystem : public Component
  {
  public:
    YTEDeclareType(WWiseSystem);

    // Sets up the system, probably could just be the constructor.

    WWiseSystem(Composition *aOwner, RSValue *aProperties);

    virtual void Initialize() override;
    // Updates the system to the current frame.
    void Update(double);

    // Cleans up anything in the system.
    virtual ~WWiseSystem();

    void RegisterObject(AkGameObjectID aId, std::string &aName);
    void DeregisterObject(AkGameObjectID aId);

    u8 RegisterListener(AkGameObjectID aId, std::string &aName);
    void DeregisterListener(AkGameObjectID aId, u8 aListener);

    void LoadAllBanks();
    AudioBank& LoadBank(const std::string &aFilename);

    void UnloadBank(const std::string &aBankName);
    void UnloadAllBanks();

    void SendEvent(const std::string &aEvent, AkGameObjectID aId);
    void SendEvent(u64 aEventId, AkGameObjectID aId);

    void SetSwitch(const std::string &aSwitchGroup, const std::string &aSwitch, AkGameObjectID aId);
    void SetSwitch(u64 aSwitchGroupId, u64 aSwitchId, AkGameObjectID aId);

    void SetState(const std::string &aStateGroup, const std::string &aState);
    void SetState(u64 aStateGroupId, u64 aStateId);

    void GetSoundIDFromString(const std::string& aName, u64& aOutID);

    void SetRTPC(const std::string &aRTPC, float aValue);
    void SetRTPC(u64 aRTPC, float aValue);

    bool GetMute();
    void SetMute(bool aMute);

    std::unordered_map<std::string, AudioBank>& GetBanks() { return mBanks; }

  private:
    void WindowLostOrGainedFocusHandler(const WindowFocusLostOrGained *aEvent);
    void WindowMinimizedOrRestoredHandler(const WindowMinimizedOrRestored *aEvent);

    void ReadTxtFile(std::string &aFile, AudioBank &bank);

    std::vector<u8> mAvailableListeners;
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