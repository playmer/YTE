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
#include "YTE/GameComponents/CameraAnchor.hpp"
#include "YTE/Graphics/Animation.hpp"
#include "YTE/GameComponents/StarMovement.hpp"
#include "YTE/GameComponents/ProgressionParticles.hpp"

namespace YTE
{
  YTEDefineType(BasilDialogue) { RegisterType<BasilDialogue>();
    TypeBuilder<BasilDialogue> builder; }

  BasilDialogue::BasilDialogue(Composition *aOwner, Space *aSpace, RSValue *aProperties)
    : Component(aOwner, aSpace)
    , mSoundEmitter(nullptr)
    , mSoundSystem(nullptr)
    , mAnimator(nullptr)
    , mCameraAnchor(nullptr)
    , mLambAnchor(nullptr)
  {
    UnusedArguments(aProperties);

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
    mOwner->RegisterEvent<&BasilDialogue::OnCollisionStarted>(Events::CollisionStarted, this);
    mOwner->RegisterEvent<&BasilDialogue::OnCollisionEnded>(Events::CollisionEnded, this);
    mSpace->RegisterEvent<&BasilDialogue::OnQuestStart>(Events::QuestStart, this);
    mSpace->RegisterEvent<&BasilDialogue::OnUpdateActiveQuestState>(Events::UpdateActiveQuestState, this);

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
         std::make_pair("BB_I_H_1", mSoundSystem->GetSoundIDFromString("BB_I_H_1")),
         std::make_pair("BB_I_H_2", mSoundSystem->GetSoundIDFromString("BB_I_H_2")),
         std::make_pair("BB_I_H_3", mSoundSystem->GetSoundIDFromString("BB_I_H_3")),
         std::make_pair("BB_I_H_4", mSoundSystem->GetSoundIDFromString("BB_I_H_4")),
         std::make_pair("BB_I_H_5", mSoundSystem->GetSoundIDFromString("BB_I_H_5")),
         std::make_pair("BB_I_H_6", mSoundSystem->GetSoundIDFromString("BB_I_H_6")),
         std::make_pair("BB_I_H_7", mSoundSystem->GetSoundIDFromString("BB_I_H_7")),
         std::make_pair("BB_I_H_8", mSoundSystem->GetSoundIDFromString("BB_I_H_8"))
      });
      // G                                                             
      mDialogueConvos.emplace_back(std::map<std::string, u64>
      {
        std::make_pair("BB_I_G_1", mSoundSystem->GetSoundIDFromString("BB_I_G_1")),
        std::make_pair("BB_I_G_2", mSoundSystem->GetSoundIDFromString("BB_I_G_2"))
      });

      // FB ////////////////////////////
      // H
      mDialogueConvos.emplace_back(std::map<std::string, u64>
      {
        std::make_pair("BB_FB_H_1", mSoundSystem->GetSoundIDFromString("BB_FB_H_1")),
        std::make_pair("BB_FB_H_2", mSoundSystem->GetSoundIDFromString("BB_FB_H_2")),
        std::make_pair("BB_FB_H_3", mSoundSystem->GetSoundIDFromString("BB_FB_H_3")),
        std::make_pair("BB_FB_H_4", mSoundSystem->GetSoundIDFromString("BB_FB_H_4")),
        std::make_pair("BB_FB_H_5", mSoundSystem->GetSoundIDFromString("BB_FB_H_5")),
        std::make_pair("BB_FB_H_6", mSoundSystem->GetSoundIDFromString("BB_FB_H_6"))
      });
      // NP           
      mDialogueConvos.emplace_back(std::map<std::string, u64>
      {
        std::make_pair("BB_FB_NP_1", mSoundSystem->GetSoundIDFromString("BB_FB_NP_1"))
      });
      // C                                                              
      mDialogueConvos.emplace_back(std::map<std::string, u64>
      {
        std::make_pair("BB_FB_C_1", mSoundSystem->GetSoundIDFromString("BB_FB_C_1")),
        std::make_pair("BB_FB_C_2", mSoundSystem->GetSoundIDFromString("BB_FB_C_2")),
        std::make_pair("BB_FB_C_3", mSoundSystem->GetSoundIDFromString("BB_FB_C_3")),
        std::make_pair("BB_FB_C_4", mSoundSystem->GetSoundIDFromString("BB_FB_C_4")),
        std::make_pair("BB_FB_C_5", mSoundSystem->GetSoundIDFromString("BB_FB_C_5"))
      });
      // G
      mDialogueConvos.emplace_back(std::map<std::string, u64>
      {
         std::make_pair("BB_FB_G_1", mSoundSystem->GetSoundIDFromString("BB_FB_G_1")),
         std::make_pair("BB_FB_G_2", mSoundSystem->GetSoundIDFromString("BB_FB_G_2"))
      });

      // YM //////////////////////////////////
      // H
      mDialogueConvos.emplace_back(std::map<std::string, u64>
      {
        std::make_pair("BB_YM_H_1", mSoundSystem->GetSoundIDFromString("BB_YM_H_1")),
        std::make_pair("BB_YM_H_2", mSoundSystem->GetSoundIDFromString("BB_YM_H_2")),
        std::make_pair("BB_YM_H_3", mSoundSystem->GetSoundIDFromString("BB_YM_H_3")),
        std::make_pair("BB_YM_H_4", mSoundSystem->GetSoundIDFromString("BB_YM_H_4"))
      });
      // NP           
      mDialogueConvos.emplace_back(std::map<std::string, u64>
      {
         std::make_pair("BB_YM_NP_1", mSoundSystem->GetSoundIDFromString("BB_YM_NP_1")),
         std::make_pair("BB_YM_NP_2", mSoundSystem->GetSoundIDFromString("BB_YM_NP_2"))
      });
      // C                                                              
      mDialogueConvos.emplace_back(std::map<std::string, u64>
      {
         std::make_pair("BB_YM_C_1", mSoundSystem->GetSoundIDFromString("BB_YM_C_1")),
         std::make_pair("BB_YM_C_2", mSoundSystem->GetSoundIDFromString("BB_YM_C_2")),
         std::make_pair("BB_YM_C_3", mSoundSystem->GetSoundIDFromString("BB_YM_C_3")),
      });
      // G
      mDialogueConvos.emplace_back(std::map<std::string, u64>
      {
        std::make_pair("BB_YM_G_1", mSoundSystem->GetSoundIDFromString("BB_YM_G_1"))
      });

      // MH //////////////////////////////////
      // H
      mDialogueConvos.emplace_back(std::map<std::string, u64>
      {
        std::make_pair("BB_MH_H_1", mSoundSystem->GetSoundIDFromString("BB_MH_H_1")),
        std::make_pair("BB_MH_H_2", mSoundSystem->GetSoundIDFromString("BB_MH_H_2")),
        std::make_pair("BB_MH_H_3", mSoundSystem->GetSoundIDFromString("BB_MH_H_3")),
        std::make_pair("BB_MH_H_4", mSoundSystem->GetSoundIDFromString("BB_MH_H_4")),
        std::make_pair("BB_MH_H_5", mSoundSystem->GetSoundIDFromString("BB_MH_H_5")),
        std::make_pair("BB_MH_H_6", mSoundSystem->GetSoundIDFromString("BB_MH_H_6")),
        std::make_pair("BB_MH_H_7", mSoundSystem->GetSoundIDFromString("BB_MH_H_7"))
      });
      // NP           
      mDialogueConvos.emplace_back(std::map<std::string, u64>
      {
         std::make_pair("BB_MH_NP_1", mSoundSystem->GetSoundIDFromString("BB_MH_NP_1")),
         std::make_pair("BB_MH_NP_2", mSoundSystem->GetSoundIDFromString("BB_MH_NP_2"))
      });
      // C                                                              
      mDialogueConvos.emplace_back(std::map<std::string, u64>
      {
        std::make_pair("BB_MH_C_1", mSoundSystem->GetSoundIDFromString("BB_MH_C_1")),
        std::make_pair("BB_MH_C_2", mSoundSystem->GetSoundIDFromString("BB_MH_C_2")),
        std::make_pair("BB_MH_C_3", mSoundSystem->GetSoundIDFromString("BB_MH_C_3")),
        std::make_pair("BB_MH_C_4", mSoundSystem->GetSoundIDFromString("BB_MH_C_4"))
      });
      // G
      mDialogueConvos.emplace_back(std::map<std::string, u64>
      {
        std::make_pair("BB_MH_G_1", mSoundSystem->GetSoundIDFromString("BB_MH_G_1")),
        std::make_pair("BB_MH_G_2", mSoundSystem->GetSoundIDFromString("BB_MH_G_2")),
        std::make_pair("BB_MH_G_3", mSoundSystem->GetSoundIDFromString("BB_MH_G_3"))
      });

      // NQ //////////////////////////////////
      // H
      mDialogueConvos.emplace_back(std::map<std::string, u64>
      {
        std::make_pair("BB_NQ_H_1", mSoundSystem->GetSoundIDFromString("BB_NQ_H_1")),
        std::make_pair("BB_NQ_H_2", mSoundSystem->GetSoundIDFromString("BB_NQ_H_2")),
        std::make_pair("BB_NQ_H_3", mSoundSystem->GetSoundIDFromString("BB_NQ_H_3")),
        std::make_pair("BB_NQ_H_4", mSoundSystem->GetSoundIDFromString("BB_NQ_H_4")),
        std::make_pair("BB_NQ_H_5", mSoundSystem->GetSoundIDFromString("BB_NQ_H_5"))
      });

      mConvosIter = mDialogueConvos.begin();
      mPrevConvoIter = mConvosIter;
      mLinesIter = mConvosIter->begin();
    }
  }

  void BasilDialogue::RegisterDialogue()
  {
    mIsRegistered = true;
    mSpace->RegisterEvent<&BasilDialogue::OnDialogueStart>(Events::DialogueStart, this);
    mSpace->RegisterEvent<&BasilDialogue::OnDialogueContinue>(Events::DialogueNodeConfirm, this);
    mSpace->RegisterEvent<&BasilDialogue::OnDialogueExit>(Events::DialogueExit, this);
    mSpace->RegisterEvent<&BasilDialogue::OnPlaySoundEvent>(Events::PlaySoundEvent, this);
    mSpace->RegisterEvent<&BasilDialogue::OnPlayAnimationEvent>(Events::PlayAnimationEvent, this);
  }

  void BasilDialogue::DeregisterDialogue()
  {
    mIsRegistered = false;
    mSpace->DeregisterEvent<&BasilDialogue::OnDialogueStart>(Events::DialogueStart,  this);
    mSpace->DeregisterEvent<&BasilDialogue::OnDialogueContinue>(Events::DialogueNodeConfirm,  this);
    mSpace->DeregisterEvent<&BasilDialogue::OnDialogueExit>(Events::DialogueExit,  this);
    mSpace->DeregisterEvent<&BasilDialogue::OnPlaySoundEvent>(Events::PlaySoundEvent,  this);
    mSpace->DeregisterEvent<&BasilDialogue::OnPlayAnimationEvent>(Events::PlayAnimationEvent,  this);
  }

  void BasilDialogue::OnCollisionStarted(CollisionStarted *aEvent)
  {
    if (aEvent->OtherObject->GetComponent<BoatController>() != nullptr)
    {
      // want all the music playing if the overlap
      if (mSoundEmitter)
      {
        mSoundEmitter->PlayEvent("BB_CallOut");
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

  void BasilDialogue::OnCollisionEnded(CollisionEnded *aEvent)
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

  void BasilDialogue::Start()
  {
      // Send to the space a ptr to the activequest for the noticeboard
    NoticeBoardHookup firstQuest(&mActiveQuest);
    mSpace->SendEvent(Events::NoticeBoardHookup, &firstQuest);
  }

  void BasilDialogue::OnDialogueStart(DialogueStart *aEvent)
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

  void BasilDialogue::OnDialogueExit(DialogueExit *aEvent)
  {
    UnusedArguments(aEvent);
    mSoundBranchAccumulator = 0; // reset the conversation skip number

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

        //start the next dialogue
        RequestDialogueStart autostart;
        mSpace->SendEvent(Events::RequestDialogueStart, &autostart);
      }
      else
      {
        if (auto star = mSpace->FindFirstCompositionByName("FeedbackStar"))
        {
          star->GetComponent<StarMovement>()->SetAnchor(StarMovement::CurrentAnchor::NoticeBoard);
        }

        mActiveQuest->SetState(Quest::State::Completed);
        DeregisterDialogue();

        auto director = mSpace->FindFirstCompositionByName("Boat")->GetComponent<DialogueDirector>();
        director->DeregisterDirector();

        DialoguePossible diagEventNested;
        diagEvent.isPossible = false;
        mSpace->SendEvent(Events::DialoguePossible, &diagEventNested);
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

  void BasilDialogue::OnDialogueContinue(DialogueNodeConfirm *aEvent)
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

  void BasilDialogue::OnQuestStart(QuestStart *aEvent)
  {
    if (aEvent->mCharacter == mName)
    {
      // if we are returning from being inactive, load back our place for sound cues
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
      // in the callback we save our place for sound
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

  void BasilDialogue::OnPlaySoundEvent(PlaySoundEvent *aEvent)
  {
    if (mActiveQuest->GetName() == Quest::Name::Fetch && mActiveConvo->GetName() == Conversation::Name::Hello)
    {
      // if its an Input node
      if (!aEvent->isText)
      {
        // store how many sounds we will skip from now on
        mSoundBranchAccumulator = aEvent->size - 1;
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
          mSoundEmitter->PlayEvent(mLinesIter->second);
          for (int i = 0; i <= mSoundBranchAccumulator; ++i)
          {
            ++mLinesIter;
          }
        }
      }
    }
    else if (mActiveQuest->GetName() == Quest::Name::Fetch && mActiveConvo->GetName() == Conversation::Name::Completed)
    {
      // just count what sound cue we are on, yup im resorting to this, brace yourself captain.
      ++mSoundCueCounter;
      // if its an Input node
      if (!aEvent->isText)
      {
        // store how many sounds we will skip from now on
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
          mSoundEmitter->PlayEvent(mLinesIter->second);
          if (mSoundCueCounter + mSoundOptionChosen == 3)
          {
            // +3 lines to skip
            for (int i = 0; i < 3; ++i)
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

  void BasilDialogue::OnPlayAnimationEvent(PlayAnimationEvent *aEvent)
  {
    std::string anim = aEvent->animationName;

    mAnimator->PlayAnimationSet(anim);
  }

} //end yte