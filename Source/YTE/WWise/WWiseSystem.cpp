/******************************************************************************/
/*!
 * \author Joshua T. Fisher
 * \date   2015-09-19
 *
 * \copyright All content 2016 DigiPen (USA) Corporation, all rights reserved.
 */
/******************************************************************************/

#define UNICODE
#include "AK/SoundEngine/Common/AkMemoryMgr.h"    // Memory Manager
#include "AK/SoundEngine/Common/AkModule.h"       // Default memory and stream managers
#include "AK/Tools/Common/AkPlatformFuncs.h"      // Thread defines
#include "AK/SoundEngine/Common/IAkStreamMgr.h"
#include "AK/MusicEngine/Common/AkMusicEngine.h"  // Music Engine
#include "SoundEngine/Win32/AkFilePackageLowLevelIOBlocking.h"   // Sample low-level I/O implementation

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

#include "YTE/Utilities/Utilities.h"

#include "YTE/WWise//WWiseSystem.hpp"

#ifdef NDEBUG
#define AK_OPTIMIZED 1
#endif

namespace YTE
{
  YTEDefineType(WWiseSystem)
  {
    YTERegisterType(WWiseSystem);

    YTEBindProperty(&WWiseSystem::GetMute, &WWiseSystem::SetMute, "Mute");
    YTEBindFunction(&WWiseSystem::SetRTPC, YTENoOverload, "SetRTPC", YTEParameterNames("aRTPC", "aValue"));
    YTEBindFunction(&WWiseSystem::GetRTPC, YTENoOverload, "GetRTPC", YTEParameterNames("aRTPC"));
  }

  // We're using the default Low-Level I/O implementation that's part
  // of the SDK's sample code, with the file package extension
  CAkFilePackageLowLevelIOBlocking g_lowLevelIO;

  WWiseSystem::WWiseSystem(Composition *aOwner, RSValue *aProperties)
    : Component(aOwner, nullptr)
    , mMuted(false)
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

    #ifndef AK_OPTIMIZED
    // Initialize communications (not in release build!)
    AkCommSettings commSettings;
    AK::Comm::GetDefaultInitSettings(commSettings);
    if (AK::Comm::Init(commSettings) != AK_Success)
    {
      assert(!"Could not initialize communication.");
    }
    #endif // AK_OPTIMIZED

    LoadAllBanks();

    AkGameObjectID MY_DEFAULT_LISTENER = reinterpret_cast<AkGameObjectID>(this);
    

    // Register the main listener.
    auto check = AK::SoundEngine::RegisterGameObj(MY_DEFAULT_LISTENER, "My Default Listener");
    assert(check == AK_Success);

    // Set one listener as the default.
    AK::SoundEngine::SetDefaultListeners(&MY_DEFAULT_LISTENER, 1);

    //mOwner->YTERegister(Events::WindowFocusLostOrGained, this, &WWiseSystem::WindowLostOrGainedFocusHandler);
    //mOwner->YTERegister(Events::WindowMinimizedOrRestored, this, &WWiseSystem::WindowMinimizedOrRestoredHandler);

    //Events::WindowFocusLostOrGained, &focusEvent);
  }

  void WWiseSystem::RegisterObject(AkGameObjectID aId, std::string &aName)
  {
    auto check = AK::SoundEngine::RegisterGameObj(aId, aName.c_str());
    assert(check == AK_Success);
  }

  void WWiseSystem::DeregisterObject(AkGameObjectID aId)
  {
    auto check = AK::SoundEngine::UnregisterGameObj(aId);
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
    mRTPCs[aRTPC] = aValue;
    AK::SoundEngine::SetRTPCValue(aRTPC.c_str(), aValue);
  }

  float WWiseSystem::GetRTPC(const std::string &aRTPC)
  {
    auto rtpc = mRTPCs.find(aRTPC);

    if (rtpc != mRTPCs.end())
    {
      return rtpc->second;
    }
    else
    {
      return -1.0f;
    }
  }

  void WWiseSystem::Update(float)
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

  struct TextFileGarbage
  {

  };

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

    for (auto bankIt = banks->value.Begin(); bankIt < banks->value.End(); ++bankIt)
    {
      std::string bankFilename{ bankIt->FindMember("Path")->value.GetString() };
      std::string shortName{ bankIt->FindMember("ShortName")->value.GetString() };

      auto bankPath{ wwisePath };
      bankPath /= bankFilename;
      bankFilename = bankPath.string();

      auto &bank{ LoadBank(bankFilename) };

      bank.mName = shortName;

      auto events{ bankIt->FindMember("IncludedEvents") };
      
      if (events != bankIt->MemberEnd())
      {
        for (auto eventsIt = events->value.Begin(); eventsIt < events->value.End(); ++eventsIt)
        {
          std::string idStr{ eventsIt->FindMember("Id")->value.GetString() };
          std::string nameStr{ eventsIt->FindMember("Name")->value.GetString() };
      
          auto id{ std::stoull(idStr) };
      
          bank.mEvents.emplace_back(id, nameStr);
        }
      }
    }
  }

  AudioBank& WWiseSystem::LoadBank(const std::string &aFilename)
  {
    auto &bank{ mBanks[aFilename] };

    AKRESULT eResult = AK::SoundEngine::LoadBank(aFilename.c_str(),
                                                 AK_DEFAULT_POOL_ID, 
                                                 bank.mBankID);

    assert(eResult == AK_Success);
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
    auto id = AK::SoundEngine::PostEvent(aEvent.c_str(), aObject);
  }
}
