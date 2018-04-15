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
  YTEDefineType(StarMovement) { YTERegisterType(StarMovement); }

  StarMovement::StarMovement(Composition *aOwner, Space *aSpace, RSValue *aProperties)
    : Component(aOwner, aSpace)
    , mJohnTransform(nullptr)
    , mDaisyTransform(nullptr)
    , mBasilTransform(nullptr)
    , mStarTransform(nullptr)
    , mNoticeBoardTransform(nullptr)
    , mCurrentAnchor(CurrentAnchor::None)
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
  }

  void StarMovement::Update(LogicUpdate *aEvent)
  {
    if (mStarTransform == nullptr)
    {
      return;
    }

    if (mCurrentAnchor == CurrentAnchor::John)
    {
      if (mJohnTransform)
      {
        glm::vec3 johnPos = mJohnTransform->GetWorldTranslation();
        johnPos += glm::vec3(0, 8, 0);

        mStarTransform->SetWorldTranslation(johnPos);
      }
    }
    else if (mCurrentAnchor == CurrentAnchor::Daisy)
    {
      if (mDaisyTransform)
      {
        glm::vec3 daisyPos = mDaisyTransform->GetWorldTranslation();
        daisyPos += glm::vec3(0, 8, 0);

        mStarTransform->SetWorldTranslation(daisyPos);
      }
    }
    else if (mCurrentAnchor == CurrentAnchor::Basil)
    {
      if (mBasilTransform)
      {
        glm::vec3 basilPos = mBasilTransform->GetWorldTranslation();
        basilPos += glm::vec3(0, 8, 0);

        mStarTransform->SetWorldTranslation(basilPos);
      }
    }
    else if (mCurrentAnchor == CurrentAnchor::NoticeBoard)
    {
      if (mNoticeBoardTransform)
      {
        glm::vec3 nbPos = mNoticeBoardTransform->GetWorldTranslation();
        nbPos += glm::vec3(0, 8, 0);

        mStarTransform->SetWorldTranslation(nbPos);
      }
    }
    
    glm::vec3 rot = mStarTransform->GetRotationAsEuler();
    rot.x += 10.0f * aEvent->Dt;
    mStarTransform->SetRotation(rot);
  }

  void StarMovement::OnQuestStart(QuestStart *aEvent)
  {
    if (aEvent->mCharacter == Quest::CharacterName::John)
    {
      mCurrentAnchor = CurrentAnchor::John;
    }
    else if (aEvent->mCharacter == Quest::CharacterName::Daisy)
    {
      mCurrentAnchor = CurrentAnchor::Daisy;
    }
    else if (aEvent->mCharacter == Quest::CharacterName::Basil)
    {
      mCurrentAnchor = CurrentAnchor::Basil;
    }
    
  }

  void StarMovement::OnUpdateActiveQuestState(UpdateActiveQuestState *aEvent)
  {
    if (aEvent->mState == Quest::State::Completed)
    {
      mCurrentAnchor = CurrentAnchor::NoticeBoard;
    }
  }

} //end yte