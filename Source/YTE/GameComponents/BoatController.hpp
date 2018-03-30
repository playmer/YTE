/******************************************************************************/
/*!
\file   BoatController.hpp
\author Isaac Dayton
        Jonathan Ackerman
\par    email: jonathan.ackerman\@digipen.edu
\date   2018-01-19
\brief
    This component controls the logic for a boat. If the boat is player controlled
    it will have an accompanying InputInterpret. If the boat is AI controlled it
    will have an accompanying AiInterpreter.

All content (c) 2016 DigiPen  (USA) Corporation, all rights reserved.
*/
/******************************************************************************/

#pragma once

#ifndef YTE_Gameplay_BoatController_hpp
#define YTE_Gameplay_BoatController_hpp

#include "YTE/Core/Composition.hpp"
#include "YTE/Core/ForwardDeclarations.hpp"

#include "YTE/Physics/Transform.hpp"
#include "YTE/Physics/Orientation.hpp"
#include "YTE/Physics/BoxCollider.hpp"
#include "YTE/GameComponents/InputInterpreter.hpp"
#include "YTE/WWise/WWiseEmitter.hpp"
#include "YTE/WWise/WWiseSystem.hpp"

namespace YTE
{
/////////////////////////////////////////////////////////////////////////////////////
// Events
/////////////////////////////////////////////////////////////////////////////////////


/////////////////////////////////////////////////////////////////////////////////////
// Class
/////////////////////////////////////////////////////////////////////////////////////

  class BoatController : public Component
  {
  public:
    YTEDeclareType(BoatController);
    BoatController(Composition *aOwner, Space *aSpace, RSValue *aProperties);
    void Initialize() override;
    void Deinitialize() override;
    void ChangeSail(SailStateChanged *aEvent);
    void TurnBoat(BoatTurnEvent *aEvent);
    void DockBoat(BoatDockEvent *aEvent);
    void Update(LogicUpdate *aEvent);

    // PROPERTIES ///////////////////////////////////////////////////////////////////
    float GetMaxSailSpeed() { return mMaxSailSpeed; }
    void SetMaxSailSpeed(float& aMaxSpeed) { mMaxSailSpeed = aMaxSpeed; }

    float GetRotationSpeed() { return mMaxTurnSpeed; }
    void SetRotationSpeed(float& aSpeed) { mMaxTurnSpeed = aSpeed; }

    float GetTurnAccFactor() { return mTurnAccFactor; }
    void SetTurnAccFactor(float& aFactor) { mTurnAccFactor = aFactor; }

    float GetWindForce() { return mWindForce; }
    void SetWindForce(float& aForce) { mWindForce = aForce; }
    /////////////////////////////////////////////////////////////////////////////////

    float GetCurrentSpeed() const;

  private:
    Transform *mTransform;
    Orientation *mOrientation;
    RigidBody *mRigidBody;
    BoxCollider *mCollider;
    ParticleEmitter *mParticleEmitter;
    WWiseEmitter *mSoundEmitter;
    Composition *mNearbyDock; // be careful with this lambs

    float mMaxSailSpeed;
    float mMaxTurnSpeed;
    float mTurnAccFactor;
    float mWindForce;

    glm::vec3 mTurnVec;

    //Model *mSailModel; wherever i play anims for sails
    //Transform *mSailOrient; sail might have a different transform to rotate around

    bool mIsSailUp;
    bool mIsDocked;
    bool mStartedTurning;
    bool mPlayingTurnSound;

    float mCurrSpeed;
    float mCurrRotSpeed;

    u64 mSoundSailUp;
    u64 mSoundSailDown;
    u64 mSoundBumpDock;
    u64 mSoundBoatTurn;
    u64 mSailingStart;
    u64 mSailingStop;

    Animator *mAnimator;
    Animator *mSailsAnimator;
    Animator *mMainsailAnimator;
    Animator *mCharacterAnimator;

    float mTurnAmount;

    void StopBoatImmediately();
  };
} 

#endif
