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

#include "YTE/GameComponents/HudController.hpp"
#include "YTE/GameComponents/ProgressionParticles.hpp"

namespace YTE
{
  YTEDefineEvent(NoticeBoardHookup);

  YTEDefineType(NoticeBoardHookup) { YTERegisterType(NoticeBoardHookup); }

  YTEDefineType(NoticeBoard) { YTERegisterType(NoticeBoard); }

  NoticeBoard::NoticeBoard(Composition *aOwner, Space *aSpace, RSValue *aProperties)
    : Component(aOwner, aSpace)
  {
    YTEUnusedArgument(aProperties);
      // these are gonna be a tutorial, not postcards
    //mPostcardVec.emplace_back(Quest::CharacterName::Daisy, Quest::Name::Introduction);
    //mPostcardVec.emplace_back(Quest::CharacterName::Basil, Quest::Name::Introduction);

      // this is what determines the player's quest order
    mPostcardVec.emplace_back(Quest::CharacterName::John, Quest::Name::Introduction); // this is our tutorial postcard
    mPostcardVec.emplace_back(Quest::CharacterName::John, Quest::Name::Fetch);
    mPostcardVec.emplace_back(Quest::CharacterName::Daisy, Quest::Name::Explore);
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
    mAssignedPostcard = &mPostcardVec[0];
  }

  void NoticeBoard::OnCollisionStarted(CollisionStarted *aEvent)
  {
    if (aEvent->OtherObject->GetComponent<QuestLogic>() != nullptr)
    {
      mSpace->YTERegister(Events::RequestNoticeBoardStart, this, &NoticeBoard::OnRequestNoticeBoardStart);
      aEvent->OtherObject->GetComponent<QuestLogic>()->HookupPostcardHandle(&mAssignedPostcard);

      Quest::CharacterName character = mAssignedPostcard->GetCharacter();
      Quest *quest = *(mActiveQuestMap.at(character));
      if (quest->GetState() == Quest::State::Completed)
      {
        DialoguePossible diagEvent;
        diagEvent.isPossible = true;
        mSpace->SendEvent(Events::DialoguePossible, &diagEvent);
      }
    }
  }

  void NoticeBoard::OnCollisionEnded(CollisionEnded *aEvent)
  {
    if (aEvent->OtherObject->GetComponent<QuestLogic>() != nullptr)
    {
      mSpace->YTEDeregister(Events::RequestNoticeBoardStart, this, &NoticeBoard::OnRequestNoticeBoardStart);

      DialoguePossible diagEvent;
      diagEvent.isPossible = false;
      mSpace->SendEvent(Events::DialoguePossible, &diagEvent);
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
        break;
      }
      case Quest::CharacterName::Daisy:
      {
        mActiveQuestMap.emplace(std::make_pair(Quest::CharacterName::Daisy, aEvent->mActiveQuestHandle));
        break;
      }
      case Quest::CharacterName::Basil:
      {
        mActiveQuestMap.emplace(std::make_pair(Quest::CharacterName::Basil, aEvent->mActiveQuestHandle));
        break;
      }
    }
  }

  void NoticeBoard::OnRequestNoticeBoardStart(RequestNoticeBoardStart *aEvent)
  {
    YTEUnusedArgument(aEvent);

    if (mAssignedPostcard != nullptr)
    {
      Quest *curQuest = *(mActiveQuestMap.at(mAssignedPostcard->GetCharacter()));
      Quest::State curState = curQuest->GetState();

      if (curState == Quest::State::Completed)
      {
        curQuest->SetState(Quest::State::TurnedIn);
        // assign next postcard
        ++mAssignedPostcard;
        if (mAssignedPostcard != mPostcardVec.end()._Ptr)
        {
          Quest::CharacterName charName = mAssignedPostcard->GetCharacter();
          Quest::Name questName = mAssignedPostcard->GetQuest();

          PostcardUpdate postcardEvent;
          postcardEvent.Number = 0;

          if (charName == Quest::CharacterName::John)
          {
            if (questName == Quest::Name::Fetch)
            {
              // quest 1
              postcardEvent.Number = 0;
            }
            else if (questName == Quest::Name::Dialogue)
            {
              // quest 4
              postcardEvent.Number = 3;
            }
            else if (questName == Quest::Name::Explore)
            {
              // quest 7
              postcardEvent.Number = 6;
            }
          }
          else if (charName == Quest::CharacterName::Daisy)
          {
            if (questName == Quest::Name::Fetch)
            {
              // daisy 8
              postcardEvent.Number = 7;
            }
            else if (questName == Quest::Name::Dialogue)
            {
              // quest 6
              postcardEvent.Number = 5;
            }
            else if (questName == Quest::Name::Explore)
            {
              // quest 2
              postcardEvent.Number = 1;
            }
          }
          else if (charName == Quest::CharacterName::Basil)
          {
            if (questName == Quest::Name::Fetch)
            {
              // quest 3
              postcardEvent.Number = 2;
            }
            else if (questName == Quest::Name::Dialogue)
            {
              // quest 9
              postcardEvent.Number = 8;
            }
            else if (questName == Quest::Name::Explore)
            {
              // quest 5
              postcardEvent.Number = 4;
            }
          }

          mSpace->SendEvent(Events::PostcardUpdate, &postcardEvent);

          // make a particles poof
          ProgressionHappened progEvent;
          mSpace->SendEvent(Events::ProgressionHappened, &progEvent);

          QuestStart quest(charName, questName);
          mSpace->SendEvent(Events::QuestStart, &quest);

          auto emitter = mOwner->GetComponent<WWiseEmitter>();

          if (emitter)
          {
            emitter->PlayEvent("UI_Quest_Offered");
          }

          DialoguePossible diagEvent;
          diagEvent.isPossible = false;
          mSpace->SendEvent(Events::DialoguePossible, &diagEvent);
        }
      }
    }
    else
    {
      int temp = 0;
      temp++;
    }
  }
}//end yte