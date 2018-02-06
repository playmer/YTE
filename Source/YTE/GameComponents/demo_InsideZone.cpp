#include "YTE/GameComponents/demo_InsideZone.hpp"
#include "YTE/Physics/Transform.hpp"
#include "YTE/WWise/WWiseSystem.hpp"
#include "YTE/GameComponents/Zone.hpp"

namespace YTE
{

  YTEDefineType(demo_InsideZone)
  {
    YTERegisterType(demo_InsideZone);
  }

  demo_InsideZone::demo_InsideZone(Composition *aOwner, Space *aSpace, RSValue *aProperties)
    : Component(aOwner, aSpace)
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
      /*if (minDist < characterCalloutRange)
        {
          closestIsland->GetComponent<Island>().mCharacterCalloutString;
          play the sound for the character calling out
        }
      */
    }
  }

  // Assumes there is enough space between the two zones
  void demo_InsideZone::OnCollisionEnd(CollisionEnded *aEvent)
  {
    if (aEvent->OtherObject->GetComponent<Zone>()->GetZoneName() == Zone::ZoneName::PicanteIsles ||
        aEvent->OtherObject->GetComponent<Zone>()->GetZoneName() == Zone::ZoneName::RainyRuins)
    {
      // play ocean sound
      //mSoundEmitter->PlayEvent("");
    }
  }

  void demo_InsideZone::OnCollisionStart(CollisionStarted *aEvent)
  {
    if (aEvent->OtherObject->GetComponent<Zone>()->GetZoneName() == Zone::ZoneName::PicanteIsles)
    {
      // play picanteisles music event
      mSoundEmitter->PlayEvent("TD01_Enter");
    }
    else if (aEvent->OtherObject->GetComponent<Zone>()->GetZoneName() == Zone::ZoneName::RainyRuins)
    {
      // play rainyruins music event
      mSoundEmitter->PlayEvent("TD03_Enter");
    }
  }

}