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
#include "YTE/Core/EventHandler.hpp"
#include "YTE/GameComponents/InputInterpreter.hpp"

namespace YTE
{
    class BoatController : public Component
    {
    public:
        //void LowerSail(); callback to lower the sail when we hear a lowersail event or smth
        YTEDeclareType(BoatController);
        //BoatController(Composition *aOwner, Space *aSpace);
        void Initialize() override;
        void ChangeSailState(ToggleSailEvent *aEvent);
        //i think this bad boy just listens to input events and has callbacks to update rotation or apply a force

    private:
        //void ApplyForceToBoat();
        //void RollBoat();

        //Transform *mTransform;
        //RigidBody *mRigidBody;
        //Model *mSailModel; wherever i play anims for sail
        //Transform *mSailOrient; sail might have a different transform to rotate around
        //Orientation *mOrientation;
        //bool mIsSailUp;
        //float mSailMaxSpeed;
        //float mCurBoatRoll; i think this is going to change to a max roll
    };
} // End YTE namespace

#endif
