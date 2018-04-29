/******************************************************************************/
/*!
\file   StarMovement.cpp
\author Jonathan Ackerman
\par    email: jonathan.ackerman\@digipen.edu
\date   2018-02-27

All content (c) 2016 DigiPen  (USA) Corporation, all rights reserved.
*/
/******************************************************************************/

#include "YTE/GameComponents/StarMovement.hpp"
#include "YTE/GameComponents/DialogueDirector.hpp"
#include "YTE/GameComponents/NoticeBoard.hpp"
#include "YTE/Graphics/Animation.hpp"

namespace YTE
{
  YTEDefineType(StarMovement) { RegisterType<StarMovement>(); }

  StarMovement::StarMovement(Composition *aOwner, Space *aSpace, RSValue *aProperties)
    : Component(aOwner, aSpace)
    , mJohnTransform(nullptr)
    , mDaisyTransform(nullptr)
    , mBasilTransform(nullptr)
    , mStarTransform(nullptr)
    , mNoticeBoardTransform(nullptr)
    , mCurrentAnchor(CurrentAnchor::None)
    , mTimer(0.0)
    , mLightBeamTransform(nullptr)
    , mBoatTransform(nullptr)
  {
    YTEUnusedArgument(aProperties);
  }

  void StarMovement::Initialize()
  {
    mSpace->YTERegister(Events::LogicUpdate, this, &StarMovement::Update);

    mSpace->YTERegister(Events::QuestStart, this, &StarMovement::OnQuestStart);
    mSpace->YTERegister(Events::UpdateActiveQuestState, this, &StarMovement::OnUpdateActiveQuestState);

    if (Composition *john = mSpace->FindFirstCompositionByName("john"))
    {
      mJohnTransform = john->GetComponent<Transform>();
    }

    if (Composition *daisy = mSpace->FindFirstCompositionByName("daisy"))
    {
      mDaisyTransform = daisy->GetComponent<Transform>();
    }

    if (Composition *basil = mSpace->FindFirstCompositionByName("basil"))
    {
      mBasilTransform = basil->GetComponent<Transform>();
    }

    if (Composition *star = mSpace->FindFirstCompositionByName("FeedbackStar"))
    {
      mStarTransform = star->GetComponent<Transform>();
    }
    
    if (Composition *noticeBoard = mSpace->FindFirstCompositionByName("noticeboard"))
    {
      mNoticeBoardTransform = noticeBoard->GetComponent<Transform>();
    }

    if (Composition *lightBeam = mOwner->FindFirstCompositionByName("Cylinder"))
    {
      mLightBeamTransform = lightBeam->GetComponent<Transform>();
    }

    if (Composition *boat = mSpace->FindFirstCompositionByName("Boat"))
    {
      mBoatTransform = boat->GetComponent<Transform>();
    }
  }

  void StarMovement::Update(LogicUpdate *aEvent)
  {
    if (mStarTransform == nullptr)
    {
      return;
    }

    float rotAngle = 3.0f * static_cast<float>(aEvent->Dt);
    mStarTransform->RotateAboutLocalAxis(glm::vec3(0, 1, 0), rotAngle);

    mTimer += 4.0 * aEvent->Dt;

    float yOffset = 0.75f * static_cast<float>(sin(mTimer));
    glm::vec3 yOffsetVector = glm::vec3(0, 10 + yOffset, 0);

    if (mCurrentAnchor == CurrentAnchor::John)
    {
      if (mJohnTransform)
      {
        glm::vec3 johnPos = mJohnTransform->GetWorldTranslation();
        johnPos += yOffsetVector;

        mStarTransform->SetWorldTranslation(johnPos);
      }
    }
    else if (mCurrentAnchor == CurrentAnchor::Daisy)
    {
      if (mDaisyTransform)
      {
        glm::vec3 daisyPos = mDaisyTransform->GetWorldTranslation();
        daisyPos += yOffsetVector;

        mStarTransform->SetWorldTranslation(daisyPos);
      }
    }
    else if (mCurrentAnchor == CurrentAnchor::Basil)
    {
      if (mBasilTransform)
      {
        glm::vec3 basilPos = mBasilTransform->GetWorldTranslation();
        basilPos += yOffsetVector;

        mStarTransform->SetWorldTranslation(basilPos);
      }
    }
    else if (mCurrentAnchor == CurrentAnchor::NoticeBoard)
    {
      if (mNoticeBoardTransform)
      {
        glm::vec3 nbPos = mNoticeBoardTransform->GetWorldTranslation();
        nbPos += yOffsetVector;

        mStarTransform->SetWorldTranslation(nbPos);
      }
    }

    glm::vec3 boatPos = mBoatTransform->GetWorldTranslation();
    glm::vec3 pos = mStarTransform->GetWorldTranslation();

    glm::vec3 dist = boatPos - pos;

    float distF = glm::length(dist);
    
    float scale = (distF - 80.0f) / 200.0f;

    if (scale > 5.0f)
    {
      scale = 5.0f;
    }
    else if (scale < 0.05f)
    {
      scale = 0.0f;
      mLightBeamTransform->SetScale(0.0f, 0.0f, 0.0f);
    }
    else
    {
      mLightBeamTransform->SetScale(scale, 500.0f, scale);
    }
  }

  void StarMovement::SetActive(glm::vec3 aPos)
  {
    mStarTransform->SetWorldTranslation(aPos);
    mLightBeamTransform->SetScale(1.0f, 500.0f, 1.0f);
    mCurrentAnchor = CurrentAnchor::None;
  }

  void StarMovement::SetAnchor(CurrentAnchor aAnchor)
  {
    mCurrentAnchor = aAnchor;
  }

  void StarMovement::OnQuestStart(QuestStart *aEvent)
  {
    if (aEvent->mCharacter == Quest::CharacterName::John)
    {
      mLightBeamTransform->SetScale(1.0f, 500.0f, 1.0f);
      mCurrentAnchor = CurrentAnchor::John;
    }
    else if (aEvent->mCharacter == Quest::CharacterName::Daisy)
    {
      mLightBeamTransform->SetScale(1.0f, 500.0f, 1.0f);
      mCurrentAnchor = CurrentAnchor::Daisy;
    }
    else if (aEvent->mCharacter == Quest::CharacterName::Basil)
    {
      mLightBeamTransform->SetScale(1.0f, 500.0f, 1.0f);
      mCurrentAnchor = CurrentAnchor::Basil;
    }
  }

  void StarMovement::OnUpdateActiveQuestState(UpdateActiveQuestState *aEvent)
  {
    if (aEvent->mState == Quest::State::Completed)
    {
      mLightBeamTransform->SetScale(1.0f, 500.0f, 1.0f);
      mCurrentAnchor = CurrentAnchor::NoticeBoard;
    }
  }

} //end yte