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

namespace YTE
{
  YTEDefineType(NoticeBoard) { YTERegisterType(NoticeBoard); }

  NoticeBoard::NoticeBoard(Composition *aOwner, Space *aSpace, RSValue *aProperties)
    : Component(aOwner, aSpace)
  {
    YTEUnusedArgument(aProperties);

    // Create all the postcards, by character then quest
    mPostcardVec.emplace_back(Quest::CharacterName::John, Quest::Name::Introduction);
    mPostcardVec.emplace_back(Quest::CharacterName::Daisy, Quest::Name::Introduction);
    mPostcardVec.emplace_back(Quest::CharacterName::Basil, Quest::Name::Introduction);

    mPostcardVec.emplace_back(Quest::CharacterName::John, Quest::Name::Explore);
    mPostcardVec.emplace_back(Quest::CharacterName::Daisy, Quest::Name::Explore);
    mPostcardVec.emplace_back(Quest::CharacterName::Basil, Quest::Name::Explore);

    mPostcardVec.emplace_back(Quest::CharacterName::John, Quest::Name::Fetch);
    mPostcardVec.emplace_back(Quest::CharacterName::Daisy, Quest::Name::Fetch);
    mPostcardVec.emplace_back(Quest::CharacterName::Basil, Quest::Name::Fetch);

    mPostcardVec.emplace_back(Quest::CharacterName::John, Quest::Name::Dialogue);
    mPostcardVec.emplace_back(Quest::CharacterName::Daisy, Quest::Name::Dialogue);
    mPostcardVec.emplace_back(Quest::CharacterName::Basil, Quest::Name::Dialogue);
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

  void NoticeBoard::Initialize()
  {
    mOwner->YTERegister(Events::CollisionStarted, this, &NoticeBoard::OnCollisionStarted);
    mOwner->YTERegister(Events::CollisionEnded, this, &NoticeBoard::OnCollisionEnded);
    mSpace->YTERegister(Events::ActiveQuestBroadcast, this, &NoticeBoard::OnActiveQuestBroadcast);
    mAssignedPostcard = nullptr;
  }

  void NoticeBoard::OnActiveQuestBroadcast(ActiveQuestBroadcast *aEvent)
  {
    Quest::CharacterName character = aEvent->mActiveQuest->GetCharacter();
    if (mActiveQuestMap[character] != nullptr)
    {
      mActiveQuestMap[character] = aEvent->mActiveQuest;
    }
    else
    {
      switch (character)
      {
        case Quest::CharacterName::John:
        {
          mActiveQuestMap.emplace(std::make_pair(Quest::CharacterName::John, aEvent->mActiveQuest));
        }
        case Quest::CharacterName::Daisy:
        {
          mActiveQuestMap.emplace(std::make_pair(Quest::CharacterName::Daisy, aEvent->mActiveQuest));
        }
        case Quest::CharacterName::Basil:
        {
          mActiveQuestMap.emplace(std::make_pair(Quest::CharacterName::Basil, aEvent->mActiveQuest));
        }
      }
    }
  }

  void NoticeBoard::OnRequestNoticeBoardStart(RequestNoticeBoardStart *aEvent)
  {
    if (mAssignedPostcard != nullptr)
    {
      Quest::State curPostcardState = mActiveQuestMap.at(mAssignedPostcard->GetCharacter())->GetState();
      if (curPostcardState == Quest::State::Completed)
      {
        // assign next postcard
        ++mAssignedPostcard;
        if (mAssignedPostcard != mPostcardVec.end()._Ptr)
        {
          AssignPostcard postcard(mAssignedPostcard);
          mSpace->SendEvent(Events::AssignPostcard, &postcard);
        }
      }
    }
    else
    {
      // assign first postcard
      mAssignedPostcard = &mPostcardVec[0];
      AssignPostcard postcard(mAssignedPostcard);
      mSpace->SendEvent(Events::AssignPostcard, &postcard);
    }
  }
}//end yte