#include "YTE/GameComponents/demo_InsideZone.hpp"
#include "YTE/Physics/Transform.hpp"
#include "YTE/WWise/WWiseSystem.hpp"
#include "YTE/GameComponents/Zone.hpp"
#include "YTE/GameComponents/Island.hpp"

namespace YTE
{

  YTEDefineType(demo_InsideZone)
  {
    YTERegisterType(demo_InsideZone);
  }

  demo_InsideZone::demo_InsideZone(Composition *aOwner, Space *aSpace, RSValue *aProperties)
    : Component(aOwner, aSpace)
    , mFlag(true)
  {
    YTEUnusedArgument(aProperties);
  }

  void demo_InsideZone::Initialize()
  {
    mBoatPosition = mOwner->GetComponent<Transform>()->GetTranslation();
    mBoatCollider = mOwner->GetComponent<Collider>();
    mBoatBody = mOwner->GetComponent<GhostBody>();
    mSoundEmitter = mOwner->GetComponent<WWiseEmitter>();
    mOwner->YTERegister(Events::CollisionPersisted, this, &demo_InsideZone::OnCollisionPersist);
    mOwner->YTERegister(Events::CollisionStarted, this, &demo_InsideZone::OnCollisionStart);
    mOwner->YTERegister(Events::CollisionEnded, this, &demo_InsideZone::OnCollisionEnd);
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
    if (Composition *obj = aEvent->OtherObject)
    {
      Zone *zone = obj->GetComponent<Zone>();

      if (!zone)
      {
        return;
      }

      auto zoneType = zone->GetZoneType();
      if (zoneType != "Dock")
      {
        return;
      }

      // make sure our boat position is up to date
      mBoatPosition = mOwner->GetComponent<Transform>()->GetTranslation();
      auto dockPos = aEvent->OtherObject->GetComponent<Transform>()->GetTranslation();

      auto dist = glm::length2(dockPos - mBoatPosition); // maybe this should just be regular length?
      dist = glm::clamp(dist, 0.0f, 100.0f);

      auto zoneName = zone->GetZoneName();
      if (zoneName == "Dock_Distance")
      {
        mSoundSystem->SetRTPC("Dock_Distance", dist);
      }

      // change the volume of the island sound using the minDist
      mOwner->GetSpace()->GetComponent<WWiseSystem>()->SetRTPC(zoneName, dist);

      if (dist < 100.0f && mFlag)
      {
        //closestIsland->GetComponent<Island>().mCharacterCalloutString;
        mSoundEmitter->PlayEvent("Dia_All_CallOut");

        mFlag = false;
      }
    }
  }

  void demo_InsideZone::OnCollisionEnd(CollisionEnded *aEvent)
  {
    if (Composition *obj = aEvent->OtherObject)
    {
      Zone *zone = obj->GetComponent<Zone>();

      if (zone)
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
      }
    }
  }
}