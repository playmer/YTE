/******************************************************************************/
/*!
* \author Isaac Dayton
* \date   2018/03/15
*
* \copyright All content 2018 DigiPen (USA) Corporation, all rights reserved.
*/
/******************************************************************************/

#include "YTE/GameComponents/CameraAnchor.hpp"

#include "YTE/Physics/Orientation.hpp"

#include "YTE/Physics/Transform.hpp"


#include "YTE/GameComponents/DialogueDirector.hpp"

namespace YTE
{
  YTEDefineEvent(DialogueStart);
  YTEDefineType(DialogueStart) { YTERegisterType(DialogueStart); }

  YTEDefineEvent(UISelectEvent);
  YTEDefineEvent(UIFocusSwitchEvent);
  YTEDefineEvent(UIDisplayEvent);
  YTEDefineEvent(UIUpdateContent);

  YTEDefineType(UISelectEvent) { YTERegisterType(UISelectEvent); }
  YTEDefineType(UIFocusSwitchEvent) { YTERegisterType(UIFocusSwitchEvent); }
  YTEDefineType(UIDisplayEvent) { YTERegisterType(UIDisplayEvent); }
  YTEDefineType(UIUpdateContent) { YTERegisterType(UIUpdateContent); }


  YTEDefineType(DialogueDirector)
  {
    YTERegisterType(DialogueDirector);
  }


  DialogueDirector::DialogueDirector(Composition *aOwner, Space *aSpace, RSValue *aProperties)
    : Component(aOwner, aSpace)
    , mDialogueSpace(nullptr)
    , mCameraAnchor(nullptr)
    , mLastSelectionIndex(-1)
    , mMaxSelectionIndex(-1)
  {
    DeserializeByType(aProperties, this, GetStaticType());
  }

  void DialogueDirector::Initialize()
  {
    mDialogueSpace = mSpace->AddChildSpace("MSR_Dialogue");

    auto children = mOwner->GetCompositions()->All();

    for (auto &child : children)
    {
      if (child.second->GetComponent<CameraAnchor>() != nullptr)
      {
        mCameraAnchor = child.second.get();
        break;
      }
    }

    mOwner->YTERegister(Events::CollisionStarted, this, &DialogueDirector::OnCollisionStart);
    mOwner->YTERegister(Events::CollisionEnded, this, &DialogueDirector::OnCollisionEnd);
  }

  void DialogueDirector::OnRequestDialogueStart(RequestDialogueStart *)
  {
    if (mCameraAnchor)
    {
      DirectCameraEvent directCam;
      mCameraAnchor->SendEvent(Events::DirectCameraEvent, &directCam);
    }

    BoatDockEvent dockEvent;
    mSpace->SendEvent(Events::BoatDockEvent, &dockEvent);

    DialogueStart startDialogue;
    mSpace->SendEvent(Events::DialogueStart, &startDialogue);

    mSpace->GetComponent<InputInterpreter>()->SetInputContext(InputInterpreter::InputContext::Dialogue);
  }

  void DialogueDirector::OnDialogueNodeReady(DialogueNodeReady *aEvent)
  {
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
        UIUpdateContent content(false, aEvent->ContentMessages[i]);
        content.SelectionIndex = i;
        mDialogueSpace->SendEvent(Events::UIUpdateContent, &content);

        UIDisplayEvent display(true);
        display.DisplayIndex = i;
        mDialogueSpace->SendEvent(Events::UIDisplayEvent, &display);

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

    float stickAngle = glm::acos(glm::dot(glm::vec2(1.f, 0.f), aEvent->StickDirection));
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
    }

    UISelectEvent select(mLastSelectionIndex);
    mDialogueSpace->SendEvent(Events::UISelectEvent, &select);
  }

  void DialogueDirector::OnDialogueConfirm(DialogueConfirm *)
  {
    if (mCurNodeType == DialogueNode::NodeType::Text)
    {
        // If there are more strings in this node
      if (mCurNodeDataIndex < mCurNodeData.size() - 1)
      {
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
        DialogueNodeConfirm confirm(mLastSelectionIndex);
        mSpace->SendEvent(Events::DialogueNodeConfirm, &confirm);

          // reset our options
        mLastSelectionIndex = -1;
        UISelectEvent select(mLastSelectionIndex);
        mDialogueSpace->SendEvent(Events::UISelectEvent, &select);
      }

      /*auto emitter = mOwner->GetComponent<WWiseEmitter>();

      if (emitter)
      {
        emitter->PlayEvent("UI_Dia_Next");
      }*/
    }
  }

  void DialogueDirector::OnDialogueExit(DialogueExit *)
  {
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
    if (aEvent->OtherObject->GetComponent<BoatController>())
    {
      mSpace->YTERegister(Events::RequestDialogueStart, this, &DialogueDirector::OnRequestDialogueStart);
      mSpace->YTERegister(Events::DialogueNodeReady, this, &DialogueDirector::OnDialogueNodeReady);
      mSpace->YTERegister(Events::DialogueSelect, this, &DialogueDirector::OnDialogueSelect);
      mSpace->YTERegister(Events::DialogueConfirm, this, &DialogueDirector::OnDialogueConfirm);
      mSpace->YTERegister(Events::DialogueExit, this, &DialogueDirector::OnDialogueExit);
    }
  }

  void DialogueDirector::OnCollisionEnd(CollisionEnded * aEvent)
  {
    if (aEvent->OtherObject->GetComponent<BoatController>())
    {
      mSpace->YTEDeregister(Events::RequestDialogueStart, this, &DialogueDirector::OnRequestDialogueStart);
      mSpace->YTEDeregister(Events::DialogueNodeReady, this, &DialogueDirector::OnDialogueNodeReady);
      mSpace->YTEDeregister(Events::DialogueSelect, this, &DialogueDirector::OnDialogueSelect);
      mSpace->YTEDeregister(Events::DialogueConfirm, this, &DialogueDirector::OnDialogueConfirm);
      mSpace->YTEDeregister(Events::DialogueExit, this, &DialogueDirector::OnDialogueExit);
    }
  }

}
