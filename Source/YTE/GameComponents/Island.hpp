#pragma once

#ifndef YTE_Gameplay_Island_hpp
#define YTE_Gameplay_Island_hpp

#include "YTE/Core/Composition.hpp"
#include "YTE/Physics/GhostBody.hpp"
#include "YTE/Physics/BoxCollider.hpp"
#include "YTE/WWise/WWiseEmitter.hpp"

namespace YTE
{
  class Island : public Component
  {
  public:
    YTEDeclareType(Island);
    Island(Composition *aOwner, Space *aSpace, RSValue *aProperties);
    void Initialize() override;
    //void OnCollisionPersist(CollisionPersisted *aEvent);
    //void OnCollisionStart(CollisionStarted *aEvent);
    //void OnCollisionEnd(CollisionEnded *aEvent);
  private:
    BoxCollider *mIslandCollider;
    GhostBody *mIslandBody;
  };
}
#endif