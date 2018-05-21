/******************************************************************************/
/*!
* \author Isaac Dayton
* \date   2018/03/15
*
* \copyright All content 2018 DigiPen (USA) Corporation, all rights reserved.
*/
/******************************************************************************/

#include "YTE/GameComponents/DialogueDirector.hpp"

#include "YTE/Physics/Orientation.hpp"

#include "YTE/Physics/Transform.hpp"

#include "YTE/GameComponents/CameraAnchor.hpp"
#include "YTE/GameComponents/DaisyDialogue.hpp"
#include "YTE/GameComponents/BasilDialogue.hpp"
#include "YTE/GameComponents/QuestLogic.hpp"

namespace YTE
{
  YTEDefineEvent(DialogueStart);
  YTEDefineType(DialogueStart)
  {
    RegisterType<DialogueStart>();
    TypeBuilder<DialogueStart> builder;
  }

  YTEDefineEvent(UISelectEvent);
  YTEDefineEvent(UIFocusSwitchEvent);
  YTEDefineEvent(UIDisplayEvent);
  YTEDefineEvent(UIUpdateContent);

  YTEDefineEvent(PlaySoundEvent);
  YTEDefineType(PlaySoundEvent) { RegisterType<PlaySoundEvent>();
    TypeBuilder<PlaySoundEvent> builder;  }

  YTEDefineType(UISelectEvent) { RegisterType<UISelectEvent>();
    TypeBuilder<UISelectEvent> builder; }
  YTEDefineType(UIFocusSwitchEvent) { RegisterType<UIFocusSwitchEvent>();
    TypeBuilder<UIFocusSwitchEvent> builder; }
  YTEDefineType(UIDisplayEvent) { RegisterType<UIDisplayEvent>();
    TypeBuilder<UIDisplayEvent> builder; }
  YTEDefineType(UIUpdateContent) { RegisterType<UIUpdateContent>();
    TypeBuilder<UIUpdateContent> builder; }


  YTEDefineType(DialogueDirector)
  {
    RegisterType<DialogueDirector>();
    TypeBuilder<DialogueDirector> builder;
  }


  DialogueDirector::DialogueDirector(Composition *aOwner, Space *aSpace, RSValue *aProperties)
    : Component(aOwner, aSpace)
    , mDialogueSpace(nullptr)
    , mCameraAnchor(nullptr)
    , mLambTransform(nullptr)
    , mLastSelectionIndex(-1)
    , mMaxSelectionIndex(-1)
  {
    DeserializeByType(aProperties, this, GetStaticType());
  }

  void DialogueDirector::Initialize()
  {
    mDialogueSpace = mSpace->AddChildSpace("MSR_Dialogue");

    for (auto const& [name, child] : mOwner->GetCompositions())
    {
      if (child->GetComponent<CameraAnchor>() != nullptr)
      {
        mCameraAnchor = child.get();
        break;
      }
    }

    if (Composition *lamb = mOwner->FindFirstCompositionByName("MainCharacter"))
    {
      mLambTransform = lamb->GetComponent<Transform>();
    }

    mOwner->RegisterEvent<&DialogueDirector::OnCollisionStart>(Events::CollisionStarted, this);
    mOwner->RegisterEvent<&DialogueDirector::OnCollisionEnd>(Events::CollisionEnded, this);
    mSpace->RegisterEvent<&DialogueDirector::OnTutorialUpdate>(Events::TutorialUpdate, this);
    mSpace->RegisterEvent<&DialogueDirector::OnPostcardUpdate>(Events::PostcardUpdate, this);
    mSpace->RegisterEvent<&DialogueDirector::OnNoticeBoardHookup>(Events::NoticeBoardHookup, this);
  }

  void DialogueDirector::OnRequestDialogueStart(RequestDialogueStart *)
  {
    BoatDockEvent dockEvent;
    mSpace->SendEvent(Events::BoatDockEvent, &dockEvent);

    DialogueStart startDialogue;
    mSpace->SendEvent(Events::DialogueStart, &startDialogue);

    auto emitter = mOwner->GetComponent<WWiseEmitter>();

    mSpace->GetComponent<InputInterpreter>()->SetInputContext(InputInterpreter::InputContext::Dialogue);
  }

  void DialogueDirector::OnDialogueNodeReady(DialogueNodeReady *aEvent)
  {
    if (mLambTransform && !mIsLambSet && aEvent->DialogueLambAnchor)
    {
      mLambTransform->SetWorldTranslation(aEvent->DialogueLambAnchor->GetWorldTranslation());
      mLambTransform->SetWorldRotation(aEvent->DialogueLambAnchor->GetWorldRotation());
      mLambTransform->SetScale(1.2f, 1.2f, 1.2f);

      mIsLambSet = true;
    }

    if (mCameraAnchor != aEvent->DialogueCameraAnchor)
    {
      mCameraAnchor = aEvent->DialogueCameraAnchor;

      if (mCameraAnchor)
      {
        DirectCameraEvent directCam;
        mCameraAnchor->SendEvent(Events::DirectCameraEvent, &directCam);
      }
    }

      // clear the data from the last node
    mCurNodeData.clear();
      // copy the node data, some nodes have multiple lines
    for (std::string line : aEvent->ContentMessages)
    {
      mCurNodeData.push_back(line);
    }
      // copy the node type to check when we get a confirm event from InputInterpreter
    mCurNodeType = aEvent->DialogueType;
    mCurNodeDataIndex = 0;

    if (mCurNodeType == DialogueNode::NodeType::Text)
    {
      PlaySoundEvent triggerSound(0, 0, true);
      mSpace->SendEvent(Events::PlaySoundEvent, &triggerSound);

      UIUpdateContent content(true, aEvent->ContentMessages[0]);
      mDialogueSpace->SendEvent(Events::UIUpdateContent, &content);

      UIDisplayEvent display(true);
      mDialogueSpace->SendEvent(Events::UIDisplayEvent, &display);

      UIFocusSwitchEvent passiveFocus(true);
      mDialogueSpace->SendEvent(Events::UIFocusSwitchEvent, &passiveFocus);
    }
    else if (mCurNodeType == DialogueNode::NodeType::Input)
    {
      int size = static_cast<int>(aEvent->ContentMessages.size());
      mMaxSelectionIndex = size - 1;

      for (int i = 0; i < size; ++i)
      {
        if (i == 0)
        {
          mLastSelectionIndex = 0;

          UISelectEvent select(mLastSelectionIndex);
          select.NumOptions = size;
          mDialogueSpace->SendEvent(Events::UISelectEvent, &select);

          /*UIDisplayEvent display(true);
          display.DisplayIndex = i;
          display.NumOptions = size;
          mDialogueSpace->SendEvent(Events::UIDisplayEvent, &display);*/
        }

        UIUpdateContent content(false, aEvent->ContentMessages[i]);
        content.SelectionIndex = i;
        mDialogueSpace->SendEvent(Events::UIUpdateContent, &content);

        UIFocusSwitchEvent passiveFocus(false);
        mDialogueSpace->SendEvent(Events::UIFocusSwitchEvent, &passiveFocus);
      }
    }
  }

  void DialogueDirector::OnDialogueSelect(DialogueSelect *aEvent)
  {
      // We dont look at selections if its just text, theoretically the elements will be hidden so they wont send select events?
    if (mCurNodeType == DialogueNode::NodeType::Text)
    {
      return;
    }

    if (aEvent->SelectionDirection == DialogueSelect::Direction::Next)
    {
      mLastSelectionIndex = (mLastSelectionIndex + 1) % (mMaxSelectionIndex + 1);
    }
    else if (aEvent->SelectionDirection == DialogueSelect::Direction::Prev)
    {
      mLastSelectionIndex = ((mLastSelectionIndex + mMaxSelectionIndex) % (mMaxSelectionIndex + 1));
    }

    auto emitter = mOwner->GetComponent<WWiseEmitter>();

    if (emitter)
    {
      emitter->PlayEvent("UI_Dia_Next"); // if we are skipping through quickly, stop existing sound
    }

    /*float stickAngle = glm::acos(glm::dot(glm::vec2(1.f, 0.f), aEvent->StickDirection));
    float pi = glm::pi<float>();
    float length = glm::length(aEvent->StickDirection);

    if (length > 0.6f)
    {
      if (stickAngle >= (pi / 3.0f) && stickAngle < (2.0f * pi / 3.0f) && aEvent->StickDirection.y > 0.0f)
      {
        if (mMaxSelectionIndex >= 0)
        {
          mLastSelectionIndex = 0;
        }
      }
      else if (stickAngle < (pi / 6.0f))
      {
        if (mMaxSelectionIndex >= 1)
        {
          mLastSelectionIndex = 1;
        }
      }
      else if (stickAngle >= (5.0f * pi / 6.0f))
      {
        if (mMaxSelectionIndex >= 2)
        {
          mLastSelectionIndex = 2;
        }
      }
    }
    else
    {
      mLastSelectionIndex = -1;
    }*/

    UISelectEvent select(mLastSelectionIndex);
    mDialogueSpace->SendEvent(Events::UISelectEvent, &select);
  }

  void DialogueDirector::OnDialogueConfirm(DialogueConfirm *)
  {
    auto emitter = mOwner->GetComponent<WWiseEmitter>();

    if (mCurNodeType == DialogueNode::NodeType::Text)
    {
      if (emitter)
      {
        emitter->PlayEvent("Dia_All_Stop"); // if we are skipping through quickly, stop existing sound
      }

        // If there are more strings in this node
      if (mCurNodeDataIndex < mCurNodeData.size() - 1)
      {
        PlaySoundEvent triggerSound(0, 0, true);
        mSpace->SendEvent(Events::PlaySoundEvent, &triggerSound);

        ++mCurNodeDataIndex;
        UIUpdateContent content(true, mCurNodeData[mCurNodeDataIndex]);
        mDialogueSpace->SendEvent(Events::UIUpdateContent, &content);
      }
        // If this is the last string in the node
      else if (mCurNodeDataIndex == mCurNodeData.size() - 1)
      {
        DialogueNodeConfirm confirm(0);
        mSpace->SendEvent(Events::DialogueNodeConfirm, &confirm);
      }
    }
    else if (mCurNodeType == DialogueNode::NodeType::Input)
    {
      if (mLastSelectionIndex > -1)
      {
        // this just passes info to the Dialogue for how we are traversing the sound vec
        PlaySoundEvent triggerSound((int)mCurNodeData.size(), mLastSelectionIndex, false);
        mSpace->SendEvent(Events::PlaySoundEvent, &triggerSound);

        DialogueNodeConfirm confirm(mLastSelectionIndex);
        mSpace->SendEvent(Events::DialogueNodeConfirm, &confirm);

          // reset our options
        mLastSelectionIndex = -1;
        UISelectEvent select(mLastSelectionIndex);
        mDialogueSpace->SendEvent(Events::UISelectEvent, &select);
      }
    }

    if (emitter)
    {
      emitter->PlayEvent("UI_Dia_Next");
    }
  }

  void DialogueDirector::OnDialogueExit(DialogueExit *)
  {
    mCameraAnchor = nullptr;

    if (mLambTransform)
    {
      mLambTransform->SetTranslation(0.0f, 0.0f, 0.0f);
      mLambTransform->SetRotation(0.0f, 0.0f, 0.0f);
      mLambTransform->SetScale(1.0f, 1.0f, 1.0f);

      mIsLambSet = false;
    }

    UISelectEvent select(-1);
    mDialogueSpace->SendEvent(Events::UISelectEvent, &select);

    UIDisplayEvent shouldDisplay(false);
    mDialogueSpace->SendEvent(Events::UIDisplayEvent, &shouldDisplay);

    auto emitter = mOwner->GetComponent<WWiseEmitter>();

    if (emitter)
    {
      emitter->PlayEvent("UI_Dia_End");
    }

    mSpace->GetComponent<InputInterpreter>()->SetInputContext(InputInterpreter::InputContext::Sailing);
  }

  void DialogueDirector::OnCollisionStart(CollisionStarted * aEvent)
  {
    if (!mIsRegistered)
    {
      if (mIsTutorial)
      {
        if (mTutorialRegisteredCharacter == Quest::CharacterName::John)
        {
          if (aEvent->OtherObject->GetComponent<JohnDialogue>() != nullptr)
          {
            RegisterDirector();
          }
        }
        else if (mTutorialRegisteredCharacter == Quest::CharacterName::Daisy)
        {
          if (aEvent->OtherObject->GetComponent<DaisyDialogue>() != nullptr)
          {
            RegisterDirector();
          }
        }
        // since basil is our last tutorial we have to check when we've gone through the conversation too
        else if (mTutorialRegisteredCharacter == Quest::CharacterName::Basil && (*mBasilQuestHandle)->GetState() != Quest::State::Completed)
        {
          if (aEvent->OtherObject->GetComponent<BasilDialogue>() != nullptr)
          {
            RegisterDirector();
          }
        }
      }
      // if we arent in tutorial we dont have to worry about multi collision
      else
      {
        if (aEvent->OtherObject->GetComponent<JohnDialogue>() || aEvent->OtherObject->GetComponent<DaisyDialogue>() || aEvent->OtherObject->GetComponent<BasilDialogue>())
        {
          RegisterDirector();
        }
      }
    }
  }

  void DialogueDirector::OnCollisionEnd(CollisionEnded * aEvent)
  {
    if (mIsRegistered)
    {
      if (mIsTutorial)
      {
        if (mTutorialRegisteredCharacter == Quest::CharacterName::John)
        {
          if (aEvent->OtherObject->GetComponent<JohnDialogue>() != nullptr)
          {
            DeregisterDirector();
          }
        }
        else if (mTutorialRegisteredCharacter == Quest::CharacterName::Daisy)
        {
          if (aEvent->OtherObject->GetComponent<DaisyDialogue>() != nullptr)
          {
            DeregisterDirector();
          }
        }
        else if (mTutorialRegisteredCharacter == Quest::CharacterName::Basil)
        {
          if (aEvent->OtherObject->GetComponent<BasilDialogue>() != nullptr)
          {
            DeregisterDirector();
          }
        }
      }
      // if we arent in the tutorial we dont need to worry brub
      else
      {
        if (aEvent->OtherObject->GetComponent<JohnDialogue>() || aEvent->OtherObject->GetComponent<DaisyDialogue>() || aEvent->OtherObject->GetComponent<BasilDialogue>())
        {
          DeregisterDirector();
        }
      }
    }
  }

  void DialogueDirector::OnNoticeBoardHookup(NoticeBoardHookup *aEvent)
  {
    switch ((*aEvent->mActiveQuestHandle)->GetCharacter())
    {
      case Quest::CharacterName::John:
      {
        mJohnQuestHandle = aEvent->mActiveQuestHandle;
        break;
      }
      case Quest::CharacterName::Daisy:
      {
        mDaisyQuestHandle = aEvent->mActiveQuestHandle;
        break;
      }
      case Quest::CharacterName::Basil:
      {
        mBasilQuestHandle = aEvent->mActiveQuestHandle;
        break;
      }
    }
  }

  void DialogueDirector::OnTutorialUpdate(TutorialUpdate *aEvent)
  {
    mTutorialRegisteredCharacter = aEvent->mCharacter;
    DeregisterDirector();
  }

  // lol check em
  void DialogueDirector::OnPostcardUpdate(PostcardUpdate *)
  {
    mIsTutorial = false;
  }

  void DialogueDirector::RegisterDirector()
  {
    mIsRegistered = true;
    mSpace->RegisterEvent<&DialogueDirector::OnRequestDialogueStart>(Events::RequestDialogueStart, this);
    mSpace->RegisterEvent<&DialogueDirector::OnDialogueNodeReady>(Events::DialogueNodeReady, this);
    mSpace->RegisterEvent<&DialogueDirector::OnDialogueSelect>(Events::DialogueSelect, this);
    mSpace->RegisterEvent<&DialogueDirector::OnDialogueConfirm>(Events::DialogueConfirm, this);
    mSpace->RegisterEvent<&DialogueDirector::OnDialogueExit>(Events::DialogueExit, this);
  }

  void DialogueDirector::DeregisterDirector()
  {
    mIsRegistered = false;
    mSpace->DeregisterEvent<&DialogueDirector::OnRequestDialogueStart>(Events::RequestDialogueStart,  this);
    mSpace->DeregisterEvent<&DialogueDirector::OnDialogueNodeReady>(Events::DialogueNodeReady,  this);
    mSpace->DeregisterEvent<&DialogueDirector::OnDialogueSelect>(Events::DialogueSelect,  this);
    mSpace->DeregisterEvent<&DialogueDirector::OnDialogueConfirm>(Events::DialogueConfirm,  this);
    mSpace->DeregisterEvent<&DialogueDirector::OnDialogueExit>(Events::DialogueExit,  this);
  }

}
