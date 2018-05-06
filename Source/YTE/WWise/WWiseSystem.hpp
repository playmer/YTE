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

#include "YTE/Meta/Meta.hpp"

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

    YTE_Shared WWiseSystem(Composition *aOwner, RSValue *aProperties);

    YTE_Shared virtual void Initialize() override;
    // Updates the system to the current frame.
    YTE_Shared void Update(double);

    // Cleans up anything in the system.
    YTE_Shared ~WWiseSystem() override;

    YTE_Shared void RegisterObject(AkGameObjectID aId, std::string &aName);
    YTE_Shared void DeregisterObject(AkGameObjectID aId);
    YTE_Shared u8 RegisterListener(AkGameObjectID aId, std::string &aName);
    YTE_Shared void DeregisterListener(AkGameObjectID aId, u8 aListener);
    YTE_Shared void LoadAllBanks();
    YTE_Shared AudioBank& LoadBank(const std::string &aFilename);
    YTE_Shared void UnloadBank(const std::string &aBankName);
    YTE_Shared void UnloadAllBanks();
    YTE_Shared void SendEvent(const std::string &aEvent, AkGameObjectID aId);
    YTE_Shared void SendEvent(u64 aEventId, AkGameObjectID aId);
    YTE_Shared void SetSwitch(const std::string &aSwitchGroup, const std::string &aSwitch, AkGameObjectID aId);
    YTE_Shared void SetSwitch(u64 aSwitchGroupId, u64 aSwitchId, AkGameObjectID aId);
    YTE_Shared void SetState(const std::string &aStateGroup, const std::string &aState);
    YTE_Shared void SetState(u64 aStateGroupId, u64 aStateId);
    YTE_Shared u64 GetSoundIDFromString(const std::string& aName);
    YTE_Shared void SetRTPC(const std::string &aRTPC, float aValue);
    YTE_Shared void SetRTPC(u64 aRTPC, float aValue);
    YTE_Shared bool GetMute();
    YTE_Shared void SetMute(bool aMute);

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