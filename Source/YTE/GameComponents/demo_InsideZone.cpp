#include "YTE/GameComponents/demo_InsideZone.hpp"
#include "YTE/Physics/Transform.hpp"
#include "YTE/WWise/WWiseSystem.hpp"
#include "YTE/GameComponents/Zone.hpp"
#include "YTE/GameComponents/Island.hpp"

namespace YTE
{

  YTEDefineType(demo_InsideZone)
  {
    RegisterType<demo_InsideZone>();
    TypeBuilder<demo_InsideZone> builder;
  }

  demo_InsideZone::demo_InsideZone(Composition *aOwner, Space *aSpace, RSValue *aProperties)
    : Component(aOwner, aSpace)
    , mFlag(true)
  {
    UnusedArguments(aProperties);
  }

  void demo_InsideZone::Initialize()
  {
    mBoatPosition = mOwner->GetComponent<Transform>()->GetTranslation();
    mBoatCollider = mOwner->GetComponent<Collider>();
    mBoatBody = mOwner->GetComponent<GhostBody>();
    mSoundEmitter = mOwner->GetComponent<WWiseEmitter>();
    mOwner->RegisterEvent<&demo_InsideZone::OnCollisionPersist>(Events::CollisionPersisted, this);
    mOwner->RegisterEvent<&demo_InsideZone::OnCollisionStart>(Events::CollisionStarted, this);
    mOwner->RegisterEvent<&demo_InsideZone::OnCollisionEnd>(Events::CollisionEnded, this);
    mSoundSystem = mSpace->GetEngine()->GetComponent<WWiseSystem>();

    if (mSoundSystem)
    {
      mIslandEnter  = mSoundSystem->GetSoundIDFromString("Islands_Enter");
      mIslandLeave  = mSoundSystem->GetSoundIDFromString("Islands_Leave");
      mSailingStart = mSoundSystem->GetSoundIDFromString("Sailing_Start");
      mSailingStop  = mSoundSystem->GetSoundIDFromString("Sailing_Stop");
      mSoundSystem->SetRTPC("Dock_Distance", 100);
    }
  }

  void demo_InsideZone::Deinitialize()
  {
    mSoundEmitter->PlayEvent(mIslandLeave);
  }

  void demo_InsideZone::OnCollisionPersist(CollisionPersisted *aEvent)
  {
    UnusedArguments(aEvent);
    return;
  }

  void demo_InsideZone::OnCollisionEnd(CollisionEnded *aEvent)
  {
    if (Composition *obj = aEvent->OtherObject)
    {
      Zone *zone = obj->GetComponent<Zone>();

      if (zone)
      {
        auto zoneType = zone->GetZoneType();
        if (zoneType == "Island")
        {
          auto it = mCollidingIslands.find(zone);
          if (it != mCollidingIslands.end())
          {
            mCollidingIslands.erase(it);
          }

          if (mCollidingIslands.empty())
          {
            mSoundEmitter->PlayEvent(mIslandLeave);
            mCurrentZone = "";
          }
        }
        else if (zoneType == "Environment")
        {
          auto it = mCollidingEnvironments.find(zone);
          if (it != mCollidingEnvironments.end())
          {
            mCollidingEnvironments.erase(it);
          }

          if (mCollidingEnvironments.empty())
          {
            mSoundEmitter->PlayEvent("IslandEnv_Stop");
          }
        }
        else if (zoneType == "Character")
        {
          auto it = mCollidingCharacters.find(zone);
          if (it != mCollidingCharacters.end())
          {
            mCollidingCharacters.erase(it);
          }

          if (mCollidingCharacters.empty())
          {
            mSoundEmitter->PlayEvent("M_Dock_Leave");
          }
        }
        else
        {
          // we be dock
          mSoundEmitter->PlayEvent("M_Dock_Leave");
        }
      }
    }
  }

  void demo_InsideZone::OnCollisionStart(CollisionStarted *aEvent)
  {
    if (Composition *obj = aEvent->OtherObject)
    {
      Zone *zone = obj->GetComponent<Zone>();

      if (zone)
      {
        auto zoneType = zone->GetZoneType();
        if (zoneType == "Island")
        {
          auto zoneName = zone->GetZoneName();

          mOwner->GetEngine()->Log(LogType::Information, fmt::format("{}", zoneName));

          if (zoneName != mCurrentZone)
          {
            mSoundSystem->SetState("Current_Island", zoneName);
            mCurrentZone = zoneName;
            mSoundEmitter->PlayEvent(mIslandEnter);
          }

          if (zoneName == mCurrentZone)
          {
            mCollidingIslands.insert(zone);
          }
        }
        else if (zoneType == "Environment")
        {
          if (!mCollidingEnvironments.empty())
          {
            mSoundEmitter->PlayEvent("IslandEnv_Start");
          }
          mCollidingEnvironments.insert(zone);
        }
        else if (zoneType == "Character")
        {
          if (mCollidingCharacters.empty())
          {
            auto zoneName = zone->GetZoneName();
            mSoundEmitter->PlayEvent(zoneName);

            if (mCollidingCharacters.empty())
            {
              mSoundEmitter->PlayEvent("M_Dock_Enter");
            }
          }
          mCollidingCharacters.insert(zone);
        }
        else
        {
          // we be dock
          mSoundEmitter->PlayEvent("M_Dock_Enter");
        }
      }
    }
  }
}