/******************************************************************************/
/*!
\file   NoticeBoard.cpp
\author Jonathan Ackerman
\par    email: jonathan.ackerman\@digipen.edu
\date   2018-03-25

All content (c) 2016 DigiPen  (USA) Corporation, all rights reserved.
*/
/******************************************************************************/

#include "YTE/Core/Actions/Action.hpp"
#include "YTE/Core/Actions/ActionManager.hpp"

#include "YTE/Graphics/Animation.hpp"
#include "YTE/Graphics/Model.hpp"

#include "YTE/GameComponents/NoticeBoard.hpp"
#include "YTE/GameComponents/QuestLogic.hpp" /* to identify collision with boat */

#include "YTE/GameComponents/HudController.hpp"
#include "YTE/GameComponents/ProgressionParticles.hpp"

#include "YTE/GameComponents/Menu/LaunchCredits.hpp"

namespace YTE
{
  YTEDefineEvent(NoticeBoardHookup);

  YTEDefineType(NoticeBoardHookup) { YTERegisterType(NoticeBoardHookup); }

  YTEDefineType(NoticeBoard) { YTERegisterType(NoticeBoard); }

  NoticeBoard::NoticeBoard(Composition *aOwner, Space *aSpace, RSValue *aProperties)
    : Component(aOwner, aSpace)
    , mEndSequencePlaying(false)
    , mAssignedPostcard(nullptr)
    , mCameraTransform(nullptr)
    , mCameraInitPos(glm::vec3(0))
    , mCameraInitRot(glm::vec3(0))
    , mCameraPosition(0.0f)
    , mCameraRotation(0.0f)
    , mCameraRotationPrev(0.0f)
    , mFakeLerp(0.0f)
  {
    YTEUnusedArgument(aProperties);
      // these are gonna be a tutorial, not postcards
    //mPostcardVec.emplace_back(Quest::CharacterName::Daisy, Quest::Name::Introduction);
    //mPostcardVec.emplace_back(Quest::CharacterName::Basil, Quest::Name::Introduction);

      // this is what determines the player's quest order
    mPostcardVec.emplace_back(Quest::CharacterName::Basil, Quest::Name::Introduction); // this is our tutorial postcard
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

    mSpace->YTERegister(Events::LogicUpdate, this, &NoticeBoard::Update);
    
    mAssignedPostcard = &mPostcardVec[0];

    if (auto camera = mSpace->FindFirstCompositionByName("Camera"); camera)
    {
      mCameraTransform = camera->GetComponent<Transform>();
    }

    mEndJohn = mSpace->FindFirstCompositionByName("EndingJohn");
    mEndDaisy = mSpace->FindFirstCompositionByName("EndingDaisy");
    mEndBasil = mSpace->FindFirstCompositionByName("EndingBasil");
    mEndLamb = mSpace->FindFirstCompositionByName("EndingLamb");
    mEndChefBoat = mSpace->FindFirstCompositionByName("EndingChefBoat");
    mEndDaisyBoat = mSpace->FindFirstCompositionByName("EndingGardenerBoat");
  }

  void NoticeBoard::Update(LogicUpdate *aEvent)
  {
    // just for lerping the camera at the end
    if (mEndSequencePlaying && mCameraTransform)
    {
      mCameraTransform->SetWorldTranslation(mCurrentCamPos);
      mCameraTransform->SetWorldRotation(mCurrentCamRot);

      //float angle = mCameraRotation - mCameraRotationPrev;
      //
      //mCameraTransform->RotateAboutLocalAxis(glm::vec3(1, 0, 0), angle);
      //
      //mCameraRotationPrev = mCameraRotation;
    }
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

  void NoticeBoard::TriggerEndingSequence()
  {
    mEndSequencePlaying = true;

    if (mCameraTransform)
    {
      mCameraTransform->SetWorldRotation(glm::vec3(0, 0, 0));
      mCameraInitPos = mCameraTransform->GetWorldTranslation();
      mCameraInitRot = mCameraTransform->GetWorldRotationAsEuler();
    }

    auto actionManager = mSpace->GetComponent<ActionManager>();


    // find all the anchors
    auto camAnch1 = mSpace->FindFirstCompositionByName("EndingCameraAnchor1");
    auto camAnch1_transform = camAnch1->GetComponent<Transform>();

    auto camAnch2 = mSpace->FindFirstCompositionByName("EndingCameraAnchor2");
    auto camAnch2_transform = camAnch2->GetComponent<Transform>();
    
    auto camAnch3 = mSpace->FindFirstCompositionByName("EndingCameraAnchor3");
    auto camAnch3_transform = camAnch3->GetComponent<Transform>();
    
    auto camAnch4 = mSpace->FindFirstCompositionByName("EndingCameraAnchor4");
    auto camAnch4_transform = camAnch4->GetComponent<Transform>();
    
    auto camAnch5 = mSpace->FindFirstCompositionByName("EndingCameraAnchor5");
    auto camAnch5_transform = camAnch5->GetComponent<Transform>();
    
    auto camAnch6 = mSpace->FindFirstCompositionByName("EndingCameraAnchor6");
    auto camAnch6_transform = camAnch6->GetComponent<Transform>();
    
    auto camAnch7 = mSpace->FindFirstCompositionByName("EndingCameraAnchor7");
    auto camAnch7_transform = camAnch7->GetComponent<Transform>();
    
    auto camAnch8 = mSpace->FindFirstCompositionByName("EndingCameraAnchor8");
    auto camAnch8_transform = camAnch8->GetComponent<Transform>();
    
    auto camAnch9 = mSpace->FindFirstCompositionByName("EndingCameraAnchor9");
    auto camAnch9_transform = camAnch9->GetComponent<Transform>();
    
    auto camAnch10 = mSpace->FindFirstCompositionByName("EndingCameraAnchor10");
    auto camAnch10_transform = camAnch10->GetComponent<Transform>();

    ActionSequence actionSequence;

    mTargetCamPos = camAnch1_transform->GetWorldTranslation();
    mTargetCamRot = camAnch1_transform->GetWorldRotationAsEuler();
    mCurrentCamPos = mTargetCamPos;
    mCurrentCamRot = mTargetCamRot;

    mTargetCamPos = camAnch2_transform->GetWorldTranslation();
    mTargetCamRot = camAnch2_transform->GetWorldRotationAsEuler();
    ActionGroup anchor2;
    anchor2.Add<Linear::easeNone>(mCurrentCamPos.x, mTargetCamPos.x, 5.0f);
    anchor2.Add<Linear::easeNone>(mCurrentCamPos.y, mTargetCamPos.y, 5.0f);
    anchor2.Add<Linear::easeNone>(mCurrentCamPos.z, mTargetCamPos.z, 5.0f);
    anchor2.Add<Linear::easeNone>(mCurrentCamRot.x, mTargetCamRot.x, 5.0f);
    anchor2.Add<Linear::easeNone>(mCurrentCamRot.y, mTargetCamRot.y, 5.0f);
    anchor2.Add<Linear::easeNone>(mCurrentCamRot.z, mTargetCamRot.z, 5.0f);

    anchor2.Call([this]() {

      mEndJohn->GetComponent<Animator>()->SetDefaultAnimation("Idle.fbx");
      mEndDaisy->GetComponent<Animator>()->SetDefaultAnimation("Idle.fbx");
      mEndBasil->GetComponent<Animator>()->SetDefaultAnimation("Idle.fbx");
      mEndLamb->GetComponent<Animator>()->SetDefaultAnimation("Idle.fbx");

      mEndJohn->GetComponent<Animator>()->SetCurrentAnimation("Idle.fbx");
      mEndDaisy->GetComponent<Animator>()->SetCurrentAnimation("Idle.fbx");
      mEndBasil->GetComponent<Animator>()->SetCurrentAnimation("Idle.fbx");
      mEndLamb->GetComponent<Animator>()->SetCurrentAnimation("Idle.fbx");

      mEndJohn->GetComponent<Model>()->SetVisibility(true);
      mEndDaisy->GetComponent<Model>()->SetVisibility(true);
      mEndBasil->GetComponent<Model>()->SetVisibility(true);
      mEndLamb->GetComponent<Model>()->SetVisibility(true);
      
      mEndChefBoat->GetComponent<Model>()->SetVisibility(true);
      mEndDaisyBoat->GetComponent<Model>()->SetVisibility(true);

      auto emitter = mOwner->GetComponent<WWiseEmitter>();

      if (emitter)
      {
        emitter->PlayEvent("Islands_Enter");
        emitter->PlayEvent("M_Dock_Enter");
        emitter->PlayEvent("M_Dasiy");
        emitter->PlayEvent("M_Basil");
        emitter->PlayEvent("M_John");
      }
    });


    mTargetCamPos = camAnch3_transform->GetWorldTranslation();
    mTargetCamRot = camAnch3_transform->GetWorldRotationAsEuler();
    ActionGroup anchor3;
    anchor3.Add<Linear::easeNone>(mCurrentCamPos.x, mTargetCamPos.x, 5.0f);
    anchor3.Add<Linear::easeNone>(mCurrentCamPos.y, mTargetCamPos.y, 5.0f);
    anchor3.Add<Linear::easeNone>(mCurrentCamPos.z, mTargetCamPos.z, 5.0f);
    anchor3.Add<Linear::easeNone>(mCurrentCamRot.x, mTargetCamRot.x, 5.0f);
    anchor3.Add<Linear::easeNone>(mCurrentCamRot.y, mTargetCamRot.y, 5.0f);
    anchor3.Add<Linear::easeNone>(mCurrentCamRot.z, mTargetCamRot.z, 5.0f);


    mTargetCamPos = camAnch4_transform->GetWorldTranslation();
    mTargetCamRot = camAnch4_transform->GetWorldRotationAsEuler();
    ActionGroup anchor4;
    anchor4.Add<Linear::easeNone>(mCurrentCamPos.x, mTargetCamPos.x, 5.0f);
    anchor4.Add<Linear::easeNone>(mCurrentCamPos.y, mTargetCamPos.y, 5.0f);
    anchor4.Add<Linear::easeNone>(mCurrentCamPos.z, mTargetCamPos.z, 5.0f);
    anchor4.Add<Linear::easeNone>(mCurrentCamRot.x, mTargetCamRot.x, 5.0f);
    anchor4.Add<Linear::easeNone>(mCurrentCamRot.y, mTargetCamRot.y, 5.0f);
    anchor4.Add<Linear::easeNone>(mCurrentCamRot.z, mTargetCamRot.z, 5.0f);


    mTargetCamPos = camAnch5_transform->GetWorldTranslation();
    mTargetCamRot = camAnch5_transform->GetWorldRotationAsEuler();
    ActionSequence anchorSeq5;
    ActionGroup anchor5;
    anchor5.Add<Linear::easeNone>(mCurrentCamPos.x, mTargetCamPos.x, 5.0f);
    anchor5.Add<Linear::easeNone>(mCurrentCamPos.y, mTargetCamPos.y, 5.0f);
    anchor5.Add<Linear::easeNone>(mCurrentCamPos.z, mTargetCamPos.z, 5.0f);
    anchor5.Add<Linear::easeNone>(mCurrentCamRot.x, mTargetCamRot.x, 5.0f);
    anchor5.Add<Linear::easeNone>(mCurrentCamRot.y, mTargetCamRot.y, 5.0f);
    anchor5.Add<Linear::easeNone>(mCurrentCamRot.z, mTargetCamRot.z, 5.0f);


    mTargetCamPos = camAnch6_transform->GetWorldTranslation();
    mTargetCamRot = camAnch6_transform->GetWorldRotationAsEuler();
    ActionSequence anchorSeq6;
    ActionGroup anchor6;
    anchor6.Add<Linear::easeNone>(mCurrentCamPos.x, mTargetCamPos.x, 5.0f);
    anchor6.Add<Linear::easeNone>(mCurrentCamPos.y, mTargetCamPos.y, 5.0f);
    anchor6.Add<Linear::easeNone>(mCurrentCamPos.z, mTargetCamPos.z, 5.0f);
    anchor6.Add<Linear::easeNone>(mCurrentCamRot.x, mTargetCamRot.x, 5.0f);
    anchor6.Add<Linear::easeNone>(mCurrentCamRot.y, mTargetCamRot.y, 5.0f);
    anchor6.Add<Linear::easeNone>(mCurrentCamRot.z, mTargetCamRot.z, 5.0f);
    

    mTargetCamPos = camAnch7_transform->GetWorldTranslation();
    mTargetCamRot = camAnch7_transform->GetWorldRotationAsEuler();
    ActionGroup anchor7;
    anchor7.Add<Linear::easeNone>(mCurrentCamPos.x, mTargetCamPos.x, 3.0f);
    anchor7.Add<Linear::easeNone>(mCurrentCamPos.y, mTargetCamPos.y, 3.0f);
    anchor7.Add<Linear::easeNone>(mCurrentCamPos.z, mTargetCamPos.z, 3.0f);
    anchor7.Add<Linear::easeNone>(mCurrentCamRot.x, mTargetCamRot.x, 3.0f);
    anchor7.Add<Linear::easeNone>(mCurrentCamRot.y, mTargetCamRot.y, 3.0f);
    anchor7.Add<Linear::easeNone>(mCurrentCamRot.z, mTargetCamRot.z, 3.0f);
    

    mTargetCamPos = camAnch8_transform->GetWorldTranslation();
    mTargetCamRot = camAnch8_transform->GetWorldRotationAsEuler();
    ActionGroup anchor8;
    anchor8.Add<Linear::easeNone>(mCurrentCamPos.x, mTargetCamPos.x, 2.0f);
    anchor8.Add<Linear::easeNone>(mCurrentCamPos.y, mTargetCamPos.y, 2.0f);
    anchor8.Add<Linear::easeNone>(mCurrentCamPos.z, mTargetCamPos.z, 2.0f);
    anchor8.Add<Linear::easeNone>(mCurrentCamRot.x, mTargetCamRot.x, 2.0f);
    anchor8.Add<Linear::easeNone>(mCurrentCamRot.y, mTargetCamRot.y, 2.0f);
    anchor8.Add<Linear::easeNone>(mCurrentCamRot.z, mTargetCamRot.z, 2.0f);
    

    mTargetCamPos = camAnch9_transform->GetWorldTranslation();
    mTargetCamRot = camAnch9_transform->GetWorldRotationAsEuler();
    ActionGroup anchor9;
    anchor9.Add<Linear::easeNone>(mCurrentCamPos.x, mTargetCamPos.x, 2.0f);
    anchor9.Add<Linear::easeNone>(mCurrentCamPos.y, mTargetCamPos.y, 2.0f);
    anchor9.Add<Linear::easeNone>(mCurrentCamPos.z, mTargetCamPos.z, 2.0f);
    anchor9.Add<Linear::easeNone>(mCurrentCamRot.x, mTargetCamRot.x, 2.0f);
    anchor9.Add<Linear::easeNone>(mCurrentCamRot.y, mTargetCamRot.y, 2.0f);
    anchor9.Add<Linear::easeNone>(mCurrentCamRot.z, mTargetCamRot.z, 2.0f);
    

    mTargetCamPos = camAnch10_transform->GetWorldTranslation();
    mTargetCamRot = camAnch10_transform->GetWorldRotationAsEuler();
    ActionGroup anchor10;
    anchor10.Add<Linear::easeNone>(mCurrentCamPos.x, mTargetCamPos.x, 3.0f);
    anchor10.Add<Linear::easeNone>(mCurrentCamPos.y, mTargetCamPos.y, 3.0f);
    anchor10.Add<Linear::easeNone>(mCurrentCamPos.z, mTargetCamPos.z, 3.0f);
    anchor10.Add<Linear::easeNone>(mCurrentCamRot.x, mTargetCamRot.x, 3.0f);
    anchor10.Add<Linear::easeNone>(mCurrentCamRot.y, mTargetCamRot.y, 3.0f);
    anchor10.Add<Linear::easeNone>(mCurrentCamRot.z, mTargetCamRot.z, 3.0f);

    
    ActionGroup fakeFinalAction;
    fakeFinalAction.Add<Linear::easeNone>(mFakeLerp, 10.0f, 3.0f);
    fakeFinalAction.Call([this]() {
      mEndJohn->GetComponent<Animator>()->SetCurrentAnimation("Wave_Init.fbx");
      mEndDaisy->GetComponent<Animator>()->SetCurrentAnimation("Wave_Init.fbx");
      mEndBasil->GetComponent<Animator>()->SetCurrentAnimation("Wave_Init.fbx");
      mEndLamb->GetComponent<Animator>()->SetCurrentAnimation("Wave_Init.fbx");
      
      mEndJohn->GetComponent<Animator>()->AddNextAnimation("Wave_Loop.fbx");
      mEndDaisy->GetComponent<Animator>()->AddNextAnimation("Wave_Loop.fbx");
      mEndBasil->GetComponent<Animator>()->AddNextAnimation("Wave_Loop.fbx");
      mEndLamb->GetComponent<Animator>()->AddNextAnimation("Wave_Loop.fbx");

      mEndJohn->GetComponent<Animator>()->AddNextAnimation("Wave_Loop.fbx");
      mEndDaisy->GetComponent<Animator>()->AddNextAnimation("Wave_Loop.fbx");
      mEndBasil->GetComponent<Animator>()->AddNextAnimation("Wave_Loop.fbx");
      mEndLamb->GetComponent<Animator>()->AddNextAnimation("Wave_Loop.fbx");

      mEndJohn->GetComponent<Animator>()->AddNextAnimation("Wave_Loop.fbx");
      mEndDaisy->GetComponent<Animator>()->AddNextAnimation("Wave_Loop.fbx");
      mEndBasil->GetComponent<Animator>()->AddNextAnimation("Wave_Loop.fbx");
      mEndLamb->GetComponent<Animator>()->AddNextAnimation("Wave_Loop.fbx");

      mEndJohn->GetComponent<Animator>()->AddNextAnimation("Wave_Loop.fbx");
      mEndDaisy->GetComponent<Animator>()->AddNextAnimation("Wave_Loop.fbx");
      mEndBasil->GetComponent<Animator>()->AddNextAnimation("Wave_Loop.fbx");
      mEndLamb->GetComponent<Animator>()->AddNextAnimation("Wave_Loop.fbx");

      mEndJohn->GetComponent<Animator>()->AddNextAnimation("Wave_Loop.fbx");
      mEndDaisy->GetComponent<Animator>()->AddNextAnimation("Wave_Loop.fbx");
      mEndBasil->GetComponent<Animator>()->AddNextAnimation("Wave_Loop.fbx");
      mEndLamb->GetComponent<Animator>()->AddNextAnimation("Wave_Loop.fbx");

      mEndJohn->GetComponent<Animator>()->AddNextAnimation("Wave_Loop.fbx");
      mEndDaisy->GetComponent<Animator>()->AddNextAnimation("Wave_Loop.fbx");
      mEndBasil->GetComponent<Animator>()->AddNextAnimation("Wave_Loop.fbx");
      mEndLamb->GetComponent<Animator>()->AddNextAnimation("Wave_Loop.fbx");

      mEndJohn->GetComponent<Animator>()->AddNextAnimation("Wave_Loop.fbx");
      mEndDaisy->GetComponent<Animator>()->AddNextAnimation("Wave_Loop.fbx");
      mEndBasil->GetComponent<Animator>()->AddNextAnimation("Wave_Loop.fbx");
      mEndLamb->GetComponent<Animator>()->AddNextAnimation("Wave_Loop.fbx");

      mEndJohn->GetComponent<Animator>()->AddNextAnimation("Wave_Loop.fbx");
      mEndDaisy->GetComponent<Animator>()->AddNextAnimation("Wave_Loop.fbx");
      mEndBasil->GetComponent<Animator>()->AddNextAnimation("Wave_Loop.fbx");
      mEndLamb->GetComponent<Animator>()->AddNextAnimation("Wave_Loop.fbx");

      mEndJohn->GetComponent<Animator>()->AddNextAnimation("Wave_Loop.fbx");
      mEndDaisy->GetComponent<Animator>()->AddNextAnimation("Wave_Loop.fbx");
      mEndBasil->GetComponent<Animator>()->AddNextAnimation("Wave_Loop.fbx");
      mEndLamb->GetComponent<Animator>()->AddNextAnimation("Wave_Loop.fbx");
    });

    ActionGroup fakeFinalAction2;
    fakeFinalAction2.Add<Linear::easeNone>(mFakeLerp, 20.0f, 2.0f);

    ActionGroup finalAction;
    finalAction.Add<Linear::easeNone>(mFakeLerp, 50.0f, 2.0f);

    finalAction.Call([this]() {
      
      StartCredits startCredits;

      auto creditSpace = mSpace->FindFirstCompositionByName("MSR_Credits");

      if (creditSpace)
      {
        creditSpace->SendEvent(Events::StartCredits, &startCredits);
      }
    });

    actionSequence.AddGroup(anchor2);
    actionSequence.AddGroup(anchor3);
    actionSequence.AddGroup(anchor4);
    actionSequence.AddGroup(anchor5);
    actionSequence.AddGroup(anchor6);
    actionSequence.AddGroup(anchor7);
    actionSequence.AddGroup(anchor8);
    actionSequence.AddGroup(anchor9);
    actionSequence.AddGroup(anchor10);
    actionSequence.AddGroup(fakeFinalAction);
    actionSequence.AddGroup(fakeFinalAction2);
    actionSequence.AddGroup(finalAction);

    actionManager->AddSequence(mOwner, actionSequence);
  }

  void NoticeBoard::OnRequestNoticeBoardStart(RequestNoticeBoardStart *aEvent)
  {
    YTEUnusedArgument(aEvent);

    if (mAssignedPostcard != nullptr)
    {
      Quest *curQuest = *(mActiveQuestMap.at(mAssignedPostcard->GetCharacter()));
      Quest::State curState = curQuest->GetState();

      // game is finished, trigger the ending
      if (mEndSequencePlaying == false &&
          mAssignedPostcard == &*(--mPostcardVec.end()) && 
          curState == Quest::State::Completed)
      {
        TriggerEndingSequence();
      }
      else if (curState == Quest::State::Completed)
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
              postcardEvent.Number = 1;
            }
            else if (questName == Quest::Name::Dialogue)
            {
              // quest 4
              postcardEvent.Number = 4;
            }
            else if (questName == Quest::Name::Explore)
            {
              // quest 7
              postcardEvent.Number = 7;
            }
          }
          else if (charName == Quest::CharacterName::Daisy)
          {
            if (questName == Quest::Name::Fetch)
            {
              // daisy 8
              postcardEvent.Number = 8;
            }
            else if (questName == Quest::Name::Dialogue)
            {
              // quest 6
              postcardEvent.Number = 6;
            }
            else if (questName == Quest::Name::Explore)
            {
              // quest 2
              postcardEvent.Number = 2;
            }
          }
          else if (charName == Quest::CharacterName::Basil)
          {
            if (questName == Quest::Name::Introduction)
            {
              // tutorial
              postcardEvent.Number = 0;
            }
            else if (questName == Quest::Name::Fetch)
            {
              // quest 3
              postcardEvent.Number = 3;
            }
            else if (questName == Quest::Name::Dialogue)
            {
              // quest 9
              postcardEvent.Number = 9;
            }
            else if (questName == Quest::Name::Explore)
            {
              // quest 5
              postcardEvent.Number = 5;
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