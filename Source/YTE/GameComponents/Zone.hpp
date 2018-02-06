#pragma once

#ifndef YTE_Gameplay_Zone_hpp
#define YTE_Gameplay_Zone_hpp

#include "YTE/Core/Composition.hpp"
#include "YTE/Physics/GhostBody.hpp"
#include "YTE/Physics/BoxCollider.hpp"
#include "YTE/WWise/WWiseEmitter.hpp"

namespace YTE
{
  class Zone : public Component
  {
  public:
    enum class ZoneName { PicanteIsles, RainyRuins, none };
    YTEDeclareType(Zone);
    Zone(Composition *aOwner, Space *aSpace, RSValue *aProperties);
    void Initialize() override;
    ZoneName GetZoneName() { return mZoneName; };
    void SetZoneName(ZoneName aName) { mZoneName = aName; };
    //void OnCollisionPersist(CollisionPersisted *aEvent);
    //void OnCollisionStart(CollisionStarted *aEvent);
    //void OnCollisionEnd(CollisionEnded *aEvent);
  private:
    BoxCollider *mZoneCollider;
    GhostBody *mZoneBody;
    ZoneName mZoneName;
  };
}
#endif