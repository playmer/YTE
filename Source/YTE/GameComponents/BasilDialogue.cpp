/******************************************************************************/
/*!
\file   BasilDialogue.cpp
\author Jonathan Ackerman
\par    email: jonathan.ackerman\@digipen.edu
\date   2018-02-27

All content (c) 2016 DigiPen  (USA) Corporation, all rights reserved.
*/
/******************************************************************************/

#include "YTE/GameComponents/BasilDialogue.hpp"
#include "YTE/GameComponents/DialogueDirector.hpp"
#include "YTE/GameComponents/NoticeBoard.hpp"

namespace YTE
{
  YTEDefineType(BasilDialogue) { YTERegisterType(BasilDialogue); }

  BasilDialogue::BasilDialogue(Composition *aOwner, Space *aSpace, RSValue *aProperties)
    : Component(aOwner, aSpace)
    , mSoundEmitter(nullptr)
    , mSoundSystem(nullptr)
  {
    YTEUnusedArgument(aProperties);

    mQuestVec.emplace_back(Quest::Name::Introduction, Quest::CharacterName::Basil, mSpace);
    mQuestVec.emplace_back(Quest::Name::Fetch, Quest::CharacterName::Basil, mSpace);
    mQuestVec.emplace_back(Quest::Name::Explore, Quest::CharacterName::Basil, mSpace);
    mQuestVec.emplace_back(Quest::Name::Dialogue, Quest::CharacterName::Basil, mSpace);
    mQuestVec.emplace_back(Quest::Name::NotActive, Quest::CharacterName::Basil, mSpace);
    
    mActiveQuest = &mQuestVec[(int)Quest::Name::Introduction];
    mActiveConvo = &mActiveQuest->GetConversations()->at(0);
    mActiveNode = mActiveConvo->GetRoot();
  }

  void BasilDialogue::Initialize()
  {
    mOwner->YTERegister(Events::CollisionStarted, this, &BasilDialogue::OnCollisionStarted);
    mOwner->YTERegister(Events::CollisionEnded, this, &BasilDialogue::OnCollisionEnded);
    mSpace->YTERegister(Events::QuestStart, this, &BasilDialogue::OnQuestStart);
    mSpace->YTERegister(Events::UpdateActiveQuestState, this, &BasilDialogue::OnUpdateActiveQuestState);


    mSoundEmitter = mOwner->GetComponent<WWiseEmitter>();
    mSoundSystem = mSpace->GetEngine()->GetComponent<WWiseSystem>();
    if (mSoundSystem)
    {
      // INTRO /////////////////////////////
      // H
      mDialogueConvos.emplace_back(std::vector<u64>
      {
         mSoundSystem->GetSoundIDFromString("CJ_I_H_1"),
         mSoundSystem->GetSoundIDFromString("CJ_I_H_2"),
         mSoundSystem->GetSoundIDFromString("CJ_I_H_3"),
         mSoundSystem->GetSoundIDFromString("CJ_I_H_4"),
         mSoundSystem->GetSoundIDFromString("CJ_I_H_4"),
         mSoundSystem->GetSoundIDFromString("CJ_I_H_4"),
         mSoundSystem->GetSoundIDFromString("CJ_I_H_4"),
         mSoundSystem->GetSoundIDFromString("CJ_I_H_5")
      });
      // G                                                             
      mDialogueConvos.emplace_back(std::vector<u64>
      {
        mSoundSystem->GetSoundIDFromString("CJ_I_G_1"),
        mSoundSystem->GetSoundIDFromString("CJ_I_G_1")
      });

      // FI ////////////////////////////
      // H
      mDialogueConvos.emplace_back(std::vector<u64>
      {
         mSoundSystem->GetSoundIDFromString("CJ_I_H_1"),
           mSoundSystem->GetSoundIDFromString("CJ_I_H_1"),
           mSoundSystem->GetSoundIDFromString("CJ_I_H_1"),
           mSoundSystem->GetSoundIDFromString("CJ_I_H_1"),
           mSoundSystem->GetSoundIDFromString("CJ_I_H_1"),
           mSoundSystem->GetSoundIDFromString("CJ_I_H_1")
      });
      // NP           
      mDialogueConvos.emplace_back(std::vector<u64>
      {
         mSoundSystem->GetSoundIDFromString("CJ_I_H_1")
      });
      // C                                                              
      mDialogueConvos.emplace_back(std::vector<u64>
      {
         mSoundSystem->GetSoundIDFromString("CJ_I_H_1"),
           mSoundSystem->GetSoundIDFromString("CJ_I_H_1"),
           mSoundSystem->GetSoundIDFromString("CJ_I_H_1"),
           mSoundSystem->GetSoundIDFromString("CJ_I_H_1"),
           mSoundSystem->GetSoundIDFromString("CJ_I_H_1")
      });
      // G
      mDialogueConvos.emplace_back(std::vector<u64>
      {
         mSoundSystem->GetSoundIDFromString("CJ_I_H_1"),
           mSoundSystem->GetSoundIDFromString("CJ_I_H_1")
      });

      // MeP //////////////////////////////////
      // H
      mDialogueConvos.emplace_back(std::vector<u64>
      {
         mSoundSystem->GetSoundIDFromString("CJ_I_H_1"),
           mSoundSystem->GetSoundIDFromString("CJ_I_H_1"),
           mSoundSystem->GetSoundIDFromString("CJ_I_H_1"),
           mSoundSystem->GetSoundIDFromString("CJ_I_H_1")
      });
      // NP           
      mDialogueConvos.emplace_back(std::vector<u64>
      {
         mSoundSystem->GetSoundIDFromString("CJ_I_H_1"),
           mSoundSystem->GetSoundIDFromString("CJ_I_H_1")
      });
      // C                                                              
      mDialogueConvos.emplace_back(std::vector<u64>
      {
         mSoundSystem->GetSoundIDFromString("CJ_I_H_1"),
           mSoundSystem->GetSoundIDFromString("CJ_I_H_1")
      });
      // G
      mDialogueConvos.emplace_back(std::vector<u64>
      {
         mSoundSystem->GetSoundIDFromString("CJ_I_H_1")
      });

      // GC //////////////////////////////////
      // H
      mDialogueConvos.emplace_back(std::vector<u64>
      {
         mSoundSystem->GetSoundIDFromString("CJ_I_H_1"),
           mSoundSystem->GetSoundIDFromString("CJ_I_H_1"),
           mSoundSystem->GetSoundIDFromString("CJ_I_H_1"),
           mSoundSystem->GetSoundIDFromString("CJ_I_H_1"),
           mSoundSystem->GetSoundIDFromString("CJ_I_H_1"),
           mSoundSystem->GetSoundIDFromString("CJ_I_H_1"),
           mSoundSystem->GetSoundIDFromString("CJ_I_H_1")
      });
      // NP           
      mDialogueConvos.emplace_back(std::vector<u64>
      {
         mSoundSystem->GetSoundIDFromString("CJ_I_H_1"),
           mSoundSystem->GetSoundIDFromString("CJ_I_H_1")
      });
      // C                                                              
      mDialogueConvos.emplace_back(std::vector<u64>
      {
         mSoundSystem->GetSoundIDFromString("CJ_I_H_1"),
           mSoundSystem->GetSoundIDFromString("CJ_I_H_1"),
           mSoundSystem->GetSoundIDFromString("CJ_I_H_1"),
           mSoundSystem->GetSoundIDFromString("CJ_I_H_1")
      });
      // G
      mDialogueConvos.emplace_back(std::vector<u64>
      {
         mSoundSystem->GetSoundIDFromString("CJ_I_H_1"),
           mSoundSystem->GetSoundIDFromString("CJ_I_H_1"),
           mSoundSystem->GetSoundIDFromString("CJ_I_H_1")
      });

      // NQ //////////////////////////////////
      // H
      mDialogueConvos.emplace_back(std::vector<u64>
      {
         mSoundSystem->GetSoundIDFromString("CJ_I_H_1"),
           mSoundSystem->GetSoundIDFromString("CJ_I_H_1"),
           mSoundSystem->GetSoundIDFromString("CJ_I_H_1"),
           mSoundSystem->GetSoundIDFromString("CJ_I_H_1"),
           mSoundSystem->GetSoundIDFromString("CJ_I_H_1")
      });

      mConvosIter = mDialogueConvos.begin();
      mPrevConvoIter = mConvosIter;
      mLinesIter = mConvosIter->begin();
    }
  }

  void BasilDialogue::RegisterDialogue()
  {
    mSpace->YTERegister(Events::DialogueStart, this, &BasilDialogue::OnDialogueStart);
    mSpace->YTERegister(Events::DialogueNodeConfirm, this, &BasilDialogue::OnDialogueContinue);
    mSpace->YTERegister(Events::DialogueExit, this, &BasilDialogue::OnDialogueExit);
    mSpace->YTERegister(Events::PlaySoundEvent, this, &BasilDialogue::OnPlaySoundEvent);
  }

  void BasilDialogue::DeregisterDialogue()
  {
    mSpace->YTEDeregister(Events::DialogueStart, this, &BasilDialogue::OnDialogueStart);
    mSpace->YTEDeregister(Events::DialogueNodeConfirm, this, &BasilDialogue::OnDialogueContinue);
    mSpace->YTEDeregister(Events::DialogueExit, this, &BasilDialogue::OnDialogueExit);
    mSpace->YTEDeregister(Events::PlaySoundEvent, this, &BasilDialogue::OnPlaySoundEvent);
  }

  void BasilDialogue::OnCollisionStarted(CollisionStarted *aEvent)
  {
    if (mActiveQuest->GetName() != Quest::Name::Introduction)
    {
      if (aEvent->OtherObject->GetComponent<BoatController>() != nullptr)
      {
        RegisterDialogue();
      }
    }
  }

  void BasilDialogue::OnCollisionEnded(CollisionEnded *aEvent)
  {
    if (mActiveQuest->GetName() != Quest::Name::Introduction)
    {
      if (aEvent->OtherObject->GetComponent<BoatController>() != nullptr)
      {
        DeregisterDialogue();
      }
    }
  }

  void BasilDialogue::Start()
  {
      // Send to the space a ptr to the activequest for the noticeboard
    NoticeBoardHookup firstQuest(&mActiveQuest);
    mSpace->SendEvent(Events::NoticeBoardHookup, &firstQuest);
  }

  void BasilDialogue::OnDialogueStart(DialogueStart *aEvent)
  {
    YTEUnusedArgument(aEvent);

    DialogueNode::NodeType type = mActiveNode->GetNodeType();
      // For anims and sounds we wont hear back from the director so send an event to ourselves to begin
    if (type == DialogueNode::NodeType::Anim || type == DialogueNode::NodeType::Sound)
    {
        // Anims and Sounds always have 1 child
      DialogueNodeConfirm next(0);
      mSpace->SendEvent(Events::DialogueNodeConfirm, &next);
    }
      // For input and text we rely on the director responding
    else if (type == DialogueNode::NodeType::Input || type == DialogueNode::NodeType::Text)
    {
      DialogueNodeReady next(mActiveNode->GetNodeData());
      next.DialogueType = type;
      mSpace->SendEvent(Events::DialogueNodeReady, &next);
    }
  }

  void BasilDialogue::OnDialogueExit(DialogueExit *aEvent)
  {
    YTEUnusedArgument(aEvent);

    if (mActiveQuest->GetName() == Quest::Name::Introduction)
    {
      if (mActiveConvo->GetName() == Conversation::Name::Hello)
      {
        TutorialUpdate nextTutorial(Quest::CharacterName::Daisy);
        mSpace->SendEvent(Events::TutorialUpdate, &nextTutorial);

        mActiveConvo = &mActiveQuest->GetConversations()->at(1);
        mActiveNode = mActiveConvo->GetRoot();

        ++mConvosIter;
        mLinesIter = mConvosIter->begin();
      }
      else
      {
        TutorialUpdate nextTutorial(Quest::CharacterName::John);
        mSpace->SendEvent(Events::TutorialUpdate, &nextTutorial);
        mActiveQuest->SetState(Quest::State::Completed);
        // where does the active node update?
      }
    }
    else if (mActiveQuest->GetName() == Quest::Name::NotActive)
    {
        // NotActive just resets
      UpdateActiveQuestState received(mName, Quest::State::Received);
      mSpace->SendEvent(Events::UpdateActiveQuestState, &received);
    }
    else
    {
      if (mActiveQuest->GetState() == Quest::State::Received)
      {
        UpdateActiveQuestState briefed(mName, Quest::State::Briefed);
        mSpace->SendEvent(Events::UpdateActiveQuestState, &briefed);

        ++mConvosIter;
        mLinesIter = mConvosIter->begin();
        if (mActiveQuest->GetName() == Quest::Name::Fetch)
        {
          SpawnProgressionItem spawnItem;
          mSpace->SendEvent(Events::SpawnProgressionItem, &spawnItem);
        }
        else if (mActiveQuest->GetName() == Quest::Name::Explore)
        {
          SpawnProgressionLocation spawnLocation;
          mSpace->SendEvent(Events::SpawnProgressionLocation, &spawnLocation);
        }
        else if (mActiveQuest->GetName() == Quest::Name::Dialogue)
        {
          SpawnProgressionDialogue spawnTalk;
          mSpace->SendEvent(Events::SpawnProgressionDialogue, &spawnTalk);
        }
      }
      // briefed just gets repeated
      if (mActiveQuest->GetState() == Quest::State::Briefed)
      {
        UpdateActiveQuestState briefed(mName, Quest::State::Briefed);
        mSpace->SendEvent(Events::UpdateActiveQuestState, &briefed);

        mLinesIter = mConvosIter->begin();
      }
      if (mActiveQuest->GetState() == Quest::State::Completed)
      {
        UpdateActiveQuestState completed(mName, Quest::State::Completed);
        mSpace->SendEvent(Events::UpdateActiveQuestState, &completed);

        ++mConvosIter;
        mLinesIter = mConvosIter->begin();
      }
      // post quest gets repeated
      if (mActiveQuest->GetState() == Quest::State::Completed)
      {
        UpdateActiveQuestState complete(mName, Quest::State::Completed);
        mSpace->SendEvent(Events::UpdateActiveQuestState, &complete);

        mLinesIter = mConvosIter->begin();
      }
    }
  }

  void BasilDialogue::OnDialogueContinue(DialogueNodeConfirm *aEvent)
  {
    mActiveNode->ActivateNode();
    mActiveNode = mActiveNode->GetChild(aEvent->Selection);
    if (mActiveNode != nullptr)
    {
      DialogueNode::NodeType type = mActiveNode->GetNodeType();
      while (type == DialogueNode::NodeType::Anim || type == DialogueNode::NodeType::Sound)
      {
          mActiveNode->ActivateNode();
          mActiveNode = mActiveNode->GetChild(0);
          if (mActiveNode != nullptr)
          {
            type = mActiveNode->GetNodeType();
          }
          else
          {
            DialogueExit diagExit;
            mSpace->SendEvent(Events::DialogueExit, &diagExit);
            return;
          }
      }
      // For input and text we rely on the director responding
      if (type == DialogueNode::NodeType::Input || type == DialogueNode::NodeType::Text)
      {
        DialogueNodeReady next(mActiveNode->GetNodeData());
        next.DialogueType = type;
        mSpace->SendEvent(Events::DialogueNodeReady, &next);
      }
    }
    else
    {
      DialogueExit diagExit;
      mSpace->SendEvent(Events::DialogueExit, &diagExit);
    }
  }

  void BasilDialogue::OnQuestStart(QuestStart *aEvent)
  {
    if (aEvent->mCharacter == mName)
    {
      if (mActiveQuest->GetName() == Quest::Name::NotActive)
      {
        mConvosIter = mPrevConvoIter;
      }
      ++mConvosIter; //doesnt work if you skipped any convos
      mLinesIter = mConvosIter->begin();

      mActiveQuest = &mQuestVec[(int)aEvent->mQuest];
      mActiveQuest->SetState(Quest::State::Received);
    }
    else
    {
      if (mActiveQuest->GetName() == Quest::Name::Introduction)
      {
        mIntroDone = true;
        mActiveQuest->SetState(Quest::State::TurnedIn);
      }
      mActiveQuest = &mQuestVec[(int)Quest::Name::NotActive];
      UpdateActiveQuestState notactive(mName, Quest::State::NotActive);
      mSpace->SendEvent(Events::UpdateActiveQuestState, &notactive);
    }
    mActiveConvo = &( *( mActiveQuest->GetConversations() ) )[(int)Conversation::Name::Hello];
    mActiveNode = mActiveConvo->GetRoot();
  }

  void BasilDialogue::OnUpdateActiveQuestState(UpdateActiveQuestState *aEvent)
  {
    if (aEvent->mCharacter == mName)
    {
      mActiveQuest->SetState(aEvent->mState);
      if (mActiveQuest->GetName() == Quest::Name::Introduction)
      {
      }
      else if (mActiveQuest->GetName() == Quest::Name::NotActive)
      {
        mActiveNode = mActiveConvo->GetRoot();
        mPrevConvoIter = mConvosIter; // save our place for sound cues
        mConvosIter = mDialogueConvos.end() - 1; // NotActive quest will always come last, and only has a Hello convo
        mLinesIter = mConvosIter->begin();
      }
      else
      {
        if (aEvent->mState == Quest::State::Briefed)
        {
          mActiveConvo = &(*(mActiveQuest->GetConversations()))[(int)Conversation::Name::NoProgress];
          mActiveNode = mActiveConvo->GetRoot();
        }
        if (aEvent->mState == Quest::State::Accomplished)
        {
          mActiveConvo = &(*(mActiveQuest->GetConversations()))[(int)Conversation::Name::Completed];
          mActiveNode = mActiveConvo->GetRoot();

          // because normally we move these by exiting convos, but here its from progression
          ++mConvosIter;
          mLinesIter = mConvosIter->begin();
        }
        if (aEvent->mState == Quest::State::Completed)
        {
          mActiveConvo = &(*(mActiveQuest->GetConversations()))[(int)Conversation::Name::PostQuest];
          mActiveNode = mActiveConvo->GetRoot();
        }
      }
    }
  }

  void BasilDialogue::OnPlaySoundEvent(PlaySoundEvent *)
  {
    if (mConvosIter != mDialogueConvos.end())
    {
      if (mLinesIter != mConvosIter->end())
      {
        mSoundEmitter->PlayEvent(*mLinesIter);
        ++mLinesIter;
      }
    }
  }

} //end yte