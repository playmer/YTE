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
#include "YTE/Physics/RigidBody.hpp"
#include "YTE/Physics/Collider.hpp"

namespace YTE
{
    YTEDefineType(BoatController)
    {
        YTERegisterType(BoatController);

        std::vector<std::vector<Type*>> deps =
        {
            { Transform::GetStaticType() },
            { Orientation::GetStaticType() },
            { RigidBody::GetStaticType() }
        };
        BoatController::GetStaticType()->AddAttribute<ComponentDependencies>(deps);
    }
    
    BoatController::BoatController(Composition *aOwner, Space *aSpace, RSValue *aProperties)
        : Component(aOwner, aSpace)
    {
        YTEUnusedArgument(aProperties);
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
        std::cout << "SAIL UP: " + mIsSailUp;
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