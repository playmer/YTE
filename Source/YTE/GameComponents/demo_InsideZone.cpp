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
  }

  void demo_InsideZone::OnCollisionPersist(CollisionPersisted *aEvent)
  {
    // make sure our boat position is up to date
    mBoatPosition = mOwner->GetComponent<Transform>()->GetTranslation();
    // get the map of compositions on this zone
    YTE::CompositionMap &compMap = *aEvent->OtherObject->GetCompositions();
    // if our map isnt empty
    if (compMap.size() > size_t(0.0f))
    {
      float minDist = 0.0f;
      auto closestIsland = compMap.begin()->second.get();
      for (auto &cmp : compMap)
      {
        auto islandCmp = cmp.second.get();
        glm::vec3 islandPos = islandCmp->GetComponent<Transform>()->GetWorldTranslation();
        float dist = glm::length2(islandPos - mBoatPosition);
        // if we dont have a min, indicated by 0, or we found a closer island set it as the closest
        if (minDist == 0.0f || dist < minDist)
        {
          minDist = dist;
          closestIsland = cmp.second.get();
        }
      }
      // change the volume of the island sound using the minDist
      mOwner->GetSpace()->GetComponent<WWiseSystem>()->SetRTPC("Island_Distance", minDist);

      if (minDist < 100.0f && mFlag)
      {
        //closestIsland->GetComponent<Island>().mCharacterCalloutString;
        mSoundEmitter->PlayEvent("Dia_All_CallOut");

        mFlag = false;
      }
    }
  }

  void demo_InsideZone::OnCollisionEnd(CollisionEnded *aEvent)
  {
    Zone *zone = aEvent->OtherObject->GetComponent<Zone>();

    if (zone)
    {
      if (zone->GetOwner()->GetName() == "PicanteIsles")
      {
        mSoundEmitter->PlayEvent("TD01_Leave");
      }
      else if (zone->GetOwner()->GetName() == "RainyRuins")
      {
        mSoundEmitter->PlayEvent("TD03_Leave");
        mOwner->GetSpace()->GetComponent<WWiseSystem>()->SetRTPC("Rain", 0.0f);
        mSoundEmitter->PlayEvent("A_Rain_Stop");
      }

      // play ocean sound
    }
  }

  void demo_InsideZone::OnCollisionStart(CollisionStarted *aEvent)
  {
    Zone *zone = aEvent->OtherObject->GetComponent<Zone>();

    if (zone)
    {
      if (zone->GetOwner()->GetName() == "PicanteIsles")
      {
        // play picanteisles music event
        mSoundEmitter->PlayEvent("TD01_Enter");
      }
      else if (zone->GetOwner()->GetName() == "RainyRuins")
      {
        // play rainyruins music event
        mSoundEmitter->PlayEvent("TD03_Enter");
        mSoundEmitter->PlayEvent("A_Rain_Start");
        mOwner->GetSpace()->GetComponent<WWiseSystem>()->SetRTPC("Rain", 30.0f);
      }
    }
  }

}