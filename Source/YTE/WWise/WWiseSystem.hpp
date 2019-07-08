#pragma once

#ifndef YTE_WWise_WWiseSystem_h
#define YTE_WWise_WWiseSystem_h

#include <unordered_map>
#include <vector>

#include "YTE/Core/Component.hpp"

#include "YTE/Meta/Meta.hpp"

#include "YTE/Platform/ForwardDeclarations.hpp"

#include "YTE/Utilities/String/String.hpp"

#include "YTE/WWise/ForwardDeclarations.hpp"

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

    WwiseBank mBankID;
  };

  class WWiseSystem : public Component
  {
  public:
    YTEDeclareType(WWiseSystem);

    // Sets up the system, probably could just be the constructor.

    YTE_Shared WWiseSystem(Composition *aOwner);

    YTE_Shared virtual void Initialize() override;
    // Updates the system to the current frame.
    YTE_Shared void Update(double);

    // Cleans up anything in the system.
    YTE_Shared ~WWiseSystem() override;

    YTE_Shared void RegisterObject(WwiseObject aId, std::string& aName);
    YTE_Shared void DeregisterObject(WwiseObject aId);
    YTE_Shared u8 RegisterListener(WwiseObject aId, std::string& aName);
    YTE_Shared void DeregisterListener(WwiseObject aId, u8 aListener);
    YTE_Shared void LoadAllBanks();
    YTE_Shared AudioBank& LoadBank(std::string const& aFilename);
    YTE_Shared void UnloadBank(std::string const& aBankName);
    YTE_Shared void UnloadAllBanks();
    YTE_Shared void SendEvent(std::string const& aEvent, WwiseObject aId);
    YTE_Shared void SendEvent(u64 aEventId, WwiseObject aId);
    YTE_Shared void SetSwitch(std::string const& aSwitchGroup, std::string const& aSwitch, WwiseObject aId);
    YTE_Shared void SetSwitch(u64 aSwitchGroupId, u64 aSwitchId, WwiseObject aId);
    YTE_Shared void SetState(std::string const& aStateGroup, std::string const& aState);
    YTE_Shared void SetState(u64 aStateGroupId, u64 aStateId);
    YTE_Shared u64 GetSoundIDFromString(std::string const& aName);
    YTE_Shared void SetRTPC(std::string const& aRTPC, float aValue);
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