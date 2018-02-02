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

#include "YTE/Graphics/ParticleEmitter.hpp"

#include "YTE/Physics/PhysicsSystem.hpp"
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
        mOwner->GetEngine()->YTERegister(Events::SailStateChanged, this, &BoatController::ChangeSail);
        mOwner->GetEngine()->YTERegister(Events::BoatTurnEvent, this, &BoatController::TurnBoat);

        mOwner->GetSpace()->YTERegister(Events::LogicUpdate, this, &BoatController::Update);
        // Member Init
        //mForwardVec = mOwner->GetComponent<Orientation>()->GetForwardVector();
        mRigidBody = mOwner->GetComponent<RigidBody>();
        mOrientation = mOwner->GetComponent<Orientation>();
        mTransform = mOwner->GetComponent<Transform>();
        mIsSailUp = false;
        mRotationAngle = 0.0f;
        //mForwardVec = mOrientation->GetForwardVector();

        /*
        //temp
        auto composition = mOwner->FindFirstCompositionByName("particles");
        if (composition != nullptr)
        {
          mEmitter = composition->GetComponent<ParticleEmitter>();
        }
        */
    }
/******************************************************************************/
/*
    Event Callbacks
*/
/******************************************************************************/
    void BoatController::TurnBoat(BoatTurnEvent *aEvent)
    {
      if (aEvent->Stick == Xbox_Buttons::LeftStick)
      {
        mRotationAngle -= aEvent->StickDirection.x / 2.0f;
        if (mRotationAngle > 360.0f)
        {
          mRotationAngle = 0.0f;
        }
        if (mRotationAngle < 0.0f)
        {
          mRotationAngle = 360.0f;
        }
      }
    }

    void BoatController::ChangeSail(SailStateChanged *aEvent)
    {
        mIsSailUp = aEvent->SailUp;
    }

    void BoatController::Update(LogicUpdate *aEvent)
    {
      /*
      if (mEmitter != NULL)
      {
        mEmitter->SetInitVelocity(glm::vec3(mOrientation->GetForwardVector().x, -0.5f, mOrientation->GetForwardVector().z) * -2.0f);
      }
      */
      //glm::vec3 moveVec = CalculateMovementVector(aEvent->Dt);
      //ApplyMovementVector(moveVec);
      mTransform->SetRotationProperty(glm::vec3(0, mRotationAngle, 0));
      if (mIsSailUp)
      {
        mRigidBody->SetVelocity(mOrientation->GetForwardVector() * 1000.0f * aEvent->Dt);

        /*
        if (mCurSpeed < mMaxSailSpeed)
        {
          mRigidBody->ApplyImpulse(mOrientation->GetForwardVector() * aEvent->Dt * 10.0f, glm::vec3(0, 0, 0));
        }
        */
      }
      else
      {
        mRigidBody->SetVelocity(glm::vec3(0,0,0));
        /*
        if (mCurSpeed > 0.0f)
        {
          mRigidBody->ApplyImpulse(mOrientation->GetForwardVector() * aEvent->Dt * -10.0f, glm::vec3(0, 0, 0));
        }
        else
        {
          mRigidBody->SetVelocity(glm::vec3(0, 0, 0));
        }
        */
      }
      //auto temp = mRigidBody->GetVelocity().length;
      //mCurSpeed;
    }
/******************************************************************************/
/*
    Helper Functions
*/
/******************************************************************************/
    
    glm::vec3 BoatController::CalculateMovementVector(float dt)
    {
      return glm::vec3(dt, dt, dt);
      /*
      glm::vec3 curVec = mRigidBody->GetVelocity();
      glm::vec3 newVec = glm::vec3(0, 0, 0);
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
      btVector3 btCurVec = OurVec3ToBt(curVec);
      btVector3 btNewVec = OurVec3ToBt(newVec);
      
      return impulse;
      */
    }

    void BoatController::ApplyMovementVector(glm::vec3 aImpulse)
    {
      mRigidBody->ApplyImpulse(aImpulse, glm::vec3(0, 0, 0));
    }
    
}//end yte namespace