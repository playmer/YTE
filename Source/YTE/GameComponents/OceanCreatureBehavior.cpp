/******************************************************************************/
/*!
\file   OceanCreatureBehavior.cpp
\author Jonathan Ackerman
\par    email: jonathan.ackerman\@digipen.edu
\date   2018-02-27

All content (c) 2016 DigiPen  (USA) Corporation, all rights reserved.
*/
/******************************************************************************/

#include "YTE/Core/Engine.hpp"
#include "YTE/GameComponents/OceanCreatureBehavior.hpp"
#include "YTE/WWise/WWiseEmitter.hpp"

namespace YTE
{
  YTEDefineType(OceanCreatureBehavior) 
  { 
    RegisterType<OceanCreatureBehavior>();
    TypeBuilder<OceanCreatureBehavior> builder;
  
    builder.Property<&OceanCreatureBehavior::GetFlipRotation, &OceanCreatureBehavior::SetFlipRotation>( "Flip Rotation")
      .AddAttribute<EditorProperty>()
      .AddAttribute<Serializable>();

    builder.Property<&OceanCreatureBehavior::GetJumpDistance, &OceanCreatureBehavior::SetJumpDistance>( "Jump Distance")
      .AddAttribute<EditorProperty>()
      .AddAttribute<Serializable>();

    builder.Property<&OceanCreatureBehavior::GetSleepTime, &OceanCreatureBehavior::SetSleepTime>( "Sleep Time")
      .AddAttribute<EditorProperty>()
      .AddAttribute<Serializable>();

    builder.Property<&OceanCreatureBehavior::GetRotateSpeed, &OceanCreatureBehavior::SetRotateSpeed>( "Rotate Speed")
      .AddAttribute<EditorProperty>()
      .AddAttribute<Serializable>();
  }

  OceanCreatureBehavior::OceanCreatureBehavior(Composition *aOwner, Space *aSpace, RSValue *aProperties)
    : Component(aOwner, aSpace)
    , mBoatTransform(nullptr)
    , mParentTransform(nullptr)
    , mStartJump(false)
    , mTimer(0.0)
    , mFlipRotation(false)
    , mJumpDistance(20.0f)
    , mSleepTime(30.0)
    , mState(State::Waiting)
    , mRotateSpeed(4.0f)
  {
    DeserializeByType(aProperties, this, GetStaticType());
  }

  void OceanCreatureBehavior::Initialize()
  {
    mSpace->RegisterEvent<&OceanCreatureBehavior::Update>(Events::LogicUpdate, this);

    if (Composition *camera = GetSpace()->FindFirstCompositionByName("Camera"))
    {
      mCameraTransform = camera->GetComponent<Transform>();
    }

    if (Composition *boat = mSpace->FindFirstCompositionByName("Boat"))
    {
      mBoatTransform = boat->GetComponent<Transform>();
    }

    mParentTransform = mOwner->GetParent()->GetComponent<Transform>();
  }

  void OceanCreatureBehavior::Update(LogicUpdate *aEvent)
  {
    if (mBoatTransform == nullptr || mParentTransform == nullptr)
    {
      return;
    }

    glm::vec3 pos = mParentTransform->GetWorldTranslation();
    glm::vec3 boatPos = mBoatTransform->GetWorldTranslation();

    glm::vec3 diff = pos - boatPos;

    float dist = glm::length(diff);

    if (dist > mJumpDistance)
    {
      mStartJump = true;
    }
    else
    {
      mStartJump = false;
    }

    switch (mState)
    {
      case State::Waiting:
      {
        if (mStartJump)
        {
          // set initial rotation
          mParentTransform->SetWorldRotation(glm::vec3(0));

          auto emitter = mOwner->GetComponent<WWiseEmitter>();
          if (emitter)
          {
            emitter->PlayEvent("SFX_Ani_Jump");
          }

          mState = State::Jumping;
        }

        break;
      }

      case State::Jumping:
      {
        mParentTransform->SetWorldRotation(mCameraTransform->GetWorldRotation());

        float rotAngle = mRotateSpeed * static_cast<float>(aEvent->Dt);

        if (mFlipRotation)
        {
          rotAngle = -rotAngle;
        }

        mTimer += rotAngle;

        // update rotation
        mParentTransform->RotateAboutLocalAxis(glm::vec3(0, 0, 1), static_cast<float>(mTimer));

        if (abs(mTimer) > 6.0)
        {
          mTimer = 0.0;
          mState = State::Sleeping;
        }
        break;
      }

      case State::Sleeping:
      {
        mTimer += aEvent->Dt;

        if (mTimer > mSleepTime)
        {
          mTimer = 0.0;
          mState = State::Waiting;
        }
        break;
      }
    }
  }

  bool OceanCreatureBehavior::GetFlipRotation() const
  {
    return mFlipRotation;
  }

  void OceanCreatureBehavior::SetFlipRotation(bool aFlip)
  {
    mFlipRotation = aFlip;
  }

  float OceanCreatureBehavior::GetJumpDistance() const
  {
    return mJumpDistance;
  }

  void OceanCreatureBehavior::SetJumpDistance(float aDistance)
  {
    mJumpDistance = aDistance;
  }

  float OceanCreatureBehavior::GetSleepTime() const
  {
    return static_cast<float>(mSleepTime);
  }

  void OceanCreatureBehavior::SetSleepTime(float aTime)
  {
    mSleepTime = aTime;
  }

  float OceanCreatureBehavior::GetRotateSpeed() const
  {
    return mRotateSpeed;
  }

  void OceanCreatureBehavior::SetRotateSpeed(float aSpeed)
  {
    mRotateSpeed = aSpeed;
  }

} //end yte