/******************************************************************************/
/*!
 * \author Joshua T. Fisher
 * \date   2015-09-19
 *
 * \copyright All content 2016 DigiPen (USA) Corporation, all rights reserved.
 */
/******************************************************************************/
#include <cstdio>

#define UNICODE
#include "AK/SoundEngine/Common/AkMemoryMgr.h"    // Memory Manager
#include "AK/SoundEngine/Common/AkModule.h"       // Default memory and stream managers
#include "AK/Tools/Common/AkPlatformFuncs.h"      // Thread defines
#include "AK/SoundEngine/Common/IAkStreamMgr.h"
#include "AK/MusicEngine/Common/AkMusicEngine.h"  // Music Engine
#include "SoundEngine/Win32/AkFilePackageLowLevelIOBlocking.h"   // Sample low-level I/O implementation



///////////////////////////////////////////////////////////////////
// Plugins
///////////////////////////////////////////////////////////////////
#include <AK/AkPlatforms.h>
#include <AK/SoundEngine/Common/AkTypes.h>
#include <AK/SoundEngine/Common/IAkPlugin.h>

// Effect plug-ins
#include "AK/Plugin/AkCompressorFXFactory.h"       // Compressor
#include "AK/Plugin/AkDelayFXFactory.h"            // Delay
#include "AK/Plugin/AkMatrixReverbFXFactory.h"     // Matrix reverb
#include "AK/Plugin/AkMeterFXFactory.h"            // Meter
#include "AK/Plugin/AkExpanderFXFactory.h"         // Expander
#include "AK/Plugin/AkParametricEQFXFactory.h"     // Parametric equalizer
#include "AK/Plugin/AkGainFXFactory.h"             // Gain
#include "AK/Plugin/AkPeakLimiterFXFactory.h"      // Peak limiter
#include "AK/Plugin/AkRoomVerbFXFactory.h"         // RoomVerb
#include "AK/Plugin/AkGuitarDistortionFXFactory.h" // Guitar distortion
#include "AK/Plugin/AkStereoDelayFXFactory.h"      // Stereo delay
#include "AK/Plugin/AkPitchShifterFXFactory.h"     // Pitch shifter
#include "AK/Plugin/AkTimeStretchFXFactory.h"      // Time stretch
#include "AK/Plugin/AkFlangerFXFactory.h"          // Flanger
#include "AK/Plugin/AkTremoloFXFactory.h"          // Tremolo
#include "AK/Plugin/AkHarmonizerFXFactory.h"       // Harmonizer
#include "AK/Plugin/AkRecorderFXFactory.h"         // Recorder

// Platform specific
#ifdef AK_PS4
#include "AK/Plugin/SceAudio3dEngineFactory.h"          // SCE Audio3d
#endif

// Sources plug-ins
#include "AK/Plugin/AkSilenceSourceFactory.h"         // Silence generator
#include "AK/Plugin/AkSineSourceFactory.h"            // Sine wave generator
#include "AK/Plugin/AkToneSourceFactory.h"            // Tone generator
#include "AK/Plugin/AkAudioInputSourceFactory.h"      // Audio input

// Required by codecs plug-ins
#include "AK/Plugin/AkVorbisDecoderFactory.h"
#ifdef AK_APPLE
#include "AK/Plugin/AkAACFactory.h"      // Note: Usable only on Apple devices. Ok to include it on other platforms as long as it is not referenced.
#endif
#ifdef AK_NX
#include "AK/Plugin/AkOpusFactory.h"    // Note: Usable only on NX. Ok to include it on other platforms as long as it is not referenced.
#endif


// Include for communication between WWise and the game -- Not needed in the release version
#ifndef AK_OPTIMIZED
  #include <AK/Comm/AkCommunication.h>
#endif // AK_OPTIMIZED

#undef UNICODE

#include <fstream>
#include <iostream>
#include <filesystem>

#include "YTE/Core/AssetLoader.hpp"
#include "YTE/Core/Engine.hpp"

#include "YTE/Platform/TargetDefinitions.hpp"

#include "YTE/Utilities/Utilities.hpp"

#include "YTE/WWise//WWiseSystem.hpp"

#ifdef NDEBUG
#define AK_OPTIMIZED 1
#endif

namespace YTE
{
  YTEDefineType(WWiseSystem)
  {
    RegisterType<WWiseSystem>();
    TypeBuilder<WWiseSystem> builder;

    builder.Property<&WWiseSystem::GetMute, &WWiseSystem::SetMute>( "Mute");
    builder.Function<SelectOverload<void (WWiseSystem::*) (u64, float),&WWiseSystem::SetRTPC>()>("SetRTPC")
      .SetParameterNames("aRTPC", "aValue");
    builder.Function<SelectOverload<void (WWiseSystem::*) (const std::string&, float),&WWiseSystem::SetRTPC>()>("SetRTPC")
      .SetParameterNames("aRTPC", "aValue");
  }

  // We're using the default Low-Level I/O implementation that's part
  // of the SDK's sample code, with the file package extension
  CAkFilePackageLowLevelIOBlocking g_lowLevelIO;

  WWiseSystem::WWiseSystem(Composition *aOwner, RSValue *aProperties)
    : Component(aOwner, nullptr)
    , mMuted(false)
    , mAvailableListeners{0,1,2,3,4,5,6,7}
  {
    YTEUnusedArgument(aProperties);
  }

  void WWiseSystem::Initialize()
  {
    // Create and initialize an instance of the default memory manager. Note
    // that you can override the default memory manager with your own. Refer
    // to the SDK documentation for more information.
    AkMemSettings memSettings;
    memSettings.uMaxNumPools = 20;

    if (AK::MemoryMgr::Init(&memSettings) != AK_Success)
    {
      assert(!"Could not create the memory manager.");
    }

    // Create and initialize an instance of the default streaming manager. Note
    // that you can override the default streaming manager with your own. Refer
    // to the SDK documentation for more information.
    AkStreamMgrSettings stmSettings;
    AK::StreamMgr::GetDefaultSettings(stmSettings);

    // Customize the Stream Manager settings here.
    if (!AK::StreamMgr::Create(stmSettings))
    {
      assert(!"Could not create the Streaming Manager");
    }

    // Create a streaming device with blocking low-level I/O handshaking.
    // Note that you can override the default low-level I/O module with your own. Refer
    // to the SDK documentation for more information.        
    AkDeviceSettings deviceSettings;
    AK::StreamMgr::GetDefaultDeviceSettings(deviceSettings);

    // Customize the streaming device settings here.

    // CAkFilePackageLowLevelIOBlocking::Init() creates a streaming device
    // in the Stream Manager, and registers itself as the File Location Resolver.
    if (g_lowLevelIO.Init(deviceSettings) != AK_Success)
    {
      assert(!"Could not create the streaming device and Low-Level I/O system");
    }

    // Create the Sound Engine
    // Using default initialization parameters

    AkInitSettings initSettings;
    AkPlatformInitSettings platformInitSettings;
    AK::SoundEngine::GetDefaultInitSettings(initSettings);
    AK::SoundEngine::GetDefaultPlatformInitSettings(platformInitSettings);

    if (AK::SoundEngine::Init(&initSettings, &platformInitSettings) != AK_Success)
    {
      assert(!"Could not initialize the Sound Engine.");
    }

    // Initialize the music engine
    // Using default initialization parameters
    AkMusicSettings musicInit;
    AK::MusicEngine::GetDefaultInitSettings(musicInit);

    if (AK::MusicEngine::Init(&musicInit) != AK_Success)
    {
      assert(!"Could not initialize the Music Engine.");
    }

  if constexpr(CompilerOptions::Debug())
  {
      AkCommSettings commSettings;
      AK::Comm::GetDefaultInitSettings(commSettings);

      if (AK::Comm::Init(commSettings) != AK_Success)
      {
        assert(!"Could not initialize communication.");
      }
  }

    LoadAllBanks();

    

    //// Register the main listener.
    // AkGameObjectID MY_DEFAULT_LISTENER = reinterpret_cast<AkGameObjectID>(this);
    //auto check = AK::SoundEngine::RegisterGameObj(MY_DEFAULT_LISTENER, "My Default Listener");
    //YTEUnusedArgument(check);
    //assert(check == AK_Success);
    //
    //// Set one listener as the default.
    //AK::SoundEngine::SetDefaultListeners(&MY_DEFAULT_LISTENER, 1);
  }


  u8 WWiseSystem::RegisterListener(AkGameObjectID aId, std::string &aName)
  {
    if (0 != mAvailableListeners.size())
    {
      auto listener = mAvailableListeners.back();
      mAvailableListeners.pop_back();
      RegisterObject(aId, aName);

      return listener;
    }

    return 9;
  }

  void WWiseSystem::DeregisterListener(AkGameObjectID aId, u8 aListener)
  {
    DeregisterObject(aId);
    mAvailableListeners.emplace_back(aListener);
  }

  void WWiseSystem::RegisterObject(AkGameObjectID aId, std::string &aName)
  {
    auto check = AK::SoundEngine::RegisterGameObj(aId, aName.c_str());
    YTEUnusedArgument(check);
    assert(check == AK_Success);
  }

  void WWiseSystem::DeregisterObject(AkGameObjectID aId)
  {
    auto check = AK::SoundEngine::UnregisterGameObj(aId);
    YTEUnusedArgument(check);
    assert(check == AK_Success);
  }

  void WWiseSystem::WindowLostOrGainedFocusHandler(const WindowFocusLostOrGained *aEvent)
  {
    mFocused = aEvent->Focused;

    if ((mFocusHandled == false) && (aEvent->Focused == false) && !mMinimized)
    {
      if (mMuted == false)
      {
        AK::SoundEngine::Suspend(true);
      }

      mPriorToMinimize = mPaused;
      mPaused = true;
      mFocusHandled = true;
    }
    else if ((aEvent->Focused == true) && !mMinimized)
    {
      if (mMuted == false)
      {
        AK::SoundEngine::WakeupFromSuspend();
      }

      mPaused = mPriorToMinimize;
      mFocusHandled = false;
    }
  }

  void WWiseSystem::WindowMinimizedOrRestoredHandler(const WindowMinimizedOrRestored *aEvent)
  {
    mMinimized = aEvent->Minimized;

    if (aEvent->Minimized && mFocused)
    {
      if (mMuted == false)
      {
        AK::SoundEngine::Suspend(true);
      }

      mPriorToMinimize = mPaused;
      mPaused = true;
    }
    else if (!aEvent->Minimized && mFocused)
    {
      if (mMuted == false)
      {
        AK::SoundEngine::WakeupFromSuspend();
      }

      mPaused = mPriorToMinimize;
      mFocusHandled = false;
    }
  }

  bool WWiseSystem::GetMute()
  {
    return mMuted;
  }

  void WWiseSystem::SetMute(bool aMute)
  {
      // They're the same, we don't care.
    if (aMute == mMuted)
    {
      return;
    }

      // They want it muted? They got it.
    if (aMute)
    {
      mMuted = aMute;
      AK::SoundEngine::Suspend(true);
      return;
    }

    if (aMute == false && mMuted == true)
    {
      mMuted = aMute;
      AK::SoundEngine::WakeupFromSuspend();
    }
    else
    {
      mMuted = aMute;
    }
  }

  void WWiseSystem::SetRTPC(const std::string &aRTPC, float aValue)
  {
    AK::SoundEngine::SetRTPCValue(aRTPC.c_str(), aValue);
  }

  void WWiseSystem::SetRTPC(u64 aRTPC, float aValue)
  {
    AK::SoundEngine::SetRTPCValue(static_cast<AkRtpcID>(aRTPC), aValue);
  }

  void WWiseSystem::Update(double)
  {
    AK::SoundEngine::RenderAudio(true);
  }

  WWiseSystem::~WWiseSystem()
  {
    #ifndef AK_OPTIMIZED
    // Terminate Communication Services
    AK::Comm::Term();
    #endif // AK_OPTIMIZED

    // Terminate the music engine
    AK::MusicEngine::Term();

    // Terminate the sound engine
    AK::SoundEngine::Term();

    // Terminate the streaming device and streaming manager

    // CAkFilePackageLowLevelIOBlocking::Term() destroys its associated streaming device 
    // that lives in the Stream Manager, and unregisters itself as the File Location Resolver.
    g_lowLevelIO.Term();

    if (AK::IAkStreamMgr::Get())
    {
      AK::IAkStreamMgr::Get()->Destroy();
    }

    // Terminate the Memory Manager
    AK::MemoryMgr::Term();
  }
  
  namespace WWiseStatics
  {
    static std::string Events{ "Event" };
    static std::string Switches{ "Switch" };
    static std::string SwitchGroups{ "Switch Group" };
    static std::string States{ "State" };
    static std::string StateGroups{ "State Group" };
    static std::string RTPCs{ "Game Parameter" };
  }

  void RemoveEmptyStrings(std::vector<std::vector<std::string>> &aLines)
  {
    for (auto &line : aLines)
    {
      line.erase(std::remove_if(line.begin(), 
                                line.end(), 
                                [](std::string &a) {return a.empty(); }), 
                 line.end());
    }
  }

  void SortAudioPairs(std::vector<AudioBank::AudioPair> &aPairList)
  {
    std::sort(aPairList.begin(),
              aPairList.end(),
              [](AudioBank::AudioPair &aLeft, AudioBank::AudioPair &aRight)
    {
      return aRight.mName > aLeft.mName;
    });
  }

  void WWiseSystem::ReadTxtFile(std::string &aFile, AudioBank &bank)
  {
    std::string fileText;
    ReadFileToString(aFile, fileText);

    std::vector<std::vector<std::string>> lines;


    for (auto &line : split(fileText, '\n', true))
    {
      lines.emplace_back(split(line, '\t', false));
    }

    std::vector<std::vector<std::string>> events;       // Event
    std::vector<std::vector<std::string>> switches;     // Switch
    std::vector<std::vector<std::string>> switchGroups; // Switch Group
    std::vector<std::vector<std::string>> states;     // Switch
    std::vector<std::vector<std::string>> stateGroups; // Switch Group
    std::vector<std::vector<std::string>> rtpcs;        // Game Parameter

    std::vector<std::vector<std::string>> otherTopLevel;
    std::vector<std::vector<std::string>> other;

    std::vector<std::vector<std::string>> *current{ &other };

    for (auto &line : lines)
    {
      if (0 != line.size())
      {
        if (WWiseStatics::Events == line[0])
        {
          current = &events;
        }
        else if (WWiseStatics::Switches == line[0])
        {
          current = &switches;
        }
        else if (WWiseStatics::SwitchGroups == line[0])
        {
          current = &switchGroups;
        }
        else if (WWiseStatics::States == line[0])
        {
          current = &states;
        }
        else if (WWiseStatics::StateGroups == line[0])
        {
          current = &stateGroups;
        }
        else if (WWiseStatics::RTPCs == line[0])
        {
          current = &rtpcs;
        }
        else if (0 != line[0].size())
        {
          otherTopLevel.emplace_back(std::move(line));
          current = &other;
        }
        else
        {
          current->emplace_back(std::move(line));
        }
      }
    }

    RemoveEmptyStrings(events);
    RemoveEmptyStrings(switches);
    RemoveEmptyStrings(switchGroups);
    RemoveEmptyStrings(states);
    RemoveEmptyStrings(stateGroups);
    RemoveEmptyStrings(rtpcs);
    RemoveEmptyStrings(otherTopLevel);
    RemoveEmptyStrings(other);

    for (auto &event : events)
    {
      auto id{ std::stoull(event[0]) };

      bank.mEvents.emplace_back(id, event[1]);
    }
    SortAudioPairs(bank.mEvents);

    for (auto &aSwitch : switches)
    {
      auto id{ std::stoull(aSwitch[0]) };

      bank.mSwitchGroups[aSwitch[2]].second.emplace_back(id, aSwitch[1]);
    }

    for (auto &switchGroup : switchGroups)
    {
      auto id{ std::stoull(switchGroup[0]) };

      auto &group = bank.mSwitchGroups[switchGroup[1]];

      group.first.mId = id;
      group.first.mName = switchGroup[1];
      SortAudioPairs(group.second);
    }

    for (auto &state : states)
    {
      auto id{ std::stoull(state[0]) };

      bank.mStateGroups[state[2]].second.emplace_back(id, state[1]);
    }

    for (auto &stateGroup : stateGroups)
    {
      auto id{ std::stoull(stateGroup[0]) };

      auto &group = bank.mStateGroups[stateGroup[1]];

      group.first.mId = id;
      group.first.mName = stateGroup[1];
      SortAudioPairs(group.second);
    }

    for (auto &rtpc : rtpcs)
    {
      auto id{ std::stoull(rtpc[0]) };

      bank.mRTPCs.emplace_back(id, rtpc[1]);
    }

    SortAudioPairs(bank.mRTPCs);
  }

  void WWiseSystem::LoadAllBanks()
  {
    namespace fs = std::experimental::filesystem;

    auto gamePath{ Path::GetGamePath() };
    auto gamePathStr{ gamePath.String() };

    fs::path wwisePath = gamePathStr;
    wwisePath = wwisePath.parent_path();
    wwisePath /= "WWise";

    auto bnkInfo{ wwisePath };
    bnkInfo /= "SoundbanksInfo.json";

    if (false == fs::exists(wwisePath) || false == fs::exists(bnkInfo))
    {
      return;
    }

    auto bnkInfoStr{ bnkInfo.string() };

    std::string fileText;
    auto success = ReadFileToString(bnkInfoStr, fileText);

    RSDocument document;

    if (success && document.Parse(fileText.c_str()).HasParseError())
    {
      std::cout << "Error in WWise JSON: " << bnkInfoStr << std::endl;
    }

    auto &info = document["SoundBanksInfo"];
    auto banks = info.FindMember("SoundBanks");

    std::vector<std::pair<std::string, std::string>> bankAndShortName;

    // Retrieve banks from the JSON file.
    for (auto bankIt = banks->value.Begin(); bankIt < banks->value.End(); ++bankIt)
    {
      std::string bankFilename{ bankIt->FindMember("Path")->value.GetString() };
      std::string shortName{ bankIt->FindMember("ShortName")->value.GetString() };

      bankAndShortName.emplace_back(bankFilename, shortName);
    }

    // Make sure Init is the first bank we load.
    for (auto &bankNames : bankAndShortName)
    {
      if ("Init" == bankNames.second && bankAndShortName[0].second != bankNames.second)
      {
        std::swap(bankNames, bankAndShortName[0]);
      }
    }

    // Load the banks and their txt files.
    for (auto &bankNames : bankAndShortName)
    {
      auto bankPath{ wwisePath };
      bankPath /= bankNames.first;
      std::string bankFilename = bankPath.string();

      auto &bank{ LoadBank(bankFilename) };

      bank.mName = bankNames.second;

      auto txtPath{ wwisePath };
      txtPath /= bankNames.second;
      txtPath += ".txt";
      std::string txtFilename{ txtPath.string() };

      ReadTxtFile(txtFilename, bank);
    }
  }

  AudioBank& WWiseSystem::LoadBank(const std::string &aFilename)
  {
    auto &bank{ mBanks[aFilename] };

    AKRESULT eResult = AK::SoundEngine::LoadBank(aFilename.c_str(),
                                                 AK_DEFAULT_POOL_ID, 
                                                 bank.mBankID);
    YTEUnusedArgument(eResult);
    //assert(eResult == AK_Success);
    return bank;
  }
    
  void WWiseSystem::UnloadBank(const std::string &bnkName)
  {
    if (mBanks.find(bnkName) == mBanks.end())
    {
      std::cout << "Could not unload " << bnkName << std::endl;
      return;
    }
    else
    {
      AK::SoundEngine::UnloadBank(mBanks[bnkName].mBankID, nullptr);
    }
  }

  void WWiseSystem::UnloadAllBanks()
  {
    for (auto &bank : mBanks)
    {
      AK::SoundEngine::UnloadBank(bank.second.mBankID, nullptr);
    }
  }

  void WWiseSystem::SendEvent(const std::string &aEvent, AkGameObjectID aObject)
  {
    AK::SoundEngine::PostEvent(aEvent.c_str(), aObject);
  }

  void WWiseSystem::SendEvent(u64 aEvent, AkGameObjectID aObject)
  {
    AK::SoundEngine::PostEvent(static_cast<AkUniqueID>(aEvent), aObject);
  }

  void WWiseSystem::SetSwitch(const std::string &aSwitchGroup, const std::string &aSwitch, AkGameObjectID aId)
  {
    AK::SoundEngine::SetSwitch(aSwitchGroup.c_str(), aSwitch.c_str(), aId);
  }

  void WWiseSystem::SetSwitch(u64 aSwitchGroupId, u64 aSwitchId, AkGameObjectID aId)
  {
    AK::SoundEngine::SetSwitch(static_cast<AkSwitchGroupID>(aSwitchGroupId), static_cast<AkSwitchStateID>(aSwitchId), aId);
  }

  void WWiseSystem::SetState(const std::string &aStateGroup, const std::string &aState)
  {
    AK::SoundEngine::SetState(aStateGroup.c_str(), aState.c_str());
  }
  
  void WWiseSystem::SetState(u64 aStateGroupId, u64 aStateId)
  {
    AK::SoundEngine::SetState(static_cast<AkStateGroupID>(aStateGroupId), static_cast<AkStateID>(aStateId));
  }

  u64 WWiseSystem::GetSoundIDFromString(const std::string& aName)
  {
    for (auto &bank : mBanks)
    {
      for (auto &event : bank.second.mEvents)
      {
        if (event.mName == aName)
        {
          return event.mId;
        }
      }
    }

    mOwner->GetEngine()->Log(LogType::Error, 
                             fmt::format("WwiseSystem: Could not find sound with name {}",
                                         aName));

    return 0;
  }
}
