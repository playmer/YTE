/******************************************************************************/
/*!
\file   BoatController.cpp
\author Jonathan Ackerman
\par    email: jonathan.ackerman\@digipen.edu
\date   2018-01-19

All content (c) 2016 DigiPen  (USA) Corporation, all rights reserved.
*/
/******************************************************************************/

#include "YTE/GameComponents/BoatController.hpp"

#include "YTE/Graphics/ParticleEmitter.hpp"
#include "YTE/Graphics/Camera.hpp"
#include "YTE/WWise/WWiseSystem.hpp"

#include "YTE/Physics/PhysicsSystem.hpp"
#include "YTE/Physics/RigidBody.hpp"
#include "YTE/Physics/BoxCollider.hpp"

#include "YTE/GameComponents/Island.hpp"
#include "YTE/GameComponents/Zone.hpp"

namespace YTE
{
    YTEDefineEvent(RequestDialogueStart);

    YTEDefineType(RequestDialogueStart) { YTERegisterType(RequestDialogueStart); }

    YTEDefineType(BoatController)
    {
        YTERegisterType(BoatController);


        YTEBindProperty(&BoatController::GetSailUpScalar, &BoatController::SetSailUpScalar, "Sail Up Scalar")
          .AddAttribute<Serializable>()
          .AddAttribute<EditorProperty>();

        YTEBindProperty(&BoatController::GetSailDownScalar, &BoatController::SetSailDownScalar, "Sail Down Scalar")
          .AddAttribute<Serializable>()
          .AddAttribute<EditorProperty>();

        YTEBindProperty(&BoatController::GetMaxSailSpeed, &BoatController::SetMaxSailSpeed, "Max Sail Speed")
          .AddAttribute<Serializable>()
          .AddAttribute<EditorProperty>();

        YTEBindProperty(&BoatController::GetMaxCruiseSpeed, &BoatController::SetMaxCruiseSpeed, "Max Cruise Speed")
          .AddAttribute<Serializable>()
          .AddAttribute<EditorProperty>();

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
      , mCanDock(false)
      , mNearbyDock(nullptr)
    {
        YTEUnusedArgument(aProperties);
    }
    
    void BoatController::Initialize()
    {
        /* Member Init */
        mRigidBody = mOwner->GetComponent<RigidBody>();
        mOrientation = mOwner->GetComponent<Orientation>();
        mTransform = mOwner->GetComponent<Transform>();
        mCollider = mOwner->GetComponent<BoxCollider>();
        mIsSailUp = false;
        mStartedTurning = false;
        mRotationAngle = 0.0f;
        mSoundEmitter = mOwner->GetComponent<WWiseEmitter>();

        /* Event Registration */
        mOwner->GetEngine()->YTERegister(Events::SailStateChanged, this, &BoatController::ChangeSail);
        mOwner->GetEngine()->YTERegister(Events::BoatTurnEvent, this, &BoatController::TurnBoat);
        mOwner->GetEngine()->YTERegister(Events::BoatDockEvent, this, &BoatController::DockBoat);
        mSpace->YTERegister(Events::LogicUpdate, this, &BoatController::Update);
        mOwner->YTERegister(Events::CollisionStarted, this, &BoatController::OnCollisionStart);
        mOwner->YTERegister(Events::CollisionEnded, this, &BoatController::OnCollisionEnd);
        
        /*
        //temp
        auto composition = mOwner->FindFirstCompositionByName("particles");
        if (composition != nullptr)
        {
          mEmitter = composition->GetComponent<ParticleEmitter>();
        }
        */

        mSoundSystem = mOwner->GetSpace()->GetComponent<WWiseSystem>();

        if (mSoundEmitter)
        {
          mSoundEmitter->PlayEvent("Sailing_Start");
        }
    }
/******************************************************************************/
/*
    Event Callbacks
*/
/******************************************************************************/
    void BoatController::DockBoat(BoatDockEvent *aEvent)
    {
      YTEUnusedArgument(aEvent);
      if (mCanDock)
      {
        // change input context
        mSpace->GetComponent<InputInterpreter>()->SetInputContext(InputInterpreter::InputContext::Dialogue);
        // send the request dialogue event
        RequestDialogueStart dStart;
        dStart.camera = nullptr;

        auto children = mOwner->GetCompositions();

        for (auto &it : *children)
        {
          if (it.second->GetComponent<Camera>())
          {
            dStart.camera = it.second.get();
            break;
          }
        }

        if (mNearbyDock)
        {
          mNearbyDock->SendEvent("RequestDialogueStart", &dStart);
        }

        // play the docking sound
        if (mSoundEmitter)
        {
          mSoundEmitter->PlayEvent("SFX_Boat_Bump");
        }
      }
    }

    void BoatController::TurnBoat(BoatTurnEvent *aEvent)
    {
      if (aEvent->StickDirection == glm::vec2(0, 0))
      {
        mStartedTurning = false;
      }

      if (!mStartedTurning && aEvent->StickDirection.x != 0.0f)
      {
        if (mSoundEmitter)
        {
          mSoundEmitter->PlayEvent("SFX_Boat_Turn");
        }

        mStartedTurning = true;
      }

      auto rotationDelta = -(aEvent->StickDirection.x / 2.0f) * static_cast<float>(mOwner->GetEngine()->GetDt());
      mTransform->Rotate({0, 1, 0 }, rotationDelta);
    }

    void BoatController::ChangeSail(SailStateChanged *aEvent)
    {
      std::string sound;
      if (aEvent->SailUp)
      {
        sound = "SFX_Sail_Up";
      }
      else
      {
        sound = "SFX_Sail_Down";
      }

      if (mSoundEmitter)
      {
        mSoundEmitter->PlayEvent(sound);
      }

      mIsSailUp = aEvent->SailUp;
    }

    void BoatController::Update(LogicUpdate *aEvent)
    {
      if (!mRigidBody)
      {
        return;
      }

      float dt = static_cast<float>(aEvent->Dt);
      /*
      if (mParticleEmitter != NULL)
      {
        mEmitter->SetInitVelocity(glm::vec3(mOrientation->GetForwardVector().x, -0.5f, mOrientation->GetForwardVector().z) * -2.0f);
      }
      */
      //glm::vec3 moveVec = CalculateMovementVector(aEvent->Dt);
      //ApplyMovementVector(moveVec);

      glm::vec3 vel = mRigidBody->GetVelocity();

      mCurSpeed = glm::length(vel);
      mSoundSystem->SetRTPC("Sailing_Volume", mCurSpeed);

      if (mIsSailUp)
      {
        auto gamepad = mOwner->GetEngine()->GetGamepadSystem()->GetXboxController(Controller_Id::Xbox_P1);
        
        if (gamepad->IsButtonDown(Xbox_Buttons::DPAD_Left))
        {
          mRigidBody->SetVelocity(mOrientation->GetUpVector() * 1000.0f * mMaxSailSpeed * dt);
        }
        else if (gamepad->IsButtonDown(Xbox_Buttons::DPAD_Right))
        {
          mRigidBody->SetVelocity(mOrientation->GetRightVector() * 400.0f * mMaxSailSpeed * dt);
        }
        else
        {
          mRigidBody->SetVelocity(mOrientation->GetForwardVector() * 500.0f * mMaxSailSpeed * dt);
        }

        //btTransform trans;
        //mRigidBody->GetBody()->getMotionState()->getWorldTransform(trans);
        //
        //float matr[16]{ 0.0f };
        //
        //trans.getOpenGLMatrix(matr);
        //
        //float x = matr[12];
        //float y = matr[13];
        //float z = matr[14];
        //
        //std::cout << "Position :  (" << x << ", " << y << ", " << z << ")" << std::endl;
        
        //if (mCurSpeed < 50.0f)
        //{
        //  mRigidBody->ApplyImpulse(mOrientation->GetForwardVector() * aEvent->Dt, 
        //                           mTransform->GetTranslation());
        //}
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

    float BoatController::GetSailUpScalar()
    {
      return mSailUpScalar;
    }

    void BoatController::SetSailUpScalar(float aSpeed)
    {
      mSailUpScalar = aSpeed;
    }

    float BoatController::GetSailDownScalar()
    {
      return mSailDownScalar;
    }

    void BoatController::SetSailDownScalar(float aSpeed)
    {
      mSailDownScalar = aSpeed;
    }

    float BoatController::GetMaxSailSpeed()
    {
      return mMaxSailSpeed;
    }
    
    void BoatController::SetMaxSailSpeed(float aSpeed)
    {
      mMaxSailSpeed = aSpeed;
    }

    float BoatController::GetMaxCruiseSpeed()
    {
      return mMaxCruiseSpeed;
    }

    void BoatController::SetMaxCruiseSpeed(float aSpeed)
    {
      mMaxCruiseSpeed = aSpeed;
    }
    
    void BoatController::OnCollisionStart(CollisionStarted *aEvent)
    {
      if (aEvent->OtherObject->GetComponent<Island>() != nullptr)
      {
        mCanDock = true;
        mNearbyDock = aEvent->OtherObject;
      }
    }
    void BoatController::OnCollisionEnd(CollisionEnded *aEvent)
    {
      if (aEvent->OtherObject->GetComponent<Island>() != nullptr)
      {
        mCanDock = false;
        mNearbyDock = nullptr;
      }
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
      //mRigidBody->ApplyImpulse(aImpulse, glm::vec3(0, 0, 0));
    }
    
}//end yte namespace