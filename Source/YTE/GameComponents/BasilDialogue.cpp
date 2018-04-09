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
#include "YTE/GameComponents/JohnDialogue.hpp" /* TutorialUpdate */
#include "YTE/GameComponents/NoticeBoard.hpp"

namespace YTE
{
  YTEDefineType(BasilDialogue) { YTERegisterType(BasilDialogue); }

  BasilDialogue::BasilDialogue(Composition *aOwner, Space *aSpace, RSValue *aProperties)
    : Component(aOwner, aSpace)
  {
    YTEUnusedArgument(aProperties);

    mQuestVec.emplace_back(Quest::Name::Introduction, Quest::CharacterName::Basil);
    mQuestVec.emplace_back(Quest::Name::Fetch, Quest::CharacterName::Basil);
    mQuestVec.emplace_back(Quest::Name::Explore, Quest::CharacterName::Basil);
    mQuestVec.emplace_back(Quest::Name::Dialogue, Quest::CharacterName::Basil);
    mQuestVec.emplace_back(Quest::Name::NotActive, Quest::CharacterName::Basil);
    
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

    // Send to the space a ptr to the activequest for the noticeboard
    //NoticeBoardHookup firstQuest(&mActiveQuest);
    //mSpace->SendEvent(Events::NoticeBoardHookup, &firstQuest);
  }

  void BasilDialogue::RegisterDialogue()
  {
    mSpace->YTERegister(Events::DialogueStart, this, &BasilDialogue::OnDialogueStart);
    mSpace->YTERegister(Events::DialogueNodeConfirm, this, &BasilDialogue::OnDialogueContinue);
    mSpace->YTERegister(Events::DialogueExit, this, &BasilDialogue::OnDialogueExit);
  }

  void BasilDialogue::DeregisterDialogue()
  {
    mSpace->YTEDeregister(Events::DialogueStart, this, &BasilDialogue::OnDialogueStart);
    mSpace->YTEDeregister(Events::DialogueNodeConfirm, this, &BasilDialogue::OnDialogueContinue);
    mSpace->YTEDeregister(Events::DialogueExit, this, &BasilDialogue::OnDialogueExit);
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
    std::cout << std::endl << "SENDING notice board hookup" << std::endl;
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
      }
      else
      {
        TutorialUpdate nextTutorial(Quest::CharacterName::John);
        mSpace->SendEvent(Events::TutorialUpdate, &nextTutorial);
        mActiveQuest->SetState(Quest::State::Completed); //the notice board looks for this to assign the next postcard i think
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
      }
      if (mActiveQuest->GetState() == Quest::State::Completed)
      {
        UpdateActiveQuestState completed(mName, Quest::State::Completed);
        mSpace->SendEvent(Events::UpdateActiveQuestState, &completed);
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
          mActiveNode = mActiveNode->GetChild(aEvent->Selection);
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
      mActiveQuest = &mQuestVec[(int)aEvent->mQuest];
      UpdateActiveQuestState received(mName, Quest::State::Received);
      mSpace->SendEvent(Events::UpdateActiveQuestState, &received);
    }
    else
    {
      mActiveQuest = &mQuestVec[(int)Quest::Name::NotActive];
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
        if (aEvent->mState == Quest::State::Completed)
        {
          mActiveConvo = &mActiveQuest->GetConversations()->at(1);
          mActiveNode = mActiveConvo->GetRoot();
        }
      }
      else if (mActiveQuest->GetName() == Quest::Name::NotActive)
      {
        mActiveNode = mActiveConvo->GetRoot();
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
} //end yte