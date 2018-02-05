/******************************************************************************/
/*!
\file   BoatController.hpp
\author Jonathan Ackerman
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
#include "YTE/Core/EventHandler.hpp"
#include "YTE/GameComponents/InputInterpreter.hpp"

namespace YTE
{
    class BoatController : public Component
    {
    public:
        //void LowerSail(); callback to lower the sail when we hear a lowersail event or smth
        YTEDeclareType(BoatController);
        BoatController(Composition *aOwner, Space *aSpace, RSValue *aProperties);
        void Initialize() override;
        void ChangeSail(SailStateChanged *aEvent);
        void TurnBoat(BoatTurnEvent *aEvent);
        void Update(LogicUpdate *aEvent);

        float GetSailUpScalar();
        void SetSailUpScalar(float aSpeed);

        float GetSailDownScalar();
        void SetSailDownScalar(float aSpeed);

        float GetMaxSailSpeed();
        void SetMaxSailSpeed(float aSpeed);

        float GetMaxCruiseSpeed();
        void SetMaxCruiseSpeed(float aSpeed);


    private:
        glm::vec3 CalculateMovementVector(float dt);
        void ApplyMovementVector(glm::vec3 aImpulse);
        //void ApplyForceToBoat();
        //void RollBoat();

        Transform *mTransform;
        Orientation *mOrientation;
        RigidBody *mRigidBody;
        ParticleEmitter *mEmitter;
        //Model *mSailModel; wherever i play anims for sail
        //Transform *mSailOrient; sail might have a different transform to rotate around

        // Flag that represents the state of the boat's sail
        bool mIsSailUp;

        float mSailUpScalar = 10.0f; // @@@ make sure these get set in editor not here
        float mSailDownScalar = 5.0f;
        float mCurSpeed = 0.0f;
        float mMaxSailSpeed = 10.0f;
        float mMaxCruiseSpeed = 5.0f;

        // Vector that represents the direction of the force to apply to the boat (move elsewhere?)
        glm::vec3 mMoveVec = { 0, 0, 0 };
        glm::vec3 mForwardVec;
        float mRotationAngle; // @@@TODO: take this out and add a Rotate() on the Transform later
        //float mSailMaxSpeed;
        //float mCurBoatRoll; i think this is going to change to a max roll
    };
} // End YTE namespace

#endif
