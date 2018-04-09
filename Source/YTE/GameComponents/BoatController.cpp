/******************************************************************************/
/*!
\file   BoatController.cpp
\author Isaac Dayton
        Jonathan Ackerman
\par    email: jonathan.ackerman\@digipen.edu
\date   2018-01-19

All content (c) 2016 DigiPen  (USA) Corporation, all rights reserved.
*/
/******************************************************************************/

#include "YTE/GameComponents/BoatController.hpp"

#include "YTE/Core/Actions/Action.hpp"

#include "YTE/Graphics/Animation.hpp"
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
  YTEDefineType(BoatController)
  {
    YTERegisterType(BoatController);


    YTEBindProperty(&BoatController::GetMaxSailSpeed, &BoatController::SetMaxSailSpeed, "MaxSailSpeed")
      .AddAttribute<Serializable>()
      .AddAttribute<EditorProperty>();

    YTEBindProperty(&BoatController::GetRotationSpeed, &BoatController::SetRotationSpeed, "MaxTurnSpeed")
      .AddAttribute<Serializable>()
      .AddAttribute<EditorProperty>();

    YTEBindProperty(&BoatController::GetTurnAccFactor, &BoatController::SetTurnAccFactor, "TurnAccelerationFactor")
      .AddAttribute<Serializable>()
      .AddAttribute<EditorProperty>();

    YTEBindProperty(&BoatController::GetWindForce, &BoatController::SetWindForce, "WindForce")
      .AddAttribute<Serializable>()
      .AddAttribute<EditorProperty>();

    std::vector<std::vector<Type*>> deps =
    {
      { TypeId<Transform>() },
      { TypeId<Orientation>() },
      { TypeId<RigidBody>() }
    };

    GetStaticType()->AddAttribute<ComponentDependencies>(deps);
  }

  BoatController::BoatController(Composition *aOwner, Space *aSpace, RSValue *aProperties)
    : Component(aOwner, aSpace)
    , mNearbyDock(nullptr)
    , mPlayingTurnSound(false)
    , mCurrSpeed(0.f)
    , mCurrRotSpeed(0.f)
    , mAnimator(nullptr)
    , mSailsAnimator(nullptr)
    , mMainsailAnimator(nullptr)
    , mTargetRotationAmount(0.0f)
    , mCurrentRotationAmount(0.0f)
  {

    mMaxSailSpeed = 25.0f;
    mMaxTurnSpeed = 0.8f;
    mTurnAccFactor = 0.5f;
    mWindForce = 10.f;

    DeserializeByType(aProperties, this, GetStaticType());
  }

  void BoatController::Initialize()
  {
    /* Member Init */
    mRigidBody = mOwner->GetComponent<RigidBody>();
    mOrientation = mOwner->GetComponent<Orientation>();
    mTransform = mOwner->GetComponent<Transform>();
    mCollider = mOwner->GetComponent<BoxCollider>();
    mIsSailUp = false;
    mIsDocked = false;
    mStartedTurning = false;
    mSoundEmitter = mOwner->GetComponent<WWiseEmitter>();

    mRigidBody->SetDamping(0.9f, 0.9f);

      // Cache ids for all sounds used by this component
    auto soundSystem = mSpace->GetEngine()->GetComponent<WWiseSystem>();

    if (soundSystem)
    {
      mSoundSailUp   = soundSystem->GetSoundIDFromString("SFX_Sail_Up");
      mSoundSailDown = soundSystem->GetSoundIDFromString("SFX_Sail_Down");
      mSoundBumpDock = soundSystem->GetSoundIDFromString("SFX_Boat_Bump");
      mSoundBoatTurn = soundSystem->GetSoundIDFromString("SFX_Boat_Turn");
      mSailingStart  = soundSystem->GetSoundIDFromString("Sailing_Start");
      mSailingStop   = soundSystem->GetSoundIDFromString("Sailing_Stop");
    }

    mSoundEmitter->PlayEvent(mSailingStart);

    /* Event Registration */
    mSpace->YTERegister(Events::SailStateChanged, this, &BoatController::ChangeSail);
    mSpace->YTERegister(Events::BoatTurnEvent, this, &BoatController::TurnBoat);
    mSpace->YTERegister(Events::BoatDockEvent, this, &BoatController::DockBoat);
    mSpace->YTERegister(Events::LogicUpdate, this, &BoatController::Update);

    mAnimator = mOwner->GetComponent<Animator>();
    
    if (Composition *sails = mOwner->FindFirstCompositionByName("Sails"))
    {
      mSailsAnimator = sails->GetComponent<Animator>();
    }
    
    if (Composition *mainsail = mOwner->FindFirstCompositionByName("Mainsail"))
    {
      mMainsailAnimator = mainsail->GetComponent<Animator>();
    }

    if (Composition *character= mOwner->FindFirstCompositionByName("MainCharacter"))
    {
      mCharacterAnimator = character->GetComponent<Animator>();
    }

    if (mAnimator)
    {
      mAnimator->SetDefaultAnimation("Boat_Turn.fbx");
      mAnimator->SetCurrentAnimation("Boat_Turn.fbx");
    }

    if (mSailsAnimator)
    {
      mSailsAnimator->SetDefaultAnimation("Boat_Turn.fbx");
      mSailsAnimator->SetCurrentAnimation("Boat_Turn.fbx");
    }

    if (mMainsailAnimator)
    {
      mMainsailAnimator->SetDefaultAnimation("Boat_Turn.fbx");
      mMainsailAnimator->SetCurrentAnimation("Boat_Turn.fbx");
    }

    if (mCharacterAnimator)
    {
      mCharacterAnimator->SetDefaultAnimation("MC_Boat_Turn.fbx");
      mCharacterAnimator->SetCurrentAnimation("MC_Boat_Turn.fbx");
    }

  }

  void BoatController::Deinitialize()
  {
    mSoundEmitter->PlayEvent(mSailingStop);
  }

  /******************************************************************************/
  /*
    Event Callbacks
  */
  /******************************************************************************/
  void BoatController::DockBoat(BoatDockEvent *aEvent)
  {
    YTEUnusedArgument(aEvent);

    StopBoatImmediately();

    if (mSoundEmitter)
    {
      mSoundEmitter->PlayEvent(mSoundBumpDock);
    }
  }

  void BoatController::ChangeSail(SailStateChanged *aEvent)
  {
    if (aEvent->SailUp)
    {
      if (!mIsSailUp)
      {
        mSoundEmitter->PlayEvent(mSoundSailUp);
        mRigidBody->SetDamping(0.f, 0.9f);

        mOwner->FindFirstCompositionByName("Mainsail")->GetComponent<Transform>()->SetScale(glm::vec3(1.0f));
        mOwner->FindFirstCompositionByName("Sails")->GetComponent<Transform>()->SetScale(glm::vec3(1.0f));
      }
    }
    else
    {
      if (mIsSailUp)
      {
        mSoundEmitter->PlayEvent(mSoundSailDown);
        mRigidBody->SetDamping(0.9f, 0.9f);

        mOwner->FindFirstCompositionByName("Mainsail")->GetComponent<Transform>()->SetScale(glm::vec3(0.0f));
        mOwner->FindFirstCompositionByName("Sails")->GetComponent<Transform>()->SetScale(glm::vec3(0.0f));
      }
    }

    mIsSailUp = aEvent->SailUp;
  }

  void BoatController::TurnBoat(BoatTurnEvent *aEvent)
  {
    mStartedTurning = true;
    if (!mPlayingTurnSound)
    {
      if (mCurrRotSpeed > mMaxTurnSpeed * 0.75f)
      {
        mSoundEmitter->PlayEvent(mSoundBoatTurn);
        mPlayingTurnSound = true;
      }
    }

    mTargetRotationAmount = 0.35 * aEvent->StickDirection.x;

      // Dead-zone check and apply response curves
    float length = glm::length(aEvent->StickDirection);

    if (length > 0.01f)
    {
      float absX = glm::abs(aEvent->StickDirection.x);

      float turnScale;
      float startVal = 0.0f;
      float change = 1.0f / 0.3f;
      float duration = 1.0f;

      if (absX < 0.3f)
      {
        Quad::easeIn::Ease(turnScale, startVal, change, absX, duration);
      }
      else if (absX < 0.7f)
      {
        turnScale = absX;
      }
      else
      {
          // Have to manipulate the parabola to line up with our piecewise function correctly
        float vertOffset = 1.0f;
        Quad::piecewiseEaseOut::Ease(turnScale, vertOffset, change, absX, duration);
      }

      glm::vec3 right = mOrientation->GetRightVector();
      right.y = 0.0f;

        // Can check zero here because we've already passed the dead-zone check
      if (aEvent->StickDirection.x >= 0.0f)
      {
        mTurnVec = turnScale * right;

        if (mAnimator)
        {
          double maxTime = mAnimator->GetMaxTime();
          double stickTurn = (((-turnScale + 1.0) / 2.0) * maxTime);

            // update boat animation : current stick rotation
          mAnimator->SetCurrentAnimTime(stickTurn);
        }

        if (mCharacterAnimator)
        {
          double maxTime = mAnimator->GetMaxTime();
          double stickTurn = (((-turnScale + 1.0) / 2.0) * maxTime);

            // update boat animation : current stick rotation
          mCharacterAnimator->SetCurrentAnimTime(stickTurn);
        }
      }
      else
      {
        mTurnVec = -turnScale * right;

        if (mAnimator)
        {
          double maxTime = mAnimator->GetMaxTime();
          double stickTurn = ((turnScale + 1.0) * maxTime) / 2.0;

          // update boat animation : current stick rotation
          mAnimator->SetCurrentAnimTime(stickTurn);
        }

        if (mCharacterAnimator)
        {
          double maxTime = mAnimator->GetMaxTime();
          double stickTurn = ((turnScale + 1.0) * maxTime) / 2.0;

          // update boat animation : current stick rotation
          mCharacterAnimator->SetCurrentAnimTime(stickTurn);
        }
      }
    }
    else
    {
      mCurrRotSpeed = 0.f;
      mPlayingTurnSound = false;

      if (mAnimator)
      {
        double maxTime = mAnimator->GetMaxTime();
        //double stickTurn = ((turnScale /*aEvent->StickDirection.x + 1.0 */)* maxTime);// / 2.0;
        mAnimator->SetCurrentAnimTime(0.5 * maxTime);
      }

      if (mCharacterAnimator)
      {
        double maxTime = mAnimator->GetMaxTime();
        //double stickTurn = ((turnScale /*aEvent->StickDirection.x + 1.0*/)* maxTime);// / 2.0;
        mCharacterAnimator->SetCurrentAnimTime(0.5 * maxTime);
      }
    }
  }

  void BoatController::Update(LogicUpdate *aEvent)
  {
    // calculate anim key frame
    double sinVal = sin(glm::radians(mTransform->GetWorldRotationAsEuler().y));

    if (mSailsAnimator)
    {
      double sailsFrame = ((sinVal + 1) * mSailsAnimator->GetMaxTime()) / 2.0;

      // update sails animation : world rotation y axis
      mSailsAnimator->SetCurrentAnimTime(sailsFrame);
    }

    if (mMainsailAnimator)
    {
      double mainsailFrame = ((sinVal + 1) * mMainsailAnimator->GetMaxTime()) / 2.0;

      // update mainsail animation : world rotation y axis
      mMainsailAnimator->SetCurrentAnimTime(mainsailFrame);
    }


    if (mStartedTurning)
    {
      if (mCurrRotSpeed < mMaxTurnSpeed)
      {
        mCurrRotSpeed += static_cast<float>(mTurnAccFactor * aEvent->Dt);
      }

      mRigidBody->ApplyForce(mCurrRotSpeed * mTurnVec, mOrientation->GetForwardVector());
      //mRigidBody->ApplyForce(mTurnVec, glm::vec3(0, 1, 0));
    }
    /*else
    {
      if (mTransform->GetRotation().x != 0.0f)
        mRigidBody->ApplyForce(-mTurnVec, glm::vec3(0, 1, 0));
    }*/

    glm::vec3 vel = mRigidBody->GetVelocity();

    mCurrSpeed = glm::sqrt((vel.x * vel.x) + (vel.z * vel.z));

    auto forward = mOrientation->GetForwardVector();
    mRigidBody->SetVelocity(mCurrSpeed * forward.x, vel.y, mCurrSpeed * forward.z);

    if (mIsSailUp)
    {
      glm::vec3 forward = mOrientation->GetForwardVector();
      forward.y = 0.0f;

      mRigidBody->ApplyForce(mWindForce * (forward), glm::vec3(0));

      if (mCurrSpeed > mMaxSailSpeed)
      {
        mRigidBody->ApplyForce(mWindForce * -forward, glm::vec3(0));
      }
    }
    else
    {
      if (mCurrSpeed < 0.1f)
      {
        mRigidBody->SetVelocity(0, vel.y, 0);
        //mRigidBody->SetGravity(glm::vec3(0));
      }
    }

    mStartedTurning = false;


    // update boat rotation
    float rotDiff = mTargetRotationAmount - mCurrentRotationAmount;
    float angle = 3.0f * aEvent->Dt * rotDiff;
    
    mCurrentRotationAmount += angle;

    if (abs(angle) > 0.00001f)
    {
      mTransform->RotateAboutLocalAxis(glm::vec3(0, 0, 1), angle);
    }

    // send boat rotation event for compass needle
    BoatRotation boatRotEvent;
    boatRotEvent.BoatForward = mOrientation->GetForwardVector();
    mSpace->SendEvent(Events::BoatRotation, &boatRotEvent);
  }
  float BoatController::GetCurrentSpeed() const
  {
    return mCurrSpeed;
  }
  ////////////////////////////////////////////////////////////////////////////////////

  void BoatController::StopBoatImmediately()
  {
    mIsSailUp = false;
    mRigidBody->SetVelocity(0.f, mRigidBody->GetVelocity().y, 0.f);
    mRigidBody->SetDamping(1.0f, 1.0f);
  }
}
