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
#include "YTE/GameComponents/DialogueDirector.hpp"
#include "YTE/GameComponents/NoticeBoard.hpp"
#include "YTE/GameComponents/CameraAnchor.hpp"
#include "YTE/Graphics/Animation.hpp"
#include "YTE/GameComponents/StarMovement.hpp"
#include "YTE/GameComponents/ProgressionParticles.hpp"

namespace YTE
{
  YTEDefineType(DaisyDialogue) { RegisterType<DaisyDialogue>();
    TypeBuilder<DaisyDialogue> builder; }

  DaisyDialogue::DaisyDialogue(Composition *aOwner, Space *aSpace, RSValue *aProperties)
    : Component(aOwner, aSpace)
    , mSoundEmitter(nullptr)
    , mSoundSystem(nullptr)
    , mCameraAnchor(nullptr)
    , mLambAnchor(nullptr)
  {
    UnusedArguments(aProperties);

    mQuestVec.emplace_back(Quest::Name::Introduction, Quest::CharacterName::Daisy, mSpace);
    mQuestVec.emplace_back(Quest::Name::Fetch, Quest::CharacterName::Daisy, mSpace);
    mQuestVec.emplace_back(Quest::Name::Explore, Quest::CharacterName::Daisy, mSpace);
    mQuestVec.emplace_back(Quest::Name::Dialogue, Quest::CharacterName::Daisy, mSpace);
    mQuestVec.emplace_back(Quest::Name::NotActive, Quest::CharacterName::Daisy, mSpace);
    
    mActiveQuest = &mQuestVec[(int)Quest::Name::Introduction];
    mActiveConvo = &mActiveQuest->GetConversations()->at(0);
    mActiveNode = mActiveConvo->GetRoot();
  }

  void DaisyDialogue::Initialize()
  {
    mOwner->RegisterEvent<&DaisyDialogue::OnCollisionStarted>(Events::CollisionStarted, this);
    mOwner->RegisterEvent<&DaisyDialogue::OnCollisionEnded>(Events::CollisionEnded, this);
    mSpace->RegisterEvent<&DaisyDialogue::OnQuestStart>(Events::QuestStart, this);
    mSpace->RegisterEvent<&DaisyDialogue::OnUpdateActiveQuestState>(Events::UpdateActiveQuestState, this);

    if (Composition *lambAnchor = mOwner->FindFirstCompositionByName("LambAnchor"))
    {
      mLambAnchor = lambAnchor->GetComponent<Transform>();
    }

    auto children = mOwner->GetCompositions()->All();

    for (auto &child : children)
    {
      if (child.second->GetComponent<CameraAnchor>() != nullptr)
      {
        mCameraAnchor = child.second.get();
        break;
      }
    }

    mAnimator = mOwner->GetComponent<Animator>();
    mAnimator->SetDefaultAnimation("Idle.fbx");

    mSoundEmitter = mOwner->GetComponent<WWiseEmitter>();
    mSoundSystem = mSpace->GetEngine()->GetComponent<WWiseSystem>();
    if (mSoundSystem)
    {
      // INTRO /////////////////////////////
      // H
      mDialogueConvos.emplace_back(std::map<std::string, u64>
      {
         std::make_pair("FD_I_H_1", mSoundSystem->GetSoundIDFromString("FD_I_H_1")),
         std::make_pair("FD_I_H_2", mSoundSystem->GetSoundIDFromString("FD_I_H_2")),
         std::make_pair("FD_I_H_3", mSoundSystem->GetSoundIDFromString("FD_I_H_3")),
         std::make_pair("FD_I_H_4", mSoundSystem->GetSoundIDFromString("FD_I_H_4"))
      });
      // G                                                             
      mDialogueConvos.emplace_back(std::map<std::string, u64>
      {
        std::make_pair("FD_I_G_1", mSoundSystem->GetSoundIDFromString("FD_I_G_1")),
        std::make_pair("FD_I_G_2", mSoundSystem->GetSoundIDFromString("FD_I_G_2"))
      });

      // CG ////////////////////////////
      // H
      mDialogueConvos.emplace_back(std::map<std::string, u64>
      {
        std::make_pair("FD_CG_H_1", mSoundSystem->GetSoundIDFromString("FD_CG_H_1")),
        std::make_pair("FD_CG_H_2", mSoundSystem->GetSoundIDFromString("FD_CG_H_2")),
        std::make_pair("FD_CG_H_3", mSoundSystem->GetSoundIDFromString("FD_CG_H_3"))
      });
      // NP           
      mDialogueConvos.emplace_back(std::map<std::string, u64>
      {
        std::make_pair("FD_CG_NP_1", mSoundSystem->GetSoundIDFromString("FD_CG_NP_1"))
      });
      // C                                                              
      mDialogueConvos.emplace_back(std::map<std::string, u64>
      {
         std::make_pair("FD_CG_C_1", mSoundSystem->GetSoundIDFromString("FD_CG_C_1")),
         std::make_pair("FD_CG_C_2", mSoundSystem->GetSoundIDFromString("FD_CG_C_2"))
      });
      // G
      mDialogueConvos.emplace_back(std::map<std::string, u64>
      {
        std::make_pair("FD_CG_G_1", mSoundSystem->GetSoundIDFromString("FD_CG_G_1"))
      });

      // D //////////////////////////////////
      // H
      mDialogueConvos.emplace_back(std::map<std::string, u64>
      {
         std::make_pair("FD_D_H_1", mSoundSystem->GetSoundIDFromString("FD_D_H_1")),
         std::make_pair("FD_D_H_2", mSoundSystem->GetSoundIDFromString("FD_D_H_2"))
      });
      // NP           
      mDialogueConvos.emplace_back(std::map<std::string, u64>
      {
        std::make_pair("FD_D_NP_1", mSoundSystem->GetSoundIDFromString("FD_D_NP_1")),
        std::make_pair("FD_D_NP_2", mSoundSystem->GetSoundIDFromString("FD_D_NP_2"))
      });
      // C                                                              
      mDialogueConvos.emplace_back(std::map<std::string, u64>
      {
         std::make_pair("FD_D_C_1", mSoundSystem->GetSoundIDFromString("FD_D_C_1")),
         std::make_pair("FD_D_C_2", mSoundSystem->GetSoundIDFromString("FD_D_C_2"))
      });
      // G
      mDialogueConvos.emplace_back(std::map<std::string, u64>
      {
        std::make_pair("FD_D_G_1", mSoundSystem->GetSoundIDFromString("FD_D_G_1"))
      });

      // SfS //////////////////////////////////
      // H
      mDialogueConvos.emplace_back(std::map<std::string, u64>
      {
        std::make_pair("FD_SfS_H_1", mSoundSystem->GetSoundIDFromString("FD_SfS_H_1")),
        std::make_pair("FD_SfS_H_2", mSoundSystem->GetSoundIDFromString("FD_SfS_H_2")),
        std::make_pair("FD_SfS_H_3", mSoundSystem->GetSoundIDFromString("FD_SfS_H_3")),
        std::make_pair("FD_SfS_H_4", mSoundSystem->GetSoundIDFromString("FD_SfS_H_4")),
        std::make_pair("FD_SfS_H_5", mSoundSystem->GetSoundIDFromString("FD_SfS_H_5")),
        std::make_pair("FD_SfS_H_6", mSoundSystem->GetSoundIDFromString("FD_SfS_H_6")),
        std::make_pair("FD_SfS_H_7", mSoundSystem->GetSoundIDFromString("FD_SfS_H_7")),
        std::make_pair("FD_SfS_H_8", mSoundSystem->GetSoundIDFromString("FD_SfS_H_8")),
        std::make_pair("FD_SfS_H_9", mSoundSystem->GetSoundIDFromString("FD_SfS_H_9")),
        std::make_pair("FD_SfS_H_10", mSoundSystem->GetSoundIDFromString("FD_SfS_H_10")),
        std::make_pair("FD_SfS_H_11", mSoundSystem->GetSoundIDFromString("FD_SfS_H_11")),
        std::make_pair("FD_SfS_H_12", mSoundSystem->GetSoundIDFromString("FD_SfS_H_12"))
      });
      // NP           
      mDialogueConvos.emplace_back(std::map<std::string, u64>
      {
         std::make_pair("FD_SfS_NP_1", mSoundSystem->GetSoundIDFromString("FD_SfS_NP_1")),
         std::make_pair("FD_SfS_NP_2", mSoundSystem->GetSoundIDFromString("FD_SfS_NP_2"))
      });
      // C                                                              
      mDialogueConvos.emplace_back(std::map<std::string, u64>
      {
        std::make_pair("FD_SfS_C_1", mSoundSystem->GetSoundIDFromString("FD_SfS_C_1")),
        std::make_pair("FD_SfS_C_2", mSoundSystem->GetSoundIDFromString("FD_SfS_C_2")),
        std::make_pair("FD_SfS_C_3", mSoundSystem->GetSoundIDFromString("FD_SfS_C_3")),
        std::make_pair("FD_SfS_C_4", mSoundSystem->GetSoundIDFromString("FD_SfS_C_4")),
        std::make_pair("FD_SfS_C_5", mSoundSystem->GetSoundIDFromString("FD_SfS_C_5")),
        std::make_pair("FD_SfS_C_6", mSoundSystem->GetSoundIDFromString("FD_SfS_C_6"))
      });
      // G
      mDialogueConvos.emplace_back(std::map<std::string, u64>
      {
        std::make_pair("FD_SfS_G_1", mSoundSystem->GetSoundIDFromString("FD_SfS_G_1"))
      });

      // NQ //////////////////////////////////
      // H
      mDialogueConvos.emplace_back(std::map<std::string, u64>
      {
         std::make_pair("FD_NQ_H_1", mSoundSystem->GetSoundIDFromString("FD_NQ_H_1")),
         std::make_pair("FD_NQ_H_2", mSoundSystem->GetSoundIDFromString("FD_NQ_H_2"))
      });

      mConvosIter = mDialogueConvos.begin();
      mPrevConvoIter = mConvosIter;
      mLinesIter = mConvosIter->begin();
    }
  }

  void DaisyDialogue::RegisterDialogue()
  {
    mIsRegistered = true;
    mSpace->RegisterEvent<&DaisyDialogue::OnDialogueStart>(Events::DialogueStart, this);
    mSpace->RegisterEvent<&DaisyDialogue::OnDialogueContinue>(Events::DialogueNodeConfirm, this);
    mSpace->RegisterEvent<&DaisyDialogue::OnDialogueExit>(Events::DialogueExit, this);
    mSpace->RegisterEvent<&DaisyDialogue::OnPlaySoundEvent>(Events::PlaySoundEvent, this);
    mSpace->RegisterEvent<&DaisyDialogue::OnPlayAnimationEvent>(Events::PlayAnimationEvent, this);
  }

  void DaisyDialogue::DeregisterDialogue()
  {
    mIsRegistered = false;
    mSpace->DeregisterEvent<&DaisyDialogue::OnDialogueStart>(Events::DialogueStart,  this);
    mSpace->DeregisterEvent<&DaisyDialogue::OnDialogueContinue>(Events::DialogueNodeConfirm,  this);
    mSpace->DeregisterEvent<&DaisyDialogue::OnDialogueExit>(Events::DialogueExit,  this);
    mSpace->DeregisterEvent<&DaisyDialogue::OnPlaySoundEvent>(Events::PlaySoundEvent,  this);
    mSpace->DeregisterEvent<&DaisyDialogue::OnPlayAnimationEvent>(Events::PlayAnimationEvent,  this);
  }

  void DaisyDialogue::OnCollisionStarted(CollisionStarted *aEvent)
  {
    if (aEvent->OtherObject->GetComponent<BoatController>() != nullptr)
    {
      // want all the music playing if the overlap
      if (mSoundEmitter)
      {
        mSoundEmitter->PlayEvent("FD_CallOut");
      }

      if (mActiveQuest->GetName() != Quest::Name::Introduction)
      {
        if (mSoundEmitter)
        {
          mSoundEmitter->PlayEvent("UI_Dia_Start");
        }

        RegisterDialogue();

        DialoguePossible diagEvent;
        diagEvent.isPossible = true;
        mSpace->SendEvent(Events::DialoguePossible, &diagEvent);
      }
      else
      {
        if (mIsRegistered)
        {
          DialoguePossible diagEvent;
          diagEvent.isPossible = true;
          mSpace->SendEvent(Events::DialoguePossible, &diagEvent);
        }
      }
    }
  }

  void DaisyDialogue::OnCollisionEnded(CollisionEnded *aEvent)
  {
    if (aEvent->OtherObject->GetComponent<BoatController>() != nullptr)
    {
      if (mActiveQuest->GetName() != Quest::Name::Introduction)
      {
        DeregisterDialogue();
      }

      DialoguePossible diagEvent;
      diagEvent.isPossible = false;
      mSpace->SendEvent(Events::DialoguePossible, &diagEvent);
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
    UnusedArguments(aEvent);

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
      DialoguePossible diagEvent;
      diagEvent.isPossible = false;
      mSpace->SendEvent(Events::DialoguePossible, &diagEvent);

      DialogueNodeReady next(mActiveNode->GetNodeData());
      next.DialogueType = type;
      next.DialogueCameraAnchor = mCameraAnchor;
      next.DialogueLambAnchor = mLambAnchor;
      mSpace->SendEvent(Events::DialogueNodeReady, &next);
    }
  }

  void DaisyDialogue::OnDialogueExit(DialogueExit *aEvent)
  {
    UnusedArguments(aEvent);

    DialoguePossible diagEvent;
    diagEvent.isPossible = true;
    mSpace->SendEvent(Events::DialoguePossible, &diagEvent);

    if (mActiveQuest->GetName() == Quest::Name::Introduction)
    {
      if (mActiveConvo->GetName() == Conversation::Name::Hello)
      {
        //TutorialUpdate nextTutorial(Quest::CharacterName::Daisy);
        //mSpace->SendEvent(Events::TutorialUpdate, &nextTutorial);

        mActiveConvo = &mActiveQuest->GetConversations()->at(1);
        mActiveNode = mActiveConvo->GetRoot();

        ++mConvosIter;
        mLinesIter = mConvosIter->begin();

        // start our next dialogue
        RequestDialogueStart autostart;
        mSpace->SendEvent(Events::RequestDialogueStart, &autostart);
      }
      else
      {
        if (auto star = mSpace->FindFirstCompositionByName("FeedbackStar"))
        {
          star->GetComponent<StarMovement>()->SetAnchor(StarMovement::CurrentAnchor::Basil);
        }

        mActiveQuest->SetState(Quest::State::Completed);
        TutorialUpdate nextTutorial(Quest::CharacterName::Basil);
        mSpace->SendEvent(Events::TutorialUpdate, &nextTutorial);
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
          SpawnProgressionItem spawnItem(mName);
          mSpace->SendEvent(Events::SpawnProgressionItem, &spawnItem);
        }
        else if (mActiveQuest->GetName() == Quest::Name::Explore)
        {
          SpawnProgressionLocation spawnLocation(mName);
          mSpace->SendEvent(Events::SpawnProgressionLocation, &spawnLocation);
        }
        else if (mActiveQuest->GetName() == Quest::Name::Dialogue)
        {
          SpawnProgressionDialogue spawnTalk(mName);
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

        if (mSoundEmitter)
        {
          mSoundEmitter->PlayEvent("UI_Quest_Completed");
        }

        ProgressionHappened progEvent;
        mSpace->SendEvent(Events::ProgressionHappened, &progEvent);

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

  void DaisyDialogue::OnDialogueContinue(DialogueNodeConfirm *aEvent)
  {
    DialoguePossible diagEvent;
    diagEvent.isPossible = false;
    mSpace->SendEvent(Events::DialoguePossible, &diagEvent);

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
        next.DialogueCameraAnchor = mCameraAnchor;
        next.DialogueLambAnchor = mLambAnchor;
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

  void DaisyDialogue::OnUpdateActiveQuestState(UpdateActiveQuestState *aEvent)
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
        // if we are already in the NotActive quest, we dont want to do this
        if (mConvosIter != mDialogueConvos.end() - 1)
        {
          mPrevConvoIter = mConvosIter; // save our place for sound cues
          mConvosIter = mDialogueConvos.end() - 1; // NotActive quest will always come last, and only has a Hello convo
        }
        // we do need to reset the audio tho
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

          if (mSoundEmitter)
          {
            mSoundEmitter->PlayEvent("UI_Quest_Completed");
          }

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

  void DaisyDialogue::OnPlaySoundEvent(PlaySoundEvent *aEvent)
  {
    // special case for our branching
    if (mActiveQuest->GetName() == Quest::Name::Fetch && mActiveConvo->GetName() == Conversation::Name::Hello)
    {
      // just count what sound cue we are on, yup im resorting to this, brace yourself captain.
      ++mSoundCueCounter;
      if (!aEvent->isText)
      {
        // store how many sounds we will skip
        mSoundOptionChosen = aEvent->option;
        // go to the chosen sound option
        for (int i = 0; i < aEvent->option; ++i)
        {
          ++mLinesIter;
        }
        return;
      }

      if (mConvosIter != mDialogueConvos.end())
      {
        if (mLinesIter != mConvosIter->end())
        {
          std::cout << mLinesIter->first << std::endl;
          mSoundEmitter->PlayEvent(mLinesIter->second);
          if (mSoundCueCounter + mSoundOptionChosen == 5)
          {
            // +5 lines to skip
            for (int i = 0; i < 5; ++i)
            {
              ++mLinesIter;
            }
            return;
          }
          else if (mSoundCueCounter + mSoundOptionChosen == 6)
          {
            // +4 lines to skip
            for (int i = 0; i < 4; ++i)
            {
              ++mLinesIter;
            }
            return;
          }
          ++mLinesIter;
        }
      }
    }
    else
    {
      if (aEvent->isText)
      {
        if (mLinesIter != mConvosIter->end())
        {
          mSoundEmitter->PlayEvent(mLinesIter->second);
          ++mLinesIter;
        }
      }
    }
  }

  void DaisyDialogue::OnPlayAnimationEvent(PlayAnimationEvent *aEvent)
  {
    std::string anim = aEvent->animationName;

    mAnimator->PlayAnimationSet(anim);
  }

} //end yte