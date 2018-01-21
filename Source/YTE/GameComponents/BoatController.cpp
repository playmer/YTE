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
    
    BoatController::BoatController(Composition *aOwner, Space *aSpace)
        : Component(aOwner, aSpace)
    {
    }
    
    void BoatController::Initialize()
    {
        // Event Registration
        mOwner->YTERegister(Events::SailStateChanged, this, &BoatController::ChangeSail);
        mOwner->YTERegister(Events::LogicUpdate, this, &BoatController::Update);
        // Member Init
        mForwardVec = mOwner->GetComponent<Orientation>()->GetForwardVector();
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
        CalculateMovementVector(aEvent->Dt);
        ApplyMovementVector();
    }
/******************************************************************************/
/*
    Helper Functions
*/
/******************************************************************************/
    
    void BoatController::CalculateMovementVector(float dt)
    {
        //@@@ ask josh about setting a dependency for orientation, transform, etc
        if (mIsSailUp)
        {
            if (mCurSpeed < mMaxSailSpeed)
            {
                //mMoveVec += mForwardVec * mSailUpScalar * dt;
                //Adds in WindDir * WindForce (wherever these come from)
                //Adds in OceanCurrent * OceanForce (wherever those come from)
            }
        }
        else
        {
            if (mCurSpeed < mMaxCruiseSpeed)
            {
                //mMoveVec += mForwardVec * mSailDownScalar;
            }
        }
        // mMoveVec = result
    }

    void BoatController::ApplyMovementVector()
    {
        // mRigidBody->ApplyImpulse()
    }
    
}//end yte namespace