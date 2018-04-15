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

namespace YTE
{
  YTEDefineType(OceanCreatureBehavior) 
  { 
    YTERegisterType(OceanCreatureBehavior); 
  
    YTEBindProperty(&OceanCreatureBehavior::GetFlipRotation, &OceanCreatureBehavior::SetFlipRotation, "Flip Rotation")
      .AddAttribute<EditorProperty>()
      .AddAttribute<Serializable>();

    YTEBindProperty(&OceanCreatureBehavior::GetJumpDistance, &OceanCreatureBehavior::SetJumpDistance, "Jump Distance")
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
    , mSleepTimer(0.0)
    , mSleepTimerMax(30.0)
    , mState(State::Waiting)
  {
    DeserializeByType(aProperties, this, GetStaticType());
  }

  void OceanCreatureBehavior::Initialize()
  {
    mSpace->YTERegister(Events::LogicUpdate, this, &OceanCreatureBehavior::Update);

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

    float dist = diff.length();

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
          
          mState = State::Jumping;
        }

        break;
      }

      case State::Jumping:
      {
        mParentTransform->SetWorldRotation(mCameraTransform->GetWorldRotation());

        float rotAngle = 2.0f * aEvent->Dt;

        if (mFlipRotation)
        {
          rotAngle = -rotAngle;
        }

        mTimer += rotAngle;

        // update rotation
        mParentTransform->RotateAboutLocalAxis(glm::vec3(0, 0, 1), mTimer);

        if (mTimer > 2.0)
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
    return 0.0f;
  }

  void OceanCreatureBehavior::SetJumpDistance(float aDistance)
  {
  }

} //end yte