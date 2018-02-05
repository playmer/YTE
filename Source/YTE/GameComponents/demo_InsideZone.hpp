#pragma once

#ifndef YTE_Gameplay_demoZone_hpp
#define YTE_Gameplay_demoZone_hpp

#include "YTE/Core/Composition.hpp"
#include "YTE/Physics/GhostBody.hpp"
#include "YTE/Physics/Collider.hpp"

namespace YTE
{
  //maybe ChangeZone would have been a better name idk
  class demo_InsideZone : public Component
  {
  public:
    YTEDeclareType(demo_InsideZone);
    demo_InsideZone(Composition *aOwner, Space *aSpace, RSValue *aProperties);
    void Initialize() override;
    void OnCollisionPersist(CollisionPersisted *aEvent);
    void OnCollisionStart(CollisionStarted *aEvent);
    void OnCollisionEnd(CollisionEnded *aEvent);
  private:
    Collider *mBoatCollider;
    GhostBody *mBoatBody;
    glm::vec3 mBoatPosition;
  };
}
#endif