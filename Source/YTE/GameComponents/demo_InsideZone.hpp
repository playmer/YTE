#pragma once

#ifndef YTE_Gameplay_demoZone_hpp
#define YTE_Gameplay_demoZone_hpp

#include "YTE/Core/Composition.hpp"
#include "YTE/Physics/GhostBody.hpp"
#include "YTE/Physics/Collider.hpp"
#include "YTE/WWise/WWiseEmitter.hpp"
#include "YTE/WWise/WWiseSystem.hpp"
#include "YTE/GameComponents/Zone.hpp"

#include <unordered_set>

namespace YTE
{
  //maybe ChangeZone would have been a better name idk
  class demo_InsideZone : public Component
  {
  public:
    YTEDeclareType(demo_InsideZone);
    demo_InsideZone(Composition *aOwner, Space *aSpace, RSValue *aProperties);
    void Initialize() override;
    void Deinitialize() override;
    void OnCollisionPersist(CollisionPersisted *aEvent);
    void OnCollisionStart(CollisionStarted *aEvent);
    void OnCollisionEnd(CollisionEnded *aEvent);
  private:
    WWiseEmitter *mSoundEmitter;
    Collider *mBoatCollider;
    GhostBody *mBoatBody;
    WWiseSystem *mSoundSystem;
    glm::vec3 mBoatPosition;

    u64 mIslandEnter;
    u64 mIslandLeave;
    u64 mSailingStart;
    u64 mSailingStop;
    std::unordered_set<Zone*> mCollidingIslands;

    std::string mCurrentZone;

    bool mFlag;

  };
}
#endif