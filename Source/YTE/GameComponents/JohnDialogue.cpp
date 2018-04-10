/******************************************************************************/
/*!
\file   JohnDialogue.cpp
\author Jonathan Ackerman
\par    email: jonathan.ackerman\@digipen.edu
\date   2018-02-27

All content (c) 2016 DigiPen  (USA) Corporation, all rights reserved.
*/
/******************************************************************************/

#include "YTE/GameComponents/JohnDialogue.hpp"
#include "YTE/GameComponents/NoticeBoard.hpp"
#include <AK/SoundEngine/Common/AkDynamicDialogue.h>

namespace YTE
{
  YTEDefineEvent(TutorialUpdate);
  YTEDefineType(TutorialUpdate) { YTERegisterType(TutorialUpdate); }

  YTEDefineType(JohnDialogue) { YTERegisterType(JohnDialogue); }

  JohnDialogue::JohnDialogue(Composition *aOwner, Space *aSpace, RSValue *aProperties)
    : Component(aOwner, aSpace)
    , mSoundEmitter(nullptr)
    , mSoundSytem(nullptr)
  {
    YTEUnusedArgument(aProperties);
    // im the dumbest, should make class abstract, use mName instead of this dumb
    mQuestVec.emplace_back(Quest::Name::Introduction, Quest::CharacterName::John);
    mQuestVec.emplace_back(Quest::Name::Fetch, Quest::CharacterName::John);
    mQuestVec.emplace_back(Quest::Name::Explore, Quest::CharacterName::John);
    mQuestVec.emplace_back(Quest::Name::Dialogue, Quest::CharacterName::John);
    mQuestVec.emplace_back(Quest::Name::NotActive, Quest::CharacterName::John);
    
    mActiveQuest = &mQuestVec[(int)Quest::Name::Introduction];
    mActiveConvo = &mActiveQuest->GetConversations()->at(0);
    mActiveNode = mActiveConvo->GetRoot();
  }

  void JohnDialogue::Initialize()
  {
    mOwner->YTERegister(Events::CollisionStarted, this, &JohnDialogue::OnCollisionStarted);
    mOwner->YTERegister(Events::CollisionEnded, this, &JohnDialogue::OnCollisionEnded);
    mSpace->YTERegister(Events::QuestStart, this, &JohnDialogue::OnQuestStart);
    mSpace->YTERegister(Events::UpdateActiveQuestState, this, &JohnDialogue::OnUpdateActiveQuestState);

    mSoundEmitter = mOwner->GetComponent<WWiseEmitter>();
    mSoundSystem = mSpace->GetEngine()->GetComponent<WWiseSystem>();
    if (mSoundSystem)
      mSoundDialogueJohn = mSoundSystem->GetSoundIDFromString("ChefJohn");
  }

  // this is super bad but i need to call this by hand in the tutorial
  void JohnDialogue::RegisterDialogue()
  {
    mSpace->YTERegister(Events::DialogueStart, this, &JohnDialogue::OnDialogueStart);
    mSpace->YTERegister(Events::DialogueNodeConfirm, this, &JohnDialogue::OnDialogueContinue);
    mSpace->YTERegister(Events::DialogueExit, this, &JohnDialogue::OnDialogueExit);
  }
  
  // this is super bad but i need to call this by hand in the tutorial
  void JohnDialogue::DeregisterDialogue()
  {
    mSpace->YTEDeregister(Events::DialogueStart, this, &JohnDialogue::OnDialogueStart);
    mSpace->YTEDeregister(Events::DialogueNodeConfirm, this, &JohnDialogue::OnDialogueContinue);
    mSpace->YTEDeregister(Events::DialogueExit, this, &JohnDialogue::OnDialogueExit);
  }

  const char **JohnDialogue::StatesToStrings()
  {
    const char *ret[3];
    if (mActiveQuest->GetName() == Quest::Name::Fetch)
    {
      ret[0] = DynamicDialogueArgs::Quest::Fetch;
    }
    else if (mActiveQuest->GetName() == Quest::Name::Explore)
    {
      ret[0] = DynamicDialogueArgs::Quest::Explore;
    }
    else if (mActiveQuest->GetName() == Quest::Name::Dialogue)
    {
      ret[0] = DynamicDialogueArgs::Quest::Dialogue;
    }
    else if (mActiveQuest->GetName() == Quest::Name::NotActive)
    {
      ret[0] = DynamicDialogueArgs::Quest::NotActive;
    }

    if (mActiveConvo->GetName() == Conversation::Name::Hello)
    {
      ret[1] = DynamicDialogueArgs::Conversation::Hello;
    }
    else if (mActiveConvo->GetName() == Conversation::Name::NoProgress)
    {
      ret[1] = DynamicDialogueArgs::Conversation::NoProgress;
    }
    else if (mActiveConvo->GetName() == Conversation::Name::Completed)
    {
      ret[1] = DynamicDialogueArgs::Conversation::Completed;
    }
    else if (mActiveConvo->GetName() == Conversation::Name::PostQuest)
    {
      ret[1] = DynamicDialogueArgs::Conversation::PostQuest;
    }
  }

  void JohnDialogue::OnCollisionStarted(CollisionStarted *aEvent)
  {
    if (aEvent->OtherObject->GetComponent<BoatController>() != nullptr)
    {
      RegisterDialogue();
    }
  }

  void JohnDialogue::OnCollisionEnded(CollisionEnded *aEvent)
  {
    if (aEvent->OtherObject->GetComponent<BoatController>() != nullptr)
    {
      DeregisterDialogue();
    }
  }

  void JohnDialogue::Start()
  {
    std::cout << std::endl << "SENDING notice board hookup" << std::endl;
      // Send to the space a ptr to the activequest for the noticeboard
    NoticeBoardHookup firstQuest(&mActiveQuest);
    mSpace->SendEvent(Events::NoticeBoardHookup, &firstQuest);
  }

  void JohnDialogue::OnDialogueStart(DialogueStart *aEvent)
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
      if (type == DialogueNode::NodeType::Text)
      {
        AK::SoundEngine::DynamicDialogue::ResolveDialogueEvent(mSoundDialogueJohn, );
      }

      DialogueNodeReady next(mActiveNode->GetNodeData());
      next.DialogueType = type;
      mSpace->SendEvent(Events::DialogueNodeReady, &next);
    }
  }

  void JohnDialogue::OnDialogueExit(DialogueExit *aEvent)
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
        mActiveQuest->SetState(Quest::State::Completed); //the notice board looks for this to assign the next postcard i think
        // @@@(JAY): here is where we send an event to start the Postcard/Sailing tutorial
        mActiveNode = mActiveConvo->GetRoot(); // for now just repeat john's last convo to prevent crash
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
      // briefed just gets repeated
      if (mActiveQuest->GetState() == Quest::State::Briefed)
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

  void JohnDialogue::OnDialogueContinue(DialogueNodeConfirm *aEvent)
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

  void JohnDialogue::OnQuestStart(QuestStart *aEvent)
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

  void JohnDialogue::OnUpdateActiveQuestState(UpdateActiveQuestState *aEvent)
  {
    if (aEvent->mCharacter == mName)
    {
      mActiveQuest->SetState(aEvent->mState);
      if (mActiveQuest->GetName() == Quest::Name::Introduction)
      {
        /*NAH FAm
        if (aEvent->mState == Quest::State::Briefed)
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