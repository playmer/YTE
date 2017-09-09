/******************************************************************************/
/*!
 * \author Joshua T. Fisher
 * \date   2015-11-20
 *
 * \copyright All content 2016 DigiPen (USA) Corporation, all rights reserved.
*/
/******************************************************************************/
#include "YTE/Core/Engine.hpp"
#include "YTE/Core/Space.hpp"

#include "YTE/Physics/PhysicsSystem.h"
#include "YTE/Physics/Body.h"
#include "YTE/Physics/Transform.h"

namespace YTE
{
  DefineType(Body)
  {
    YTERegisterType(Body);
  }
    
  Body::Body(Composition *aOwner, Space *aSpace, RSValue *aProperties)
    : Component(aOwner, aSpace)
  {
    DeserializeByType<Body*>(aProperties, this, Body::GetStaticType());
    mSpace->CONNECT(Events::LogicUpdate, this, &Body::OnLogicUpdate);
  };

  Body::~Body()
  {
  }

  void Body::OnLogicUpdate(LogicUpdate *aEvent)
  {
    CollisionEvent newEvent;
    newEvent.OtherObject = nullptr;

    for (auto composition : mCollidedThisFrame)
    {
      auto prevComposition = std::find(mCollidedLastFrame.begin(), mCollidedLastFrame.end(), composition);

      newEvent.OtherObject = composition;

      if (prevComposition == mCollidedLastFrame.end())
      {
        // Collision Started
        mOwner->SendEvent(Events::CollisionStarted, &newEvent);
      }
      else
      {
        // Collision Persisted
        mOwner->SendEvent(Events::CollisionPersisted, &newEvent);

        // Remove Composition From mCollidedLastFrame
        mCollidedLastFrame.erase(prevComposition);
      }
    }

    for (auto composition : mCollidedLastFrame)
    {
      // Collision Ended
      newEvent.OtherObject = composition;
      mOwner->SendEvent(Events::CollisionEnded, &newEvent);
    }

    mCollidedLastFrame.clear();

    std::swap(mCollidedLastFrame, mCollidedThisFrame);
  }

  void Body::AddCollidedThisFrame(Composition *aComposition)
  {
    auto prevComposition = std::find(mCollidedThisFrame.begin(), mCollidedThisFrame.end(), aComposition);

    if (prevComposition == mCollidedThisFrame.end())
    {
      mCollidedThisFrame.emplace_back(aComposition);
    }
  }
}
