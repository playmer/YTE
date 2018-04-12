/******************************************************************************/
/*!
\file   DaisyDialogue.cpp
\author Jonathan Ackerman
\par    email: jonathan.ackerman\@digipen.edu
\date   2018-02-27

All content (c) 2016 DigiPen  (USA) Corporation, all rights reserved.
*/
/******************************************************************************/

#include "YTE/GameComponents/DaisyDialogue.hpp"
#include "YTE/GameComponents/NoticeBoard.hpp"

namespace YTE
{
  YTEDefineType(DaisyDialogue) { YTERegisterType(DaisyDialogue); }

  DaisyDialogue::DaisyDialogue(Composition *aOwner, Space *aSpace, RSValue *aProperties)
    : Component(aOwner, aSpace)
    , mSoundEmitter(nullptr)
    , mSoundSystem(nullptr)
  {
    YTEUnusedArgument(aProperties);

    mQuestVec.emplace_back(Quest::Name::Introduction, Quest::CharacterName::Daisy);
    mQuestVec.emplace_back(Quest::Name::Fetch, Quest::CharacterName::Daisy);
    mQuestVec.emplace_back(Quest::Name::Explore, Quest::CharacterName::Daisy);
    mQuestVec.emplace_back(Quest::Name::Dialogue, Quest::CharacterName::Daisy);
    mQuestVec.emplace_back(Quest::Name::NotActive, Quest::CharacterName::Daisy);
    
    mActiveQuest = &mQuestVec[(int)Quest::Name::Introduction];
    mActiveConvo = &mActiveQuest->GetConversations()->at(0);
    mActiveNode = mActiveConvo->GetRoot();
  }

  void DaisyDialogue::Initialize()
  {
    mOwner->YTERegister(Events::CollisionStarted, this, &DaisyDialogue::OnCollisionStarted);
    mOwner->YTERegister(Events::CollisionEnded, this, &DaisyDialogue::OnCollisionEnded);
    mSpace->YTERegister(Events::QuestStart, this, &DaisyDialogue::OnQuestStart);
    mSpace->YTERegister(Events::UpdateActiveQuestState, this, &DaisyDialogue::OnUpdateActiveQuestState);

    mSoundEmitter = mOwner->GetComponent<WWiseEmitter>();
    mSoundSystem = mSpace->GetEngine()->GetComponent<WWiseSystem>();
    if (mSoundSystem)
    {
      // INTRO::HELLO
      mDialogueConvos.emplace_back(std::map<std::string, u64>
      {
        std::make_pair("CJ_NQ_H_1", mSoundSystem->GetSoundIDFromString("CJ_NQ_H_1")),
        std::make_pair("CJ_NQ_H_2", mSoundSystem->GetSoundIDFromString("CJ_NQ_H_2")),
        std::make_pair("CJ_NQ_H_3", mSoundSystem->GetSoundIDFromString("CJ_NQ_H_3")),
        std::make_pair("CJ_NQ_H_4", mSoundSystem->GetSoundIDFromString("CJ_NQ_H_4")),
        std::make_pair("CJ_NQ_H_5", mSoundSystem->GetSoundIDFromString("CJ_NQ_H_5"))
      });
      // INTRO::POSTQUEST                                                             
      mDialogueConvos.emplace_back(std::map<std::string, u64>
      {
        std::make_pair("CJ_NQ_G_1", mSoundSystem->GetSoundIDFromString("CJ_NQ_G_1")),
        std::make_pair("CJ_NQ_G_2", mSoundSystem->GetSoundIDFromString("CJ_NQ_G_2"))
      });
      // FETCH::HELLO                                                                 
      mDialogueConvos.emplace_back(std::map<std::string, u64>
      {
        std::make_pair("CJ_FI_H_1", mSoundSystem->GetSoundIDFromString("CJ_FI_H_1")),
        std::make_pair("CJ_FI_H_2", mSoundSystem->GetSoundIDFromString("CJ_FI_H_2")),
        std::make_pair("CJ_FI_H_3", mSoundSystem->GetSoundIDFromString("CJ_FI_H_3")),
        std::make_pair("CJ_FI_H_4", mSoundSystem->GetSoundIDFromString("CJ_FI_H_4"))
      });
      // FETCH::NOPROGRESS            
      mDialogueConvos.emplace_back(std::map<std::string, u64>
      {
        std::make_pair("CJ_FI_NP_1", mSoundSystem->GetSoundIDFromString("CJ_FI_NP_1")),
        std::make_pair("CJ_FI_NP_2", mSoundSystem->GetSoundIDFromString("CJ_FI_NP_2")),
        std::make_pair("CJ_FI_NP_3", mSoundSystem->GetSoundIDFromString("CJ_FI_NP_3"))
      });
      mConvosIter = mDialogueConvos.begin();
      mLinesIter = mConvosIter->begin();
    }
  }

  void DaisyDialogue::RegisterDialogue()
  {
    mSpace->YTERegister(Events::DialogueStart, this, &DaisyDialogue::OnDialogueStart);
    mSpace->YTERegister(Events::DialogueNodeConfirm, this, &DaisyDialogue::OnDialogueContinue);
    mSpace->YTERegister(Events::DialogueExit, this, &DaisyDialogue::OnDialogueExit);
    mSpace->YTERegister(Events::PlaySoundEvent, this, &DaisyDialogue::OnPlaySoundEvent);
  }

  void DaisyDialogue::DeregisterDialogue()
  {
    mSpace->YTEDeregister(Events::DialogueStart, this, &DaisyDialogue::OnDialogueStart);
    mSpace->YTEDeregister(Events::DialogueNodeConfirm, this, &DaisyDialogue::OnDialogueContinue);
    mSpace->YTEDeregister(Events::DialogueExit, this, &DaisyDialogue::OnDialogueExit);
    mSpace->YTEDeregister(Events::PlaySoundEvent, this, &DaisyDialogue::OnPlaySoundEvent);
  }

  void DaisyDialogue::OnCollisionStarted(CollisionStarted *aEvent)
  {
    if (mActiveQuest->GetName() != Quest::Name::Introduction)
    {
      if (aEvent->OtherObject->GetComponent<BoatController>() != nullptr)
      {
        RegisterDialogue();
      }
    }
  }

  void DaisyDialogue::OnCollisionEnded(CollisionEnded *aEvent)
  {
    if (mActiveQuest->GetName() != Quest::Name::Introduction)
    {
      if (aEvent->OtherObject->GetComponent<BoatController>() != nullptr)
      {
        DeregisterDialogue();
      }
    }
  }

  void DaisyDialogue::Start()
  {
      // Send to the space a ptr to the activequest for the noticeboard
    NoticeBoardHookup firstQuest(&mActiveQuest);
    mSpace->SendEvent(Events::NoticeBoardHookup, &firstQuest);
  }

  void DaisyDialogue::OnDialogueStart(DialogueStart *aEvent)
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

  void DaisyDialogue::OnDialogueExit(DialogueExit *aEvent)
  {
    YTEUnusedArgument(aEvent);

    if (mActiveQuest->GetName() == Quest::Name::Introduction)
    {
      if (mActiveConvo->GetName() == Conversation::Name::Hello)
      {
        TutorialUpdate nextTutorial(Quest::CharacterName::Basil);
        mSpace->SendEvent(Events::TutorialUpdate, &nextTutorial);

        mActiveConvo = &mActiveQuest->GetConversations()->at(1);
        mActiveNode = mActiveConvo->GetRoot();

        ++mConvosIter;
        mLinesIter = mConvosIter->begin();
      }
      else
      {
        TutorialUpdate nextTutorial(Quest::CharacterName::Basil);
        mSpace->SendEvent(Events::TutorialUpdate, &nextTutorial);
        mActiveQuest->SetState(Quest::State::Completed);
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
    }
  }

  void DaisyDialogue::OnDialogueContinue(DialogueNodeConfirm *aEvent)
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
        next.Selection = aEvent->Selection;
        mSpace->SendEvent(Events::DialogueNodeReady, &next);
      }
    }
    else
    {
      DialogueExit diagExit;
      mSpace->SendEvent(Events::DialogueExit, &diagExit);
    }
  }

  void DaisyDialogue::OnQuestStart(QuestStart *aEvent)
  {
    if (aEvent->mCharacter == mName)
    {
      mActiveQuest = &mQuestVec[(int)aEvent->mQuest];
      UpdateActiveQuestState received(mName, Quest::State::Received);
      mSpace->SendEvent(Events::UpdateActiveQuestState, &received);

      ++mConvosIter;
      mLinesIter = mConvosIter->begin();
    }
    else
    {
      mActiveQuest = &mQuestVec[(int)Quest::Name::NotActive];
      UpdateActiveQuestState notactive(mName, Quest::State::NotActive);
      mSpace->SendEvent(Events::UpdateActiveQuestState, &notactive);
    }
    mActiveConvo = &( *( mActiveQuest->GetConversations() ) )[(int)Conversation::Name::Hello];
    mActiveNode = mActiveConvo->GetRoot();
  }

  void DaisyDialogue::OnUpdateActiveQuestState(UpdateActiveQuestState *aEvent)
  {
    if (aEvent->mCharacter == mName)
    {
      mActiveQuest->SetState(aEvent->mState);
      if (mActiveQuest->GetName() == Quest::Name::Introduction)
      {
        /*Remember when people used to say "rejected!"
        if (aEvent->mState == Quest::State::Completed)
        {
          mActiveConvo = &mActiveQuest->GetConversations()->at(1);
          mActiveNode = mActiveConvo->GetRoot();
        }
        */
      }
      else if (mActiveQuest->GetName() == Quest::Name::NotActive)
      {
        mActiveNode = mActiveConvo->GetRoot();
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
        }
        if (aEvent->mState == Quest::State::Completed)
        {
          mActiveConvo = &(*(mActiveQuest->GetConversations()))[(int)Conversation::Name::PostQuest];
          mActiveNode = mActiveConvo->GetRoot();
        }
      }
    }
  }

  void DaisyDialogue::OnPlaySoundEvent(PlaySoundEvent *)
  {
    if (mConvosIter != mDialogueConvos.end())
    {
      if (mLinesIter != mConvosIter->end())
      {
        mSoundEmitter->PlayEvent(mLinesIter->second);
        std::cout << mLinesIter->first << std::endl;
        ++mLinesIter;
      }
    }
  }

} //end yte