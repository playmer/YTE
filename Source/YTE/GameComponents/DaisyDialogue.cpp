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

    // Send to the space a ptr to the activequest for the noticeboard
    //NoticeBoardHookup firstQuest(&mActiveQuest);
    //mSpace->SendEvent(Events::NoticeBoardHookup, &firstQuest);
  }

  void DaisyDialogue::RegisterDialogue()
  {
    mSpace->YTERegister(Events::DialogueStart, this, &DaisyDialogue::OnDialogueStart);
    mSpace->YTERegister(Events::DialogueNodeConfirm, this, &DaisyDialogue::OnDialogueContinue);
    mSpace->YTERegister(Events::DialogueExit, this, &DaisyDialogue::OnDialogueExit);
  }

  void DaisyDialogue::DeregisterDialogue()
  {
    mSpace->YTEDeregister(Events::DialogueStart, this, &DaisyDialogue::OnDialogueStart);
    mSpace->YTEDeregister(Events::DialogueNodeConfirm, this, &DaisyDialogue::OnDialogueContinue);
    mSpace->YTEDeregister(Events::DialogueExit, this, &DaisyDialogue::OnDialogueExit);
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
    std::cout << std::endl << "SENDING notice board hookup" << std::endl;
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
      }
      else
      {
        TutorialUpdate nextTutorial(Quest::CharacterName::Basil);
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

  void DaisyDialogue::OnQuestStart(QuestStart *aEvent)
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