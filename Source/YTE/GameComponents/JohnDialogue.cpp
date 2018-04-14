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
#include "YTE/GameComponents/DialogueDirector.hpp"
#include "YTE/GameComponents/NoticeBoard.hpp"
#include "YTE/GameComponents/DialogueGraph.hpp"
#include "YTE/Graphics/Animation.hpp"

namespace YTE
{
  YTEDefineEvent(TutorialUpdate);
  YTEDefineType(TutorialUpdate) { YTERegisterType(TutorialUpdate); }

  YTEDefineEvent(SpawnProgressionItem);
  YTEDefineEvent(SpawnProgressionLocation);
  YTEDefineEvent(SpawnProgressionDialogue);

  YTEDefineType(SpawnProgressionItem) { YTERegisterType(SpawnProgressionItem); }
  YTEDefineType(SpawnProgressionLocation) { YTERegisterType(SpawnProgressionLocation); }
  YTEDefineType(SpawnProgressionDialogue) { YTERegisterType(SpawnProgressionDialogue); }

  YTEDefineType(JohnDialogue) { YTERegisterType(JohnDialogue); }

  JohnDialogue::JohnDialogue(Composition *aOwner, Space *aSpace, RSValue *aProperties)
    : Component(aOwner, aSpace)
    , mSoundEmitter(nullptr)
    , mSoundSystem(nullptr)
  {
    YTEUnusedArgument(aProperties);
    // im the dumbest, should make class abstract, use mName instead of this dumb
    mQuestVec.emplace_back(Quest::Name::Introduction, Quest::CharacterName::John, mSpace);
    mQuestVec.emplace_back(Quest::Name::Fetch, Quest::CharacterName::John, mSpace);
    mQuestVec.emplace_back(Quest::Name::Explore, Quest::CharacterName::John, mSpace);
    mQuestVec.emplace_back(Quest::Name::Dialogue, Quest::CharacterName::John, mSpace);
    mQuestVec.emplace_back(Quest::Name::NotActive, Quest::CharacterName::John, mSpace);
    
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

    mAnimator = mOwner->GetComponent<Animator>();

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
        mSoundSystem->GetSoundIDFromString("CJ_I_H_5")
      });
      // G                                                             
      mDialogueConvos.emplace_back(std::vector<u64>
      {
        mSoundSystem->GetSoundIDFromString("CJ_I_G_1")
      });

      // FI ////////////////////////////
      // H
      mDialogueConvos.emplace_back(std::vector<u64>
      {
        mSoundSystem->GetSoundIDFromString("CJ_FI_H_1"),
        mSoundSystem->GetSoundIDFromString("CJ_FI_H_2"),
        mSoundSystem->GetSoundIDFromString("CJ_FI_H_3"),
        mSoundSystem->GetSoundIDFromString("CJ_FI_H_4"),
        mSoundSystem->GetSoundIDFromString("CJ_FI_H_5")
      });
      // NP           
      mDialogueConvos.emplace_back(std::vector<u64>
      {
        mSoundSystem->GetSoundIDFromString("CJ_FI_NP_1"),
        mSoundSystem->GetSoundIDFromString("CJ_FI_NP_2"),
        mSoundSystem->GetSoundIDFromString("CJ_FI_NP_3")
      });
      // C                                                              
      mDialogueConvos.emplace_back(std::vector<u64>
      {
        mSoundSystem->GetSoundIDFromString("CJ_FI_C_1"),
        mSoundSystem->GetSoundIDFromString("CJ_FI_C_2"),
        mSoundSystem->GetSoundIDFromString("CJ_FI_C_3")
      });
      // G
      mDialogueConvos.emplace_back(std::vector<u64>
      {
        mSoundSystem->GetSoundIDFromString("CJ_FI_G_1"),
        mSoundSystem->GetSoundIDFromString("CJ_FI_G_2")
      });

      // MeP //////////////////////////////////
      // H
      mDialogueConvos.emplace_back(std::vector<u64>
      {
         mSoundSystem->GetSoundIDFromString("CJ_I_H_1"),
         mSoundSystem->GetSoundIDFromString("CJ_I_H_1"),
         mSoundSystem->GetSoundIDFromString("CJ_I_H_1"),
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
         mSoundSystem->GetSoundIDFromString("CJ_I_H_1")
      });
      // G
      mDialogueConvos.emplace_back(std::vector<u64>
      {
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
         mSoundSystem->GetSoundIDFromString("CJ_I_H_1")
      });

      mConvosIter = mDialogueConvos.begin();
      mLinesIter = mConvosIter->begin();
    }
  }

  // this is super bad but i need to call this by hand in the tutorial
  void JohnDialogue::RegisterDialogue()
  {
    mSpace->YTERegister(Events::DialogueStart, this, &JohnDialogue::OnDialogueStart);
    mSpace->YTERegister(Events::DialogueNodeConfirm, this, &JohnDialogue::OnDialogueContinue);
    mSpace->YTERegister(Events::DialogueExit, this, &JohnDialogue::OnDialogueExit);
    mSpace->YTERegister(Events::PlaySoundEvent, this, &JohnDialogue::OnPlaySoundEvent);
    mSpace->YTERegister(Events::PlayAnimationEvent, this, &JohnDialogue::OnPlayAnimationEvent);
  }
  
  // this is super bad but i need to call this by hand in the tutorial
  void JohnDialogue::DeregisterDialogue()
  {
    mSpace->YTEDeregister(Events::DialogueStart, this, &JohnDialogue::OnDialogueStart);
    mSpace->YTEDeregister(Events::DialogueNodeConfirm, this, &JohnDialogue::OnDialogueContinue);
    mSpace->YTEDeregister(Events::DialogueExit, this, &JohnDialogue::OnDialogueExit);
    mSpace->YTEDeregister(Events::PlaySoundEvent, this, &JohnDialogue::OnPlaySoundEvent);
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
        // end me, just do it.
        ++mConvosIter;
        mLinesIter = mConvosIter->begin();
      }
      else
      {
        mActiveQuest->SetState(Quest::State::Completed); //the notice board looks for this to assign the next postcard
        // @@@(JAY): here is where we send an event to start the Postcard/Sailing tutorial
        mActiveNode = mActiveConvo->GetRoot(); // for now just repeat john's last convo to prevent crash
        mLinesIter = mConvosIter->begin();
      }
    }
    else if (mActiveQuest->GetName() == Quest::Name::NotActive)
    {
        // NotActive just resets, doesnt matter what state we pass it
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
      if (mActiveQuest->GetState() == Quest::State::Accomplished)
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

  void JohnDialogue::OnQuestStart(QuestStart *aEvent)
  {
    if (aEvent->mCharacter == mName)
    {
      if (mActiveQuest->GetName() == Quest::Name::NotActive)
      {
        mConvosIter = mPrevConvoIter;
      }
      ++mConvosIter;
      mLinesIter = mConvosIter->begin();

      mActiveQuest = &mQuestVec[(int)aEvent->mQuest];
      mActiveQuest->SetState(Quest::State::Received);
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

  void JohnDialogue::OnUpdateActiveQuestState(UpdateActiveQuestState *aEvent)
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

  void JohnDialogue::OnPlaySoundEvent(PlaySoundEvent *)
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

  void JohnDialogue::OnPlayAnimationEvent(PlayAnimationEvent *aEvent)
  {
    std::string anim = aEvent->animationName;
    
    std::cout << anim << std::endl;

    //mAnimator->PlayAnimation(anim);
  }

} //end yte