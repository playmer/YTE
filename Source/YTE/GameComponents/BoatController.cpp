/******************************************************************************/
/*!
\file   BoatController.hpp
\author Jonathan Ackerman
\par    email: jonathan.ackerman\@digipen.edu
\date   2018-01-19

All content (c) 2016 DigiPen  (USA) Corporation, all rights reserved.
*/
/******************************************************************************/

#include "YTE/GameComponents/BoatController.hpp"

namespace YTE
{
    YTEDefineType(BoatController)
    {
        YTERegisterType(BoatController);
    }
    /*
    BoatController::BoatController(Composition *aOwner, Space *aSpace)
        : Component(aOwner, aSpace)
    {
    }
    */
    void BoatController::Initialize()
    {
        // Event Registration
        mOwner->YTERegister(Events::SailStateChanged, this, &BoatController::ChangeSail);
        mOwner->YTERegister(Events::LogicUpdate, this, &BoatController::Update);
    }
/******************************************************************************/
/*
    Event Callbacks
*/
/******************************************************************************/
    void BoatController::ChangeSail(SailStateChanged *aEvent)
    {
        mIsSailUp = aEvent->SailUp;
        std::cout << mIsSailUp;
    }

    void BoatController::Update(LogicUpdate *aEvent)
    {
        //float ForceMultiplier;
        if (mIsSailUp)
        {
            //ForceMultiplier = 10;?
        }
        else
        {
            //ForceMultiplier = 1;?
        }
        //CalculateForce(ForceMultiplier);
        //ApplyForce()
    }
/******************************************************************************/
/*
    Helper Functions
*/
/******************************************************************************/
    /*
    void CalculateForce(float aForce)
    {
        // Takes the ForwardDir * aForce
        // if mIsSailUp
            // Adds in WindDir * WindForce (wherever these come from)
        // mMoveVec = result
    }

    void ApplyForce()
    {
        // Boat.ApplyForce(mMoveVec)
    }
    */
}//end yte namespace