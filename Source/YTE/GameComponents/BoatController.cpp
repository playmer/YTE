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
#include "YTE/Core/Actions/ActionManager.hpp"

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
    RegisterType<BoatController>();
    TypeBuilder<BoatController> builder;


    builder.Property<&BoatController::GetMaxSailSpeed, &BoatController::SetMaxSailSpeed>( "MaxSailSpeed")
      .AddAttribute<Serializable>()
      .AddAttribute<EditorProperty>();

    builder.Property<&BoatController::GetRotationSpeed, &BoatController::SetRotationSpeed>( "MaxTurnSpeed")
      .AddAttribute<Serializable>()
      .AddAttribute<EditorProperty>();

    builder.Property<&BoatController::GetTurnAccFactor, &BoatController::SetTurnAccFactor>( "TurnAccelerationFactor")
      .AddAttribute<Serializable>()
      .AddAttribute<EditorProperty>();

    builder.Property<&BoatController::GetWindForce, &BoatController::SetWindForce>( "WindForce")
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
    , mPlayingTurnSound(false)
    , mCurrSpeed(0.f)
    , mCurrRotSpeed(0.f)
    , mAnimator(nullptr)
    , mSailsAnimator(nullptr)
    , mMainsailAnimator(nullptr)
    , mMastAnimator(nullptr)
    , mTargetRotationAmount(0.0f)
    , mCurrentRotationAmount(0.0f)
    , mSailScale(0)
    , mSailFinished(true)
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
    mSoundSystem = mSpace->GetEngine()->GetComponent<WWiseSystem>();

    if (auto backLeft = mOwner->FindFirstCompositionByName("BackEmitterLeft"))
    {
      mBackLeftEmitterTransform = backLeft->GetComponent<Transform>();
    }

    if (auto backRight = mOwner->FindFirstCompositionByName("BackEmitterRight"))
    {
      mBackRightEmitterTransform = backRight->GetComponent<Transform>();
    }

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
    mSpace->RegisterEvent<&BoatController::ChangeSail>(Events::SailStateChanged, this);
    mSpace->RegisterEvent<&BoatController::TurnBoat>(Events::BoatTurnEvent, this);
    mSpace->RegisterEvent<&BoatController::DockBoat>(Events::BoatDockEvent, this);
    mSpace->RegisterEvent<&BoatController::OnDialogueExit>(Events::DialogueExit, this);
    mSpace->RegisterEvent<&BoatController::Update>(Events::LogicUpdate, this);

    mAnimator = mOwner->GetComponent<Animator>();
    
    if (Composition *sails = mOwner->FindFirstCompositionByName("Sails"))
    {
      mSailsAnimator = sails->GetComponent<Animator>();
    }
    
    if (Composition *mainsail = mOwner->FindFirstCompositionByName("Mainsail"))
    {
      mMainsailAnimator = mainsail->GetComponent<Animator>();
    }

    if (Composition *mast = mOwner->FindFirstCompositionByName("Mast"))
    {
      mMastAnimator = mast->GetComponent<Animator>();
    }

    if (Composition *character = mOwner->FindFirstCompositionByName("MainCharacter"))
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

    if (mMastAnimator)
    {
      mMastAnimator->SetDefaultAnimation("Boat_Turn.fbx");
      mMastAnimator->SetCurrentAnimation("Boat_Turn.fbx");
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

    mCharacterAnimator->SetDefaultAnimation("Idle.fbx");
    mCharacterAnimator->SetCurrentAnimation("Idle.fbx");
  }

  void BoatController::ChangeSail(SailStateChanged *aEvent)
  {
    auto actionManager = mSpace->GetComponent<ActionManager>();

    if (aEvent->SailUp)
    {
      if (!mIsSailUp)
      {
        mSailFinished = false;
        ActionSequence raiseSail;
        raiseSail.Add<Quad::easeOut>(mSailScale, 1.0f, 0.5f);
        raiseSail.Call([this]() {
          mSailFinished = true;
          mOwner->FindFirstCompositionByName("Mainsail")->GetComponent<Transform>()->SetScale(glm::vec3(1));
          mOwner->FindFirstCompositionByName("Sails")->GetComponent<Transform>()->SetScale(glm::vec3(1));
        });
        actionManager->AddSequence(mOwner, raiseSail);


        mSoundEmitter->PlayEvent(mSoundSailUp);
        mRigidBody->SetDamping(0.f, 0.9f);
      }
    }
    else
    {
      if (mIsSailUp)
      {
        mSailFinished = false;
        ActionSequence lowerSail;
        lowerSail.Add<Quad::easeOut>(mSailScale, 0.0f, 0.5f);
        lowerSail.Call([this]() {
          mSailFinished = true;
          mOwner->FindFirstCompositionByName("Mainsail")->GetComponent<Transform>()->SetScale(glm::vec3(0));
          mOwner->FindFirstCompositionByName("Sails")->GetComponent<Transform>()->SetScale(glm::vec3(0));
        });
        actionManager->AddSequence(mOwner, lowerSail);


        mSoundEmitter->PlayEvent(mSoundSailDown);
        mRigidBody->SetDamping(0.9f, 0.9f);
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

    mTargetRotationAmount = 0.35f * aEvent->StickDirection.x;

      // Dead-zone check and apply response curves
    float length = glm::length(aEvent->StickDirection);

    double stickTurn = 0.0f;
    double maxTime = mAnimator->GetMaxTime();

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
      right = glm::normalize(right);

        // Can check zero here because we've already passed the dead-zone check
      if (aEvent->StickDirection.x >= 0.0f)
      {
        mTurnVec = turnScale * right;
        
        stickTurn = (((-turnScale + 1.0) / 2.0) * maxTime);

        if (mAnimator)
        {
          // update boat animation : current stick rotation
          mAnimator->SetCurrentAnimTime(stickTurn);
        }

        if (mCharacterAnimator)
        {
            // update boat animation : current stick rotation
          mCharacterAnimator->SetCurrentAnimTime(stickTurn);
        }
      }
      else
      {
        mTurnVec = -turnScale * right;
        
        double maxTime = mAnimator->GetMaxTime();
        stickTurn = ((turnScale + 1.0) * maxTime) / 2.0;

        if (mAnimator)
        {
          // update boat animation : current stick rotation
          mAnimator->SetCurrentAnimTime(stickTurn);
        }

        if (mCharacterAnimator)
        {
          // update boat animation : current stick rotation
          mCharacterAnimator->SetCurrentAnimTime(stickTurn);
        }
      }
    }
    else
    {
      mCurrRotSpeed = 0.f;
      mPlayingTurnSound = false;

      stickTurn = 0.5 * maxTime;

      if (mAnimator)
      {
        mAnimator->SetCurrentAnimTime(stickTurn);
      }

      if (mCharacterAnimator)
      {
        mCharacterAnimator->SetCurrentAnimTime(stickTurn);
      }
    }

    glm::vec3 rightVec = mOrientation->GetRightVector();

    glm::vec3 tillerPos = -4.5f * mOrientation->GetForwardVector();

    float emitterTurnOffset = static_cast<float>(0.1f * (stickTurn - 0.5 * maxTime));
    
    glm::vec3 emitterOffset = emitterTurnOffset * rightVec;

    // update boat trail particle emitter positions
    if (mBackLeftEmitterTransform)
    {
      glm::vec3 leftEmitPos = tillerPos - emitterOffset - 0.1f * rightVec;
      mBackLeftEmitterTransform->SetTranslation(leftEmitPos.x, leftEmitPos.y + 0.2966f, leftEmitPos.z);
    }

    if (mBackRightEmitterTransform)
    {
      glm::vec3 rightEmitPos = tillerPos - emitterOffset + 0.1f * rightVec;
      mBackRightEmitterTransform->SetTranslation(rightEmitPos.x, rightEmitPos.y + 0.2966f, rightEmitPos.z);
    }
  }

  void BoatController::OnDialogueExit(DialogueExit *)
  {
    mCharacterAnimator->SetDefaultAnimation("MC_Boat_Turn.fbx");
    mCharacterAnimator->SetCurrentAnimation("MC_Boat_Turn.fbx");

    mRigidBody->SetDamping(0.f, 0.9f);
  }

  void BoatController::Update(LogicUpdate *aEvent)
  {
    auto pos = mTransform->GetWorldTranslation();

    if (!mSailFinished)
    {
      mOwner->FindFirstCompositionByName("Mainsail")->GetComponent<Transform>()->SetScale(glm::vec3(mSailScale, 1, mSailScale));
      mOwner->FindFirstCompositionByName("Sails")->GetComponent<Transform>()->SetScale(glm::vec3(mSailScale, 1, mSailScale));
    }

    if (pos.y < 0.35f || pos.y > 0.45f)
    {
      mTransform->SetWorldTranslation(pos.x, 0.4f, pos.z);
      glm::vec3 vel = mRigidBody->GetVelocity();
      mRigidBody->SetVelocity(vel.x, 0.0f, vel.z);

      auto rot = mTransform->GetWorldRotationAsEuler();
      mTransform->SetWorldRotation(0.0f, rot.y, 0.0f);
    }

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
    
    if (mMastAnimator)
    {
      double mastFrame = ((sinVal + 1) * mMastAnimator->GetMaxTime()) / 2.0;

      // update mast animation : world rotation y axis
      mMastAnimator->SetCurrentAnimTime(mastFrame);
    }

    // update boat rotation
    float rotDiff = mTargetRotationAmount - mCurrentRotationAmount;
    float angle = 3.0f * static_cast<float>(aEvent->Dt) * rotDiff;
    
    mCurrentRotationAmount += angle;
    
    if (abs(angle) > 0.00001f)
    {
      //mTransform->RotateAboutLocalAxis(glm::vec3(0, 0, 1), angle);
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

    auto forward = mOrientation->GetForwardVector();

    //glm::vec3 speedVec = (glm::dot(vel, forward) / glm::dot(forward, forward)) * forward;
    //mCurrSpeed = glm::sqrt((speedVec.x * speedVec.x) + (speedVec.z * speedVec.z));

    mCurrSpeed = glm::sqrt((vel.x * vel.x) + (vel.z * vel.z));


    mRigidBody->SetVelocity(mCurrSpeed * forward.x, vel.y, mCurrSpeed * forward.z);

    if (mIsSailUp)
    {
      glm::vec3 tempForward = mOrientation->GetForwardVector();
      tempForward.y = 0.0f;

      mRigidBody->ApplyForce(mWindForce * (tempForward), glm::vec3(0));

      if (mCurrSpeed > mMaxSailSpeed)
      {
        mRigidBody->ApplyForce(mWindForce * -tempForward, glm::vec3(0));
      }
    }
    else
    {
      if (mCurrSpeed < 0.1f)
      {
        mRigidBody->SetVelocity(0, 0, 0);
        //mRigidBody->SetGravity(glm::vec3(0));
      }
    }

    mStartedTurning = false;

    float ratio = 100.0f / mMaxSailSpeed;
    mSoundSystem->SetRTPC("Boat_Velocity", ratio * mCurrSpeed);

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
