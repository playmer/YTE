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

#include "YTE/Event/StandardEvents.h"

#include "YTE/Meta/Type.hpp"

#include "YTE/Utilities/String/String.h"

namespace YTE
{
  struct AudioBank
  {
    std::vector<String> mEvents;
    std::vector<String> mSwitchGroup;

    //All the Game param in the bank file itself in strings
    std::vector<String> mGameParams;

    AkBankID mBankID;
    //1st = Switch Group, 2nd = Switches
    std::unordered_map <String, std::vector<String>> mSwitches;
  };


  struct SwitchGroup
  {
    std::string name;
    std::string switches;
  };

  struct SoundMaterial
  {
    std::string EventName;
    std::vector<SwitchGroup> EventChanger;
    bool called;
  };

  class WWiseSystem : public Component
  {
  public:
    DeclareType(WWiseSystem);

    // Sets up the system, probably could just be the constructor.

    WWiseSystem(Composition *aOwner, RSValue *aProperties);

    virtual void Initialize();
    // Updates the system to the current frame.
    virtual void Update(float);

    // Cleans up anything in the system.
    virtual ~WWiseSystem();


    void SetPath(const std::wstring &aPath); //Sets the path for audio files for loading
    bool LoadBank(const char *aFilename);
    //void LoadBankText(const char *aFilename);

    void UnloadBank(const std::string &aBankName);
    void UnloadAllBanks();
    void UnloadBankText(const std::string &file);

    void PrintBankHierarchy();

    void SendEvent(String aEvent, AkGameObjectID aId);
    void SetRTPC(String aRTPC, float aValue);
    float GetRTPC(String aRTPC);

    bool GetMute();
    void SetMute(bool aMute);

      

  private:
    void WindowLostOrGainedFocusHandler(const WindowFocusLostOrGained *aEvent);
    void WindowMinimizedOrRestoredHandler(const WindowMinimizedOrRestored *aEvent);

    //String mPath;
    std::unordered_map<String, float> mRTPCs;

    //1st = Bnk name, 2nd = Bank Data
    std::unordered_map<String, AudioBank> mBanks;
    std::unordered_map<String, SoundMaterial> mSoundMaterials;
    std::vector<String> mEvents;

    std::unordered_map<String, AkUniqueID> mEventIds;

    //1st = Event Name, 2nd All the event params from the event
    std::unordered_map<String, std::vector<String>> mEventParameters;

    bool mPaused = false;
    bool mPriorToMinimize = false;
    bool mFocusHandled = false;

    bool mMuted;


    bool mMinimized = false;
    bool mFocused = false;
  };
}

#endif