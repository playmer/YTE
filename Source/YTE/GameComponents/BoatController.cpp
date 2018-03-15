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
  YTEDefineEvent(RequestDialogueStart);

  YTEDefineType(RequestDialogueStart) { YTERegisterType(RequestDialogueStart); }

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
    , mPlayingTurnSound(false)
    , mCurrSpeed(0.f)
    , mCurrRotSpeed(0.f)
    , mAnimator(nullptr)
    , mSailsAnimator(nullptr)
    , mMainsailAnimator(nullptr)
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
    mStartedTurning = false;
    mSoundEmitter = mOwner->GetComponent<WWiseEmitter>();

    mRigidBody->SetDamping(0.9f, 0.9f);

      // Cache ids for all sounds used by this component
    auto soundSystem = mSpace->GetEngine()->GetComponent<WWiseSystem>();

    if (soundSystem)
    {
      soundSystem->GetSoundIDFromString("SFX_Sail_Up", mSoundSailUp);
      soundSystem->GetSoundIDFromString("SFX_Sail_Down", mSoundSailDown);
      soundSystem->GetSoundIDFromString("SFX_Boat_Bump", mSoundBumpDock);
      soundSystem->GetSoundIDFromString("SFX_Boat_Turn", mSoundBoatTurn);
    }

    /* Event Registration */
    mSpace->YTERegister(Events::SailStateChanged, this, &BoatController::ChangeSail);
    mSpace->YTERegister(Events::BoatTurnEvent, this, &BoatController::TurnBoat);
    mSpace->YTERegister(Events::BoatDockEvent, this, &BoatController::DockBoat);
    mSpace->YTERegister(Events::LogicUpdate, this, &BoatController::Update);
    mOwner->YTERegister(Events::CollisionStarted, this, &BoatController::OnCollisionStart);
    mOwner->YTERegister(Events::CollisionEnded, this, &BoatController::OnCollisionEnd);

    mAnimator = mOwner->GetComponent<Animator>();
    
    if (Composition *sails = mOwner->FindFirstCompositionByName("Sails"))
    {
      mSailsAnimator = sails->GetComponent<Animator>();
    }
    
    if (Composition *mainsail = mOwner->FindFirstCompositionByName("Mainsail"))
    {
      mMainsailAnimator = mainsail->GetComponent<Animator>();
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
      InputInterpreter *input = mSpace->GetComponent<InputInterpreter>();

      if (input)
      {
        input->SetInputContext(InputInterpreter::InputContext::Dialogue);
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
          mNearbyDock->SendEvent(Events::RequestDialogueStart, &dStart);
          dStart.camera->SendEvent(Events::RequestDialogueStart, &dStart);
        }

        // play the docking sound
        if (mSoundEmitter)
        {
          //mSoundEmitter->PlayEvent(mSoundBumpDock);
        }
      }
    }
  }

  void BoatController::ChangeSail(SailStateChanged *aEvent)
  {
    if (aEvent->SailUp)
    {
      if (!mIsSailUp)
      {
        //mSoundEmitter->PlayEvent(mSoundSailUp);
        mRigidBody->SetDamping(0.f, 0.9f);

        mOwner->FindFirstCompositionByName("Mainsail")->GetComponent<Transform>()->SetScale(glm::vec3(1.0f));
        mOwner->FindFirstCompositionByName("Sails")->GetComponent<Transform>()->SetScale(glm::vec3(1.0f));
      }
    }
    else
    {
      if (mIsSailUp)
      {
        //mSoundEmitter->PlayEvent(mSoundSailDown);
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
      //mSoundEmitter->PlayEvent(mSoundBoatTurn);
      mPlayingTurnSound = true;
    }

    if (aEvent->StickDirection.x > 0.1)
    {
      mTurnVec = mOrientation->GetRightVector();
    }
    else if (aEvent->StickDirection.x < -0.1)
    {
      mTurnVec = -mOrientation->GetRightVector();
    }
    else
    {
      mCurrRotSpeed = 0.f;
      mPlayingTurnSound = false;
    }


    if (mAnimator)
    {
      double maxTime = mAnimator->GetMaxTime();
      double stickTurn = ((aEvent->StickDirection.x + 1.0) * maxTime) / 2.0;
      
      // update boat animation : current stick rotation
      mAnimator->SetCurrentAnimTime(stickTurn);
    }

    // update character animation : current stick rotation


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
      mRigidBody->ApplyForce(mWindForce * (mOrientation->GetForwardVector()), glm::vec3(0));

      if (mCurrSpeed > mMaxSailSpeed)
      {
        mRigidBody->ApplyForce(mWindForce * -mOrientation->GetForwardVector(), glm::vec3(0));
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
}
