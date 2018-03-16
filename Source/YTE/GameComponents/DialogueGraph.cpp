/******************************************************************************/
/*!
\file   DialogueGraph.cpp
\author Jonathan Ackerman
\par    email: jonathan.ackerman\@digipen.edu
\date   2018 - 03 - 01

All content (c) 2018 DigiPen  (USA) Corporation, all rights reserved.
*/
/******************************************************************************/

#include "YTE/GameComponents/DialogueGraph.hpp"

namespace YTE
{
  YTEDefineEvent(AdvanceConversation);
  YTEDefineType(AdvanceConversation) { YTERegisterType(AdvanceConversation); }

	YTEDefineType(DialogueNode) { YTERegisterType(DialogueNode); }

  YTEDefineEvent(DialogueNodeReady);
  YTEDefineEvent(DialogueNodeConfirm);

  YTEDefineType(DialogueNodeReady) { YTERegisterType(DialogueNodeReady); }
  YTEDefineType(DialogueNodeConfirm) { YTERegisterType(DialogueNodeConfirm); }

  /*

  YTEDefineEvent(DialoguePrintText);
  YTEDefineType(DialoguePrintText) { YTERegisterType(DialoguePrintText); }

  YTEDefineEvent(DialoguePlayAnim);
  YTEDefineType(DialoguePlayAnim) { YTERegisterType(DialoguePlayAnim); }

  YTEDefineEvent(DialogueGetInput);
  YTEDefineType(DialogueGetInput) { YTERegisterType(DialogueGetInput); }

  YTEDefineType(JohnDialogue) { YTERegisterType(JohnDialogue); }
  */
  /*
  void DialogueNode::SetActiveNode(DialogueNodeEvent *aEvent)
  {
  mOwner->GetSpace()->YTEDeregister(Events::DialogueNodeEvent, this, &DialogueNode::SetActiveNode);
  mOwner->GetSpace()->YTERegister(Events::DialogueResponseEvent, this, &DialogueNode::Callback);
  switch (mType)
  {
  case NodeType::Text:
  {
  DialoguePrintText node;
  node.Data.push_back("here is the string data");
  mOwner->GetSpace()->SendEvent("DialoguePrintText", &node);
  }
  case NodeType::Anim:
  {
  DialoguePlayAnim node;
  node.Data.push_back("here is the string data");
  mOwner->GetSpace()->SendEvent("DialoguePlayAnim", &node);
  }
  case NodeType::Input:
  {
  DialogueGetInput node;
  node.Data.push_back("here is the string data");
  mOwner->GetSpace()->SendEvent("DialogueGetInput", &node);
  }
  }
  }

  void DialogueNode::ResponseCallback(DialogueResponseEvent *aEvent)
  {
  mOwner->GetSpace()->YTEDeregister(Events::DialogueResponseEvent, this, &DialogueNode::ResponseCallback);
  DialogueNodeEvent next;
  SendEvent("DialogueNodeEvent", &next);
  }
  */
  DialogueNode::DialogueNode(NodeType aType, std::vector<DialogueNode*> *aChildren, DialogueDataType *aData)
    : mType(aType), mData(*aData)
  {
    if (aChildren == nullptr)
    {
      mChildren = *(new std::vector<DialogueNode*>());
    }
    else
    {
      for (DialogueNode *child : *aChildren)
      {
        mChildren.push_back(child);
      }
    }

    switch (aType)
    {
      case NodeType::Anim:
      {
        // set the function pointer
        mNodeLogic = &DialogueNode::PlayAnim;
        break;
      }
      case NodeType::Input:
      {
        // set the function pointer
        mNodeLogic = &DialogueNode::GiveOptions;
        break;
      }
      case NodeType::Text:
      {
        // set the function pointer
        mNodeLogic = &DialogueNode::RunText;
        break;
      }
      case NodeType::Sound:
      {
        // set the function pointer
        mNodeLogic = &DialogueNode::PlaySound;
        break;
      }
    }
  }
  DialogueNode::DialogueNode(NodeType aType, std::vector<DialogueNode*> *aChildren, DialogueDataType *aData, Composition *aOwner, int aId)
    : mType(aType), mData(*aData), mOwner(aOwner), mId(aId)
  {
    if (aChildren == nullptr)
    {
      mChildren = *(new std::vector<DialogueNode*>());
    }
    else
    {
      for (DialogueNode *child : *aChildren)
      {
        mChildren.push_back(child);
      }
    }

    switch (aType)
    {
    case NodeType::Anim:
    {
      // set the function pointer
      mNodeLogic = &DialogueNode::PlayAnim;
      break;
    }
    case NodeType::Input:
    {
      // set the function pointer
      mNodeLogic = &DialogueNode::GiveOptions;
      break;
    }
    case NodeType::Text:
    {
      // set the function pointer
      mNodeLogic = &DialogueNode::RunText;
      break;
    }
    case NodeType::Sound:
    {
      // set the function pointer
      mNodeLogic = &DialogueNode::PlaySound;
      break;
    }
    }
  }
  /*
  DialogueNode::DialogueNode(NodeType aType, DialogueNode *aParent, int aStringCount, ...)
    : mType(aType), mParent(aParent)
  {
    mData = *(new std::vector<std::string>);
    va_list ap;
    va_start(ap, aStringCount);
    for (int i = 0; i < aStringCount; ++i)
    {
      mData.push_back(va_arg(ap, std::string));
    }
    va_end(ap);

    switch (aType)
    {
      case NodeType::Anim:
      {
          // set the function pointer
        mNodeLogic = &DialogueNode::PlayAnim;
        break;
      }
      case NodeType::Input:
      {
          // set the function pointer
        mNodeLogic = &DialogueNode::GiveOptions;
        break;
      }
      case NodeType::Text:
      {
          // set the function pointer
        mNodeLogic = &DialogueNode::RunText;
        break;
      }
      case NodeType::Sound:
      {
          // set the function pointer
        mNodeLogic = &DialogueNode::PlaySound;
        break;
      }
    }
  }
  */

	void DialogueNode::NextNode(DialogueNodeConfirm *aEvent)
	{
    auto space = mOwner->GetSpace();

    space->YTEDeregister(Events::DialogueNodeConfirm, this, &DialogueNode::NextNode);
   
    if (aEvent->Selection == mId)
    {
      mNodeLogic;
      for (auto child : mChildren)
      {
        // this needs to register on the space?
        space->YTERegister(Events::DialogueNodeConfirm, child, &DialogueNode::NextNode);
      }
    }
	}

	// Functors
  void DialogueNode::PlayAnim()
  {
    //mOwner->GetComponent<Animator>();
  }
  void DialogueNode::GiveOptions()
  {
    DialogueNodeReady options(mData);
    mOwner->GetSpace()->SendEvent(Events::DialogueNodeReady, &options);
  }
  void DialogueNode::RunText()
  {
    DialogueNodeReady options(mData);
    mOwner->GetSpace()->SendEvent(Events::DialogueNodeReady, &options);
  }
  void DialogueNode::PlaySound()
  {
    //mOwner->GetComponent<WWiseEmitter>()
  }

}//end yte