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
  }

  OceanCreatureBehavior::OceanCreatureBehavior(Composition *aOwner, Space *aSpace, RSValue *aProperties)
    : Component(aOwner, aSpace)
    , mBoatTransform(nullptr)
    , mParentTransform(nullptr)
    , mStartJump(false)
    , mTimer(0.0)
    , mFlipRotation(false)
  {
    DeserializeByType(aProperties, this, GetStaticType());
  }

  void OceanCreatureBehavior::Initialize()
  {
    mSpace->YTERegister(Events::LogicUpdate, this, &OceanCreatureBehavior::Update);

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

    if (dist > 1)
    {
      mStartJump = true;
    }
    else
    {
      mStartJump = false;
    }

    if (mStartJump && !mIsJumping)
    {
      mIsJumping = true;

      // set initial rotation
      mParentTransform->SetWorldRotation(glm::vec3(0));
    }

    if (mIsJumping)
    {
      float rotAngle = 2.0f * aEvent->Dt;

      if (mFlipRotation)
      {
        rotAngle = -rotAngle;
      }

      // update rotation
      mParentTransform->RotateAboutLocalAxis(glm::vec3(0, 0, 1), rotAngle);
      
      mTimer += aEvent->Dt;
    }

    if (mTimer > 2.0)
    {
      mIsJumping = false;
      mTimer = 0.0;
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

} //end yte