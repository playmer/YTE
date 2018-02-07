#pragma once

#ifndef YTE_Gameplay_Zone_hpp
#define YTE_Gameplay_Zone_hpp

#include "YTE/Core/Composition.hpp"
#include "YTE/Physics/GhostBody.hpp"
#include "YTE/Physics/BoxCollider.hpp"

namespace YTE
{
  class Zone : public Component
  {
  public:
    YTEDeclareType(Zone);
    Zone(Composition *aOwner, Space *aSpace, RSValue *aProperties);
    void Initialize() override;
    std::string GetZoneName() { return mZoneName; };
    void SetZoneName(std::string aName) { mZoneName = aName; };
    //void OnCollisionPersist(CollisionPersisted *aEvent);
    //void OnCollisionStart(CollisionStarted *aEvent);
    //void OnCollisionEnd(CollisionEnded *aEvent);
  private:
    BoxCollider *mZoneCollider;
    GhostBody *mZoneBody;
    std::string mZoneName;
  };
}
#endif