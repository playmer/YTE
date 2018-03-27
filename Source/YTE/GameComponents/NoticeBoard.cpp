/******************************************************************************/
/*!
\file   NoticeBoard.cpp
\author Jonathan Ackerman
\par    email: jonathan.ackerman\@digipen.edu
\date   2018-03-25

All content (c) 2016 DigiPen  (USA) Corporation, all rights reserved.
*/
/******************************************************************************/

#include "YTE/GameComponents/NoticeBoard.hpp"
#include "YTE/GameComponents/QuestLogic.hpp" /* to identify collision with boat */

namespace YTE
{
  YTEDefineType(NoticeBoard) { YTERegisterType(NoticeBoard); }

  NoticeBoard::NoticeBoard(Composition *aOwner, Space *aSpace, RSValue *aProperties)
    : Component(aOwner, aSpace)
  {
    YTEUnusedArgument(aProperties);

      // this is what determines the player's quest order
    mPostcardVec.emplace_back(Quest::CharacterName::John, Quest::Name::Introduction);
    mPostcardVec.emplace_back(Quest::CharacterName::John, Quest::Name::Fetch);

    mPostcardVec.emplace_back(Quest::CharacterName::Daisy, Quest::Name::Introduction);
    mPostcardVec.emplace_back(Quest::CharacterName::Daisy, Quest::Name::Explore);

    mPostcardVec.emplace_back(Quest::CharacterName::Basil, Quest::Name::Introduction);
    mPostcardVec.emplace_back(Quest::CharacterName::Basil, Quest::Name::Fetch);

    mPostcardVec.emplace_back(Quest::CharacterName::John, Quest::Name::Dialogue);
    mPostcardVec.emplace_back(Quest::CharacterName::Basil, Quest::Name::Explore);
    mPostcardVec.emplace_back(Quest::CharacterName::Daisy, Quest::Name::Dialogue);
    mPostcardVec.emplace_back(Quest::CharacterName::John, Quest::Name::Explore);
    mPostcardVec.emplace_back(Quest::CharacterName::Daisy, Quest::Name::Fetch);
    mPostcardVec.emplace_back(Quest::CharacterName::Basil, Quest::Name::Dialogue);
  }

  void NoticeBoard::Initialize()
  {
    mOwner->YTERegister(Events::CollisionStarted, this, &NoticeBoard::OnCollisionStarted);
    mOwner->YTERegister(Events::CollisionEnded, this, &NoticeBoard::OnCollisionEnded);
    mSpace->YTERegister(Events::NoticeBoardHookup, this, &NoticeBoard::OnNoticeBoardHookup);
    mAssignedPostcard = nullptr;
  }

  void NoticeBoard::OnCollisionStarted(CollisionStarted *aEvent)
  {
    if (aEvent->OtherObject->GetComponent<QuestLogic>() != nullptr)
    {
      mSpace->YTERegister(Events::RequestNoticeBoardStart, this, &NoticeBoard::OnRequestNoticeBoardStart);
    }
  }

  void NoticeBoard::OnCollisionEnded(CollisionEnded *aEvent)
  {
    if (aEvent->OtherObject->GetComponent<QuestLogic>() != nullptr)
    {
      mSpace->YTEDeregister(Events::RequestNoticeBoardStart, this, &NoticeBoard::OnRequestNoticeBoardStart);
    }
  }

  void NoticeBoard::OnNoticeBoardHookup(NoticeBoardHookup *aEvent)
  {
    Quest **handle = aEvent->mActiveQuestHandle;
    Quest::CharacterName character = (*handle)->GetCharacter();
    switch (character)
    {
      case Quest::CharacterName::John:
      {
        mActiveQuestMap.emplace(std::make_pair(Quest::CharacterName::John, aEvent->mActiveQuestHandle));
      }
      case Quest::CharacterName::Daisy:
      {
        mActiveQuestMap.emplace(std::make_pair(Quest::CharacterName::Daisy, aEvent->mActiveQuestHandle));
      }
      case Quest::CharacterName::Basil:
      {
        mActiveQuestMap.emplace(std::make_pair(Quest::CharacterName::Basil, aEvent->mActiveQuestHandle));
      }
    }
  }

  void NoticeBoard::OnRequestNoticeBoardStart(RequestNoticeBoardStart *aEvent)
  {
    if (mAssignedPostcard != nullptr)
    {
      Quest *curQuest = *(mActiveQuestMap.at(mAssignedPostcard->GetCharacter()));
      Quest::State curState = curQuest->GetState();

      if (curState == Quest::State::Completed)
      {
        // assign next postcard
        ++mAssignedPostcard;
        if (mAssignedPostcard != mPostcardVec.end()._Ptr)
        {
          QuestStart quest(mAssignedPostcard->GetCharacter(), mAssignedPostcard->GetQuest());
          mSpace->SendEvent(Events::QuestStart, &quest);
        }
      }
    }
    else
    {
      // assign first postcard
      mAssignedPostcard = &mPostcardVec[0];
      QuestStart quest(mAssignedPostcard->GetCharacter(), mAssignedPostcard->GetQuest());
      mSpace->SendEvent(Events::QuestStart, &quest);
    }
  }
}//end yte