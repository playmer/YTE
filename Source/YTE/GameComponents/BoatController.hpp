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
  YTEDeclareEvent(RequestDialogueStart);

  class RequestDialogueStart : public Event
  {
  public:
    YTEDeclareType(RequestDialogueStart);
    RequestDialogueStart() {  };

    Composition *camera;
  };

/////////////////////////////////////////////////////////////////////////////////////
// Class
/////////////////////////////////////////////////////////////////////////////////////

  class BoatController : public Component
  {
  public:
    //void LowerSail(); callback to lower the sail when we hear a lowersail event or smth
    YTEDeclareType(BoatController);
    BoatController(Composition *aOwner, Space *aSpace, RSValue *aProperties);
    void Initialize() override;
    void ChangeSail(SailStateChanged *aEvent);
    void TurnBoat(BoatTurnEvent *aEvent);
    void DockBoat(BoatDockEvent *aEvent);
    void Update(LogicUpdate *aEvent);
    void OnCollisionStart(CollisionStarted *aEvent);
    void OnCollisionEnd(CollisionEnded *aEvent);

    // PROPERTIES ///////////////////////////////////////////////////////////////////
    float GetMaxSailSpeed() { return mMaxSailSpeed; }
    void SetMaxSailSpeed(float& aMaxSpeed) { mMaxSailSpeed = aMaxSpeed; }

    float GetRotationSpeed() { return mRotationSpeed; }
    void SetRotationSpeed(float& aSpeed) { mRotationSpeed = aSpeed; }

    float GetWindForce() { return mWindForce; }
    void SetWindForce(float& aForce) { mWindForce = aForce; }

    float GetDecelerationForce() { return -mDecelerationForce; }
    void SetDecelerationForce(float& aForce) { mDecelerationForce = -aForce; }
    /////////////////////////////////////////////////////////////////////////////////

  private:
    glm::vec3 CalculateMovementVector(float dt);
    void ApplyMovementVector(glm::vec3 aImpulse);
    //void ApplyForceToBoat();
    //void RollBoat();

    Transform *mTransform;
    Orientation *mOrientation;
    RigidBody *mRigidBody;
    BoxCollider *mCollider;
    ParticleEmitter *mParticleEmitter;
    WWiseEmitter *mSoundEmitter;
    Composition *mNearbyDock; // be careful with this lambs

    WWiseSystem *mSoundSystem;

    float mMaxSailSpeed;
    float mRotationSpeed;
    float mWindForce;
    float mDecelerationForce;

    //Model *mSailModel; wherever i play anims for sails
    //Transform *mSailOrient; sail might have a different transform to rotate around

    // Flag that represents the state of the boat's sail
    bool mIsSailUp;
    // flag for docking
    bool mCanDock;
    // flag for turning
    bool mStartedTurning;

        float mSailUpScalar;
        float mSailDownScalar;
        float mCurSpeed;
        float mMaxSailSpeed;
        float mMaxCruiseSpeed;

        // Vector that represents the direction of the force to apply to the boat (move elsewhere?)
        glm::vec3 mMoveDir;
        //glm::vec3 mForwardVec;
        float mRotationAngle; // @@@TODO: take this out and add a Rotate() on the Transform later
        //float mSailMaxSpeed;
        //float mCurBoatRoll; i think this is going to change to a max roll
    };
} // End YTE namespace

#endif
